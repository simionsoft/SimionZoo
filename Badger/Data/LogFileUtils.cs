/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using System;
using System.Collections.Generic;
using System.Linq;
using Herd.Files;
using Badger.ViewModels;

namespace Badger.Data
{
    public class LogFileUtils
    {
        /// <summary>
        /// Gets the data of a variable from an episode using the parameters of the target track
        /// </summary>
        /// <param name="episode">The episode</param>
        /// <param name="trackParameters">The track parameters</param>
        /// <param name="variableIndex">Index of the variable</param>
        /// <returns>A Series objetct with the requested data or null if something fails</returns>
        public static Series GetVariableData(Log.Episode episode, Report trackParameters, int variableIndex)
        {

            if (episode.steps[episode.steps.Count - 1].episodeSimTime < trackParameters.MinEpisodeLength)
                return null;

            Series data = new Series();

            foreach (Log.Step step in episode.steps)
            {
                if (step.episodeSimTime >= trackParameters.TimeOffset && (trackParameters.TimeEnd == 0 || step.episodeSimTime <= trackParameters.TimeEnd))
                    data.AddValue(step.episodeSimTime
                     , ProcessFunc.Get(trackParameters.ProcessFunc, step.data[variableIndex]));
            }

            if (data.Values.Count == 0)
                return null;

            data.CalculateStats(trackParameters);

            if (trackParameters.Resample)
                data.Resample(trackParameters.NumSamples);
            return data;
        }

        /// <summary>
        /// Gets the average value of a variable in an episode using the track parameters
        /// </summary>
        /// <param name="episode">The episode</param>
        /// <param name="variableIndex">Index of the variable</param>
        /// <param name="trackParameters">The track parameters</param>
        /// <returns>The averaged value</returns>
        public static double GetEpisodeAverage(Log.Episode episode, int variableIndex, Report trackParameters)
        {
            double avg = 0.0;
            int count = 0;
            if (episode.steps.Count == 0) return 0.0;
            foreach (Log.Step step in episode.steps)
            {
                if (step.episodeSimTime >= trackParameters.TimeOffset && (trackParameters.TimeEnd == 0 || step.episodeSimTime <= trackParameters.TimeEnd))
                {
                    avg += ProcessFunc.Get(trackParameters.ProcessFunc, step.data[variableIndex]);
                    count++;
                }
            }
            return avg / count;
        }

        /// <summary>
        /// Gets the averaged data of the given variable from a list of episodes using the track parameters
        /// </summary>
        /// <param name="episodes">The episode list</param>
        /// <param name="trackParameters">The track parameters</param>
        /// <param name="variableIndex">Index of the variable</param>
        /// <returns>A SeriesGroup object with the requested data</returns>
        public static SeriesGroup GetAveragedData(List<Log.Episode> episodes, Report trackParameters, int variableIndex)
        {
            SeriesGroup data = new SeriesGroup(trackParameters);
            Series xYSeries = new Series();

            foreach (Log.Episode episode in episodes)
            {
                xYSeries.AddValue(episode.index
                    , GetEpisodeAverage(episode, variableIndex, trackParameters));
            }
            xYSeries.CalculateStats(trackParameters);
            if (trackParameters.Resample)
                xYSeries.Resample(trackParameters.NumSamples);
            data.AddSeries(xYSeries);
            return data;
        }

        /// <summary>
        /// Creates a Track object from a logged experimental unit and a list of reports
        /// </summary>
        /// <param name="expUnit">The logged experimental unit</param>
        /// <param name="reports">The list of reports we want</param>
        /// <returns></returns>
        public static Track LoadTrackData(LoggedExperimentalUnitViewModel expUnit, List<Report> reports)
        {
            Log.Data Log = new Log.Data();
            Log.Load(expUnit.LogFileName);

            if (!Log.SuccessfulLoad || Log.TotalNumEpisodes == 0) return null;

            Track track = new Track(expUnit.ForkValues, expUnit.LogFileName, expUnit.LogDescriptorFileName, expUnit.ExperimentFileName);
            SeriesGroup dataSeries;
            int variableIndex;
            foreach (Report report in reports)
            {
                variableIndex = expUnit.GetVariableIndex(report.Variable);
                switch (report.Type)
                {
                    case ReportType.LastEvaluation:
                        Log.Episode lastEpisode = Log.EvaluationEpisodes[Log.EvaluationEpisodes.Count - 1];
                        dataSeries = new SeriesGroup(report);
                        Series series = LogFileUtils.GetVariableData(lastEpisode, report, variableIndex);
                        if (series != null)
                        {
                            dataSeries.AddSeries(series);
                            track.AddVariableData(report, dataSeries);
                        }
                        break;
                    case ReportType.EvaluationAverages:
                        track.AddVariableData(report
                            , LogFileUtils.GetAveragedData(Log.EvaluationEpisodes, report, variableIndex));
                        break;
                    case ReportType.AllEvaluationEpisodes:
                    case ReportType.AllTrainingEpisodes:
                        dataSeries = new SeriesGroup(report);
                        List<Log.Episode> episodes;
                        if (report.Type == ReportType.AllEvaluationEpisodes)
                            episodes = Log.EvaluationEpisodes;
                        else episodes = Log.TrainingEpisodes;
                        foreach (Log.Episode episode in episodes)
                        {
                            Series subSeries = LogFileUtils.GetVariableData(episode, report, variableIndex);
                            if (subSeries != null)
                            {
                                subSeries.Id = episode.index.ToString();
                                dataSeries.AddSeries(subSeries);
                            }
                        }
                        track.AddVariableData(report, dataSeries);
                        break;
                }
            }
            return track;
        }
    }
}
