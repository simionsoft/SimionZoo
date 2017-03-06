
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class PlotLineSeriesPropertiesViewModel: PropertyChangedBase
    {
        private bool m_bVisible = true;
        public bool bVisible { get { return m_bVisible; }
            set {
                m_bVisible = value;
                NotifyOfPropertyChange(() => bVisible);
                m_series.IsVisible= bVisible; } }

        private string m_name;
        public string name { get { return m_name; } }

        OxyPlot.Series.LineSeries m_series;

        private PlotViewModel m_parent= null;
        public PlotLineSeriesPropertiesViewModel(string lineSeriesName, OxyPlot.Series.LineSeries series,PlotViewModel parentPlot)
        {
            m_name = lineSeriesName;
            m_series = series;
            m_parent = parentPlot;
        }
    }
}
