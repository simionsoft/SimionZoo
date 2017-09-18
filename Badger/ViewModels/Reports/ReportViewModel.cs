using System;
using Caliburn.Micro;
using System.IO;
using Badger.Simion;
using System.Collections.Generic;
using Badger.Data;

namespace Badger.ViewModels
{
    public class ReportViewModel : Screen
    {
        private string m_name = "Unnamed";
        public string name { get { return m_name; } set { m_name = value; NotifyOfPropertyChange(() => name); } }

        //plots
        private BindableCollection<PlotViewModel> m_plots = new BindableCollection<PlotViewModel>();
        public BindableCollection<PlotViewModel> plots
        {
            get { return m_plots; }
            set { m_plots = value; NotifyOfPropertyChange(() => plots); }
        }
        private PlotViewModel m_selectedPlot = null;
        public PlotViewModel selectedPlot
        {
            get { return m_selectedPlot; }
            set
            {
                /*
                 * workarround to solve the exception:
                 * "This PlotModel is already in use by some other PlotView control."
                 * switching the tabs multiple times
                */
                if (m_selectedPlot != null)
                {
                    ((OxyPlot.IPlotModel)m_selectedPlot.Plot).AttachPlotView(null);
                }
                m_selectedPlot = value;
                m_selectedPlot.updateView();
                NotifyOfPropertyChange(() => selectedPlot);
            }
        }

        public void updateView() { }

        //stats
        private BindableCollection<StatsViewModel> m_stats = new BindableCollection<StatsViewModel>();
        public BindableCollection<StatsViewModel> stats
        {
            get { return m_stats; }
            set { m_stats = value; NotifyOfPropertyChange(() => stats); }
        }
        private StatsViewModel m_selectedStat;
        public StatsViewModel selectedStat
        {
            get { return m_selectedStat; }
            set { m_selectedStat = value; NotifyOfPropertyChange(() => selectedStat); }
        }

        public void export(string outputFolder)
        {
            //set culture as invariant to write numbers as in english
            System.Threading.Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            //export plots
            foreach (PlotViewModel plot in plots) plot.export(outputFolder);
            //export stats
            string statsFile = outputFolder + "\\" + name + ".xml";
            try
            {
                using (StreamWriter fileWriter = File.CreateText(statsFile))
                {
                    fileWriter.WriteLine("<" + XMLConfig.statisticsFileTag + ">");
                    foreach (StatsViewModel stat in stats) stat.export(fileWriter, "  ");
                    fileWriter.WriteLine("</" + XMLConfig.statisticsFileTag + ">");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error exporting stats file: " + statsFile);
                Console.Write(ex.ToString());
            }
        }

        private PlotType getVariablePlotType(LogQuery query, string variable)
        {
            foreach (LoggedVariableViewModel var in query.loggedVariables)
            {
                if (var.name == variable)
                    return var.SelectedPlotType;
            }
            return PlotType.Undefined;
        }

        public ReportViewModel(LogQuery query, ReportParams report)
        {
            name = report.Name;

            //Create the plot
            PlotViewModel newPlot = new PlotViewModel(report.Name, 0, "Time (s)", report.Name, false, true);
            //Create the stats
            StatsViewModel newStatGroup = new StatsViewModel(report.Name);

            foreach (LogQueryResultTrackViewModel Track in query.ResultTracks)
            {
                //plot data
                DataSeries data = Track.ResultTrackData.Data[report];

                foreach (XYSeries subSeries in data.SubSeries)
                {
                    string subSeriesName;
                    if (data.SubSeries.Count == 0)
                        subSeriesName = Track.GroupId;
                    else subSeriesName = Track.GroupId + "-" + subSeries.Id;

                    //add data to the plot
                    int lineSeriesId = newPlot.addLineSeries(subSeriesName);
                    foreach (XYValue value in subSeries.Values)
                        newPlot.addLineSeriesValue(lineSeriesId, value.X, value.Y);

                    StatViewModel newStat = new StatViewModel(Track.ExperimentId, subSeriesName, subSeries.Stats);

                    newStatGroup.addStat(newStat);
                }
            }
            plots.Add(newPlot);
            stats.Add(newStatGroup);

            
            
        
            ////stats
            //StatsViewModel newStat;
            //foreach (string variable in query.variables)
            //{
            //    newStat = new StatsViewModel(variable);
            //    foreach (LogQueryResultTrackViewModel track in query.resultTracks)
            //    {
            //        TrackVariableData trackData = track.trackData.GetVariableData(variable);
            //        if (trackData != null)
            //        {
            //            //code below can be improved: organize forks hierarchically instead of a single string
            //            StatViewModel trackStats = new StatViewModel(track.ExperimentId, track.TrackId);
            //            trackStats.lastEpisodeStats = trackData.lastEvaluationEpisodeData.Stats;
            //            trackStats.experimentStats = trackData.experimentAverageData.Stats;
            //            newStat.addStat(trackStats);
            //        }
            //    }

            //    stats.Add(newStat);
            //}
            //if (stats.Count > 0)
            //    selectedStat = stats[0];

            ////plots
            //foreach (string variable in query.variables)
            //{
            //    PlotViewModel newPlot;

            //    int lineSeriesId;
            //    PlotType plotType = getVariablePlotType(query, variable);

            //    if ((plotType & PlotType.LastEvaluation) == PlotType.LastEvaluation)
            //    {
            //        //get the max length of the log
            //        double maxLength = 0.0;
            //        if (query.resultTracks.Count > 0)
            //            maxLength = query.resultTracks.Select(a => a.trackData.realTime.Length).Max();

            //        //LAST EVALUATION values: create a new plot for each variable in the result data          
            //        newPlot = new PlotViewModel(String.Format("Last Evaluation\n{0}", variable), maxLength, "Time (s)", variable, false, true);
            //        //plot data

            //        for (int i = 0; i < query.resultTracks.Count; i++)
            //        {
            //            TrackVariableData variableData = query.resultTracks[i].trackData.GetVariableData(variable);
            //            lineSeriesId = newPlot.addLineSeries(query.resultTracks[i].GroupId);
            //            for (int x = 0; x < variableData.lastEvaluationEpisodeData.YAxisValues.Length; ++x)
            //            {
            //                newPlot.addLineSeriesValue(lineSeriesId, query.resultTracks[i].trackData.simTime[x]
            //                    , variableData.lastEvaluationEpisodeData.YAxisValues[x]);
            //            }
            //        }
            //        plots.Add(newPlot);
            //    }

            //    if ((plotType & PlotType.AllEvaluationEpisodes) == PlotType.AllEvaluationEpisodes)
            //    {
            //        //get the max length of the log
            //        double maxLength = 0.0;
            //        if (query.resultTracks.Count > 0)
            //            maxLength = query.resultTracks.Select(a => a.trackData.realTime.Length).Max();

            //        //ALL EVALUATION values         
            //        newPlot = new PlotViewModel(String.Format("Evaluation Episodes\n{0}", variable), maxLength, "Time (s)", variable, false, true);
            //        //plot data

            //        bool isFirstSeries = true;
            //        for (int i = 0; i < query.resultTracks.Count; i++)
            //        {
            //            TrackVariableData variableData = query.resultTracks[i].trackData.GetVariableData(variable);

            //            int episodeIndex = 0;
            //            foreach (var episodeData in variableData.experimentEvaluationData)
            //            {
            //                lineSeriesId = newPlot.addLineSeries(String.Format("track {0}\nepisode {1}", i, episodeIndex++), isFirstSeries);
            //                isFirstSeries = false;
            //                if (episodeData.Initialized)
            //                {
            //                    for (int x = 0; x < episodeData.YAxisValues.Length; x++)
            //                    {
            //                        newPlot.addLineSeriesValue(lineSeriesId, query.resultTracks[i].trackData.simTime[x], episodeData.YAxisValues[x]);
            //                    }
            //                }
            //            }
            //        }
            //        plots.Add(newPlot);
            //    }

            //    if ((plotType & PlotType.AllTrainingEpisodes) == PlotType.AllTrainingEpisodes)
            //    {
            //        //get the max length of the log
            //        double maxLength = 0.0;
            //        if (query.resultTracks.Count > 0)
            //            maxLength = query.resultTracks.Select(a => a.trackData.realTime.Length).Max();

            //        //LAST EVALUATION values: create a new plot for each variable in the result data          
            //        newPlot = new PlotViewModel(String.Format("Training Episodes:\n{0}", variable), maxLength, "Time (s)", variable, false, true);
            //        //plot data

            //        bool isFirstSeries = true;
            //        for (int i = 0; i < query.resultTracks.Count; i++)
            //        {
            //            TrackVariableData variableData = query.resultTracks[i].trackData.GetVariableData(variable);

            //            int episodeIndex = 0;
            //            foreach (var episodeData in variableData.experimentTrainingData)
            //            {
            //                if (episodeData.Initialized)
            //                {
            //                    lineSeriesId = newPlot.addLineSeries(String.Format("track {0}\nepisode {1}", i, episodeIndex++), isFirstSeries);
            //                    isFirstSeries = false;

            //                    for (int x = 0; x < episodeData.YAxisValues.Length; ++x)
            //                    {
            //                        newPlot.addLineSeriesValue(lineSeriesId, query.resultTracks[i].trackData.simTime[x], episodeData.YAxisValues[x]);
            //                    }
            //                }
            //            }
            //        }
            //        plots.Add(newPlot);
            //    }

            //    if ((plotType & PlotType.EvaluationAverages) == PlotType.EvaluationAverages)
            //    {
            //        //get the max length of the log
            //        double maxLength = 0.0;
            //        if (query.resultTracks.Count > 0)
            //            maxLength = query.resultTracks.Select(a => a.trackData.GetVariableData(variable).experimentAverageData.YAxisValues.Length).Max();

            //        //AVERAGED EVALUATION values: create a new plot for each variable in the result data
            //        newPlot = new PlotViewModel(String.Format("Each Experiment's Average of\n{0}", variable), maxLength, "Episode", String.Format("Average of {0}", variable), false, true);
            //        //plot data
            //        for (int i = 0; i < query.resultTracks.Count; i++)
            //        {
            //            TrackVariableData variableData = query.resultTracks[i].trackData.GetVariableData(variable);
            //            if (variableData.experimentAverageData.Initialized)
            //            {
            //                lineSeriesId = newPlot.addLineSeries(query.resultTracks[i].GroupId);
            //                for (int x = 0; x < variableData.experimentAverageData.YAxisValues.Length; ++x)
            //                {
            //                    newPlot.addLineSeriesValue(lineSeriesId, x
            //                        , variableData.experimentAverageData.YAxisValues[x]);
            //                }
            //            }
            //        }
            //        plots.Add(newPlot);
            //    }
            //}

            //TODO: Add support for other plottypes!

            if (plots.Count > 0)
            {
                selectedPlot = plots[0];
                selectedPlot.updateView();
            }
        }
    }
}
