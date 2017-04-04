
using Caliburn.Micro;
using OxyPlot;

namespace Badger.ViewModels
{
    public class PlotLineSeriesPropertiesViewModel: PropertyChangedBase
    {
        private bool m_bVisible = true;

        public bool bVisible { get { return m_bVisible; }
            set {
                m_bVisible = value;
                NotifyOfPropertyChange(() => bVisible);
                m_lineSeries.IsVisible= bVisible; } }

        private string m_name;

        public string name { get { return m_name; } }

        OxyPlot.Series.LineSeries m_lineSeries;

        public OxyPlot.Series.LineSeries lineSeries { get { return m_lineSeries; } }

        private PlotViewModel m_parent= null;

        public PlotLineSeriesPropertiesViewModel(string lineSeriesName, OxyPlot.Series.LineSeries lineSeries, PlotViewModel parentPlot)
        {
            m_name = lineSeriesName;
            m_lineSeries = lineSeries;
            m_parent = parentPlot;
        }
    }
}
