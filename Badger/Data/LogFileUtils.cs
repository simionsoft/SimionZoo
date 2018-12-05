using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Herd.Files.Log;

namespace Badger.Data
{
    public class LogFileUtils
    {
        public static Series GetVariableData(EpisodesData episode, Report trackParameters, int variableIndex)
        {

            if (episode.steps[episode.steps.Count - 1].episodeSimTime < trackParameters.MinEpisodeLength)
                return null;

            Series data = new Series();

            foreach (StepData step in episode.steps)
            {
                if (step.episodeSimTime >= trackParameters.TimeOffset)
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

        public static double GetEpisodeAverage(EpisodesData episode, int variableIndex, Report trackParameters)
        {
            double avg = 0.0;
            int count = 0;
            if (episode.steps.Count == 0) return 0.0;
            foreach (StepData step in episode.steps)
            {
                if (step.episodeSimTime >= trackParameters.TimeOffset)
                {
                    avg += ProcessFunc.Get(trackParameters.ProcessFunc, step.data[variableIndex]);
                    count++;
                }
            }
            return avg / count;
        }

        public static SeriesGroup GetAveragedData(List<EpisodesData> episodes, Report trackParameters, int variableIndex)
        {
            SeriesGroup data = new SeriesGroup(trackParameters);
            Series xYSeries = new Series();

            foreach (EpisodesData episode in episodes)
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
    }
}
