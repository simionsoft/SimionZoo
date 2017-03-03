using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ReportViewModel : Screen
    {
        private string m_name= "Unnamed";
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
            set { m_selectedPlot = value;m_selectedPlot.updateView();  NotifyOfPropertyChange(() => selectedPlot); }
        }

        public void updateView() { }

        //stats
        private BindableCollection<StatsViewModel> m_stats = new BindableCollection<StatsViewModel>();
        public BindableCollection<StatsViewModel> stats
        {
            get { return m_stats; }
            set { m_stats = value;  NotifyOfPropertyChange(() => stats); }
        }
        private StatsViewModel m_selectedStat;
        public StatsViewModel selectedStat
        {
            get { return m_selectedStat; }
            set { m_selectedStat = value; NotifyOfPropertyChange(() => selectedStat); }
        }

        public void export(string outputFolder) { }

        private ReportsWindowViewModel m_parent = null;
        public ReportsWindowViewModel parent
        {
            get { return m_parent; }
            set { m_parent = value; }
        }

        private string getVariablePlotType(LogQuery query,string variable)
        {
            foreach (LoggedVariableViewModel var in query.loggedVariables)
            {
                if (var.name == variable)
                    return var.stateButton.state;
            }
            return null;
        }

        public ReportViewModel(LogQuery query)
        {
            //stats
            StatsViewModel newStat;
            foreach (string variable in query.variables)
            {
                newStat = new StatsViewModel(variable, this);
                foreach(LogQueryResultTrackViewModel track in query.resultTracks)
                {
                    TrackVariableData trackData = track.trackData.getVariableData(variable);
                    if (trackData != null)
                    {
                        //code below can be improved: organize forks hierarchically instead of a single string
                        StatViewModel trackStats = new StatViewModel(track.trackId);
                        trackStats.lastEpisodeStats = trackData.lastEpisodeData.stats;
                        trackStats.experimentStats = trackData.experimentData.stats;
                        newStat.addStat(trackStats);
                    }
                }

                stats.Add(newStat);
            }
            if (stats.Count > 0)
                selectedStat = stats[0];
            //plots
            foreach (string variable in query.variables)
            {
                PlotViewModel newPlot;

                int lineSeriesId;
                string plotType = getVariablePlotType(query, variable);

                if (plotType == LoggedVariableViewModel.PlotType.Last.ToString()
                    || plotType == LoggedVariableViewModel.PlotType.Both.ToString())
                { 
                    //LAST EVALUATION values: create a new plot for each variable in the result data          
                    newPlot = new PlotViewModel(variable, false, true);
                    //plot data

                    for (int i = 0; i < query.resultTracks.Count; i++)
                    {
                        TrackVariableData variableData = query.resultTracks[i].trackData.getVariableData(variable);
                        lineSeriesId = newPlot.addLineSeries(query.resultTracks[i].trackId);
                        for (int x = 0; x < query.resultTracks[i].trackData.realTime.Length; ++x)
                        {
                            newPlot.addLineSeriesValue(lineSeriesId, query.resultTracks[i].trackData.simTime[x]
                                , variableData.lastEpisodeData.values[x]);
                        }
                    }
                    plots.Add(newPlot);
                }
                if (plotType == LoggedVariableViewModel.PlotType.All.ToString()
                      || plotType == LoggedVariableViewModel.PlotType.Both.ToString())
                {
                    //AVERAGED EVALUATION values: create a new plot for each variable in the result data
                    newPlot = new PlotViewModel(variable + "-episode", false, true);
                    //plot data
                    for (int i = 0; i < query.resultTracks.Count; i++)
                    {
                        TrackVariableData variableData = query.resultTracks[i].trackData.getVariableData(variable);
                        lineSeriesId = newPlot.addLineSeries(query.resultTracks[i].trackId);
                        for (int x = 0; x < variableData.experimentData.values.Length; ++x)
                        {
                            newPlot.addLineSeriesValue(lineSeriesId, x
                                , variableData.experimentData.values[x]);
                        }
                    }
                    plots.Add(newPlot);
                }
            }
            if (plots.Count > 0)
            {
                selectedPlot = plots[0];
                selectedPlot.updateView();
            }
        }
        public void close()
        {
            if (m_parent != null)
                m_parent.close(this);
        }
    }
}
