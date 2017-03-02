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
            set { m_selectedPlot = value;  NotifyOfPropertyChange(() => selectedPlot); }
        }

        public void updateView() { }
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
                                , variableData.lastEpisodeValues[x]);
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
                        for (int x = 0; x < variableData.avgEpisodeValues.Length; ++x)
                        {
                            newPlot.addLineSeriesValue(lineSeriesId, x
                                , variableData.avgEpisodeValues[x]);
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
