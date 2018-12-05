using Badger.Data;
using Caliburn.Micro;
using System.Runtime.Serialization;

namespace Badger.ViewModels
{
    [DataContract]
    public class PlotLineSeriesPropertiesViewModel : PropertyChangedBase
    {
        private bool m_bVisible;
        [DataMember]
        public bool Visible
        {
            get { return m_bVisible; }
            set
            {
                m_bVisible = value;
                if (LineSeries!=null)
                    LineSeries.IsVisible = Visible;
                NotifyOfPropertyChange(() => Visible);
            }
        }
        private string m_name = "N/A";
        [DataMember]
        public string Name
        {
            get { return m_name; }
            set
            {
                m_name = value;
                NotifyOfPropertyChange(() => Name);
                if (LineSeries!=null)
                    LineSeries.Title = value;
            }
        }
        private string m_description = "N/A";
        [DataMember]
        public string Description
        {
            get { return m_description; }
            set
            {
                m_description = value;
                NotifyOfPropertyChange(() => Description);
            }
        }

        OxyPlot.Series.LineSeries m_lineSeries = null;
        public OxyPlot.Series.LineSeries LineSeries
        {
            get { return m_lineSeries; }
            set
            {
                m_lineSeries = value;
                NotifyOfPropertyChange(() => Visible);
                NotifyOfPropertyChange(() => Description);
                NotifyOfPropertyChange(() => Name);
            }
        }

        public PlotLineSeriesPropertiesViewModel(string name, string description, OxyPlot.Series.LineSeries lineSeries)
        {
            LineSeries = lineSeries;
            Visible = true;
            Name = Herd.Utils.OxyPlotMathNotation(name);
            Description = description;
        }
    }
}
