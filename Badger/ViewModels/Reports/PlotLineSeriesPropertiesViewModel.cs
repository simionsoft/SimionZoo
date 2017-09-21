
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class PlotLineSeriesPropertiesViewModel : PropertyChangedBase
    {
        private bool m_bVisible;

        public bool bVisible
        {
            get { return m_bVisible; }
            set
            {
                m_bVisible = value;
                NotifyOfPropertyChange(() => bVisible);
                m_lineSeries.IsVisible = bVisible;
            }
        }
        private string m_name = "N/A";
        public string Name { get { return m_name; } set { m_name = value;NotifyOfPropertyChange(() => Name); } }

        OxyPlot.Series.LineSeries m_lineSeries;

        public OxyPlot.Series.LineSeries lineSeries { get { return m_lineSeries; } }

        public PlotLineSeriesPropertiesViewModel(string name, OxyPlot.Series.LineSeries lineSeries)
        {
            m_lineSeries = lineSeries;
            bVisible = true;
            Name = name;
        }
    }
}
