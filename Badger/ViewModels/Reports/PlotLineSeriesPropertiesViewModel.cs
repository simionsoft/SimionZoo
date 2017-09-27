using Badger.Data;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class PlotLineSeriesPropertiesViewModel : PropertyChangedBase
    {
        private bool m_bVisible;

        public bool Visible
        {
            get { return m_bVisible; }
            set
            {
                m_bVisible = value;
                NotifyOfPropertyChange(() => Visible);
                m_lineSeries.IsVisible = Visible;
            }
        }
        private string m_name = "N/A";
        public string Name
        {
            get { return m_name; }
            set
            {
                m_name = value;
                NotifyOfPropertyChange(() => Name);
                m_lineSeries.Title = value;
            }
        }

        OxyPlot.Series.LineSeries m_lineSeries;

        public OxyPlot.Series.LineSeries LineSeries { get { return m_lineSeries; } }

        public PlotLineSeriesPropertiesViewModel(string name, OxyPlot.Series.LineSeries lineSeries)
        {
            m_lineSeries = lineSeries;
            Visible = true;
            Name = Utility.OxyPlotMathNotation(name);
        }
    }
}
