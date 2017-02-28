using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ReportViewModel : Screen
    {
        private string m_name;
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
        public ReportViewModel(LogQuery query)
        {
            foreach(string variable in query.variables)
            {
                //create a new plot for each variable in the result data
                PlotViewModel newPlot = new PlotViewModel(variable, false, true);
                //plot data
                int lineSeriesId;
                for(int i= 0; i<query.resultTracks.Count; i++)
                {
                    lineSeriesId= newPlot.addLineSeries(query.resultTracks[i].trackId);
                    for (int x = 0; x < query.resultTracks[i].trackData.realTime.Length; ++x)
                    {
                        newPlot.addLineSeriesValue(lineSeriesId, query.resultTracks[i].trackData.simTime[x]
                            , query.resultTracks[i].trackData.variables[variable].data[x]);
                    }
                }
                plots.Add(newPlot);             
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
