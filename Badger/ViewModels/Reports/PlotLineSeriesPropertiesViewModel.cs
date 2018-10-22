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
                m_lineSeries.IsVisible = Visible;
                NotifyOfPropertyChange(() => Visible);
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
        private string m_description = "N/A";
        public string Description
        {
            get { return m_description; }
            set
            {
                m_description = value;
                NotifyOfPropertyChange(() => Description);
            }
        }

        OxyPlot.Series.LineSeries m_lineSeries;

        public OxyPlot.Series.LineSeries LineSeries { get { return m_lineSeries; } }

        public PlotLineSeriesPropertiesViewModel(string name, string description, OxyPlot.Series.LineSeries lineSeries)
        {
            m_lineSeries = lineSeries;
            Visible = true;
            Name = Utility.OxyPlotMathNotation(name);
            Description = description;
        }
    }
}
