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

        [DataMember]
        public OxyPlot.Series.LineSeries LineSeries { get; set; }

        public PlotLineSeriesPropertiesViewModel(string name, string description, OxyPlot.Series.LineSeries lineSeries)
        {
            LineSeries = lineSeries;
            Visible = true;
            Name = Utility.OxyPlotMathNotation(name);
            Description = description;
        }
    }
}
