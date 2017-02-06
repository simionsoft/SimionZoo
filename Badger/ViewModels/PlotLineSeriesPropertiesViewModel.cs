
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
                m_parent.updateLineSeriesVisibility(this); } }

        private string m_name;
        public string name { get { return m_name; } }

        private int m_lineSeriesId;
        public int lineSeriesId { get { return m_lineSeriesId; }set { } }

        private PlotViewModel m_parent= null;
        public PlotLineSeriesPropertiesViewModel(string lineSeriesName, int id,PlotViewModel parentPlot)
        {
            m_name = lineSeriesName;
            m_lineSeriesId = id;
            m_parent = parentPlot;
        }

        public void solo()
        {
            m_parent.soloLineSeries(this);
        }
    }
}
