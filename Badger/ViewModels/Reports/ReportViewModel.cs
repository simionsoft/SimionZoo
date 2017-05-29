using System;
using Caliburn.Micro;
using System.IO;
using Badger.Simion;

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
            set { m_selectedPlot = value; m_selectedPlot.updateView(); NotifyOfPropertyChange(() => selectedPlot); }
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
                    fileWriter.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error exporting stats file: " + statsFile);
                Console.Write(ex.ToString());
            }
        }

        private ReportsWindowViewModel m_parent { get; set; }

        private string getVariablePlotType(LogQuery query, string variable)
        {
            foreach (LoggedVariableViewModel var in query.loggedVariables)
            {
                if (var.name == variable)
                    return var.stateButton.state;
            }
            return null;
        }

        public ReportViewModel(LogQuery query, ReportsWindowViewModel parent)
        {
            m_parent = parent;
            
            loadAllStats(query);
            if (stats.Count > 0)
                selectedStat = stats[0];

            loadAllPlots(query);
            if (plots.Count > 0)
            {
                selectedPlot = plots[0];
                selectedPlot.updateView();
            }
        }

        private void loadAllStats(LogQuery query)
        {
            StatsViewModel newStatsViewModel;
            foreach (string variable in query.variables)
            {
                newStatsViewModel = new StatsViewModel(variable);
                int trackId = 0;
                foreach (LogQueryResultTrackViewModel track in query.resultTracks)
                {
                    TrackVariableData trackData = track.trackData.getVariableData(variable);
                    if (trackData != null)
                    {
                        StatViewModel trackStats = new StatViewModel(track.groupId, trackId.ToString());
                        trackStats.lastEpisodeStats = trackData.lastEpisodeData.stats;
                        trackStats.experimentStats = trackData.experimentData.stats;
                        newStatsViewModel.addOrganized(trackStats);
                        trackId++;
                    }
                }
                stats.Add(newStatsViewModel);
            }
        }

        private void loadAllPlots(LogQuery query)
        {
            foreach (string variable in query.variables)
            {
                PlotViewModel newPlotViewModel;

                int lineSeriesId;
                string plotType = getVariablePlotType(query, variable);

                if (plotType == LoggedVariableViewModel.PlotType.Last.ToString()
                    || plotType == LoggedVariableViewModel.PlotType.Both.ToString())
                {
                    //LAST EVALUATION values: create a new plot for each variable in the result data          
                    newPlotViewModel = new PlotViewModel(variable, false, true);
                    //plot data

                    for (int i = 0; i < query.resultTracks.Count; i++)
                    {
                        TrackVariableData variableData = query.resultTracks[i].trackData.getVariableData(variable);
                        //lineSeriesId = newPlotViewModel.addLineSeries(query.resultTracks[i].groupId);
                        //It's best to show the id than the full groupId name
                        lineSeriesId = newPlotViewModel.addLineSeries(i.ToString());
                        for (int x = 0; x < query.resultTracks[i].trackData.realTime.Length; ++x)
                        {
                            newPlotViewModel.addLineSeriesValue(lineSeriesId, query.resultTracks[i].trackData.simTime[x]
                                , variableData.lastEpisodeData.values[x]);
                        }
                    }
                    plots.Add(newPlotViewModel);
                }
                if (plotType == LoggedVariableViewModel.PlotType.All.ToString()
                      || plotType == LoggedVariableViewModel.PlotType.Both.ToString())
                {
                    //AVERAGED EVALUATION values: create a new plot for each variable in the result data
                    newPlotViewModel = new PlotViewModel(variable + "-episode", false, true);
                    //plot data
                    for (int i = 0; i < query.resultTracks.Count; i++)
                    {
                        TrackVariableData variableData = query.resultTracks[i].trackData.getVariableData(variable);
                        lineSeriesId = newPlotViewModel.addLineSeries(query.resultTracks[i].groupId);
                        for (int x = 0; x < variableData.experimentData.values.Length; ++x)
                        {
                            newPlotViewModel.addLineSeriesValue(lineSeriesId, x
                                , variableData.experimentData.values[x]);
                        }
                    }
                    plots.Add(newPlotViewModel);
                }
            }
        }

        public void close()
        {
            if (m_parent != null)
                m_parent.close(this);
        }
    }
}
