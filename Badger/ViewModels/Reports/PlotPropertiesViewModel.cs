using System.ComponentModel;
using Caliburn.Micro;
using OxyPlot;

namespace Badger.ViewModels
{
    public class PlotPropertiesViewModel : PropertyChangedBase
    {
        /*
         * plot properties:
         * x in/out
         * show legend
         * 
         * line series properties:
         * -visible
         * -[color, thickness,...]
         */

        //all-track selection 
        private bool m_bAllTracksSelected = true;
        public bool AllTracksSelected
        {
            get { return m_bAllTracksSelected; }
            set
            {
                m_bAllTracksSelected = value;
                NotifyOfPropertyChange(() => AllTracksSelected);
                foreach (PlotLineSeriesPropertiesViewModel trackProperties in m_lineSeriesProperties)
                    trackProperties.Visible = value;
            }
        }
        //per-track series properties
        private BindableCollection<PlotLineSeriesPropertiesViewModel> m_lineSeriesProperties
            = new BindableCollection<PlotLineSeriesPropertiesViewModel>();

        public BindableCollection<PlotLineSeriesPropertiesViewModel> LineSeriesProperties
        {
            get { return m_lineSeriesProperties; }
            set { m_lineSeriesProperties = value; NotifyOfPropertyChange(() => LineSeriesProperties); }
        }
        //public Dictionary<string,PlotLineSeriesPropertiesViewModel> LineSeriesPropertiesDic=
        //    new Dictionary<string,PlotLineSeriesPropertiesViewModel>();

        public void AddLineSeries(string name, OxyPlot.Series.LineSeries series)
        {
            PlotLineSeriesPropertiesViewModel newLineProperties 
                = new PlotLineSeriesPropertiesViewModel(name, series);
            LineSeriesProperties.Add(newLineProperties);
            newLineProperties.PropertyChanged += RaisePropertiesChangedEvent;
        }

        public bool PropertiesChanged { get; set; }
        public void RaisePropertiesChangedEvent(object sender, PropertyChangedEventArgs e)
        {
            NotifyOfPropertyChange(() => PropertiesChanged);
        }

        //LEGEND
        //Position
        private BindableCollection<string> m_legendPositions = new BindableCollection<string>();
        public BindableCollection<string> LegendPositions
        {
            get { return m_legendPositions; }
            set { m_legendPositions = value; NotifyOfPropertyChange(() => LegendPositions); }
        }
        private string m_selectedLegendPosition;
        public string SelectedLegendPosition
        {
            get { return m_selectedLegendPosition; }
            set { m_selectedLegendPosition = value; NotifyOfPropertyChange(() => SelectedLegendPosition); }
        }
        //Placement
        private BindableCollection<string> m_legendPlacements = new BindableCollection<string>();
        public BindableCollection<string> LegendPlacements
        {
            get { return m_legendPlacements; }
            set { m_legendPlacements = value; NotifyOfPropertyChange(() => LegendPlacements); }
        }
        private string m_selectedLegendPlacement;
        public string SelectedLegendPlacement
        {
            get { return m_selectedLegendPlacement; }
            set { m_selectedLegendPlacement = value; NotifyOfPropertyChange(() => SelectedLegendPlacement); }
        }
        //Orientation
        private BindableCollection<string> m_legendOrientations = new BindableCollection<string>();
        public BindableCollection<string> LegendOrientations
        {
            get { return m_legendOrientations; }
            set { m_legendOrientations = value; NotifyOfPropertyChange(() => LegendOrientations); }
        }
        private string m_selectedLegendOrientation;
        public string SelectedLegendOrientation
        {
            get { return m_selectedLegendOrientation; }
            set { m_selectedLegendOrientation = value; NotifyOfPropertyChange(() => SelectedLegendOrientation); }
        }
        //Visibility
        private bool m_bLegendVisible = true;
        public bool LegendVisible
        {
            get { return m_bLegendVisible; }
            set { m_bLegendVisible = value; NotifyOfPropertyChange(() => LegendVisible); }
        }
        private bool m_bLegendBorder = true;
        public bool LegendBorder
        {
            get { return m_bLegendBorder; }
            set { m_bLegendBorder = value; NotifyOfPropertyChange(() => LegendBorder); }
        }
        private bool m_bLegendSolidBackground = true;
        public bool LegendSolidBackground
        {
            get { return m_bLegendSolidBackground; }
            set { m_bLegendSolidBackground = value; NotifyOfPropertyChange(() => LegendSolidBackground); }
        }
        //Font
        private BindableCollection<string> m_fonts = new BindableCollection<string>();
        public BindableCollection<string> Fonts
        {
            get { return m_fonts; }
            set { m_fonts = value; NotifyOfPropertyChange(() => Fonts); }
        }
        private string m_selectedFont;
        public string SelectedFont
        {
            get { return m_selectedFont; }
            set { m_selectedFont = value;NotifyOfPropertyChange(() => SelectedFont); }
        }
        //Texts
        private string m_title = "N/A";
        public string Title
        {
            get { return m_title; }
            set { m_title = value; NotifyOfPropertyChange(() => Title); }
        }
        private string m_xAxisName = "N/A";
        public string XAxisName
        {
            get { return m_xAxisName; }
            set { m_xAxisName = value; NotifyOfPropertyChange(() => XAxisName); }
        }
        private string m_yAxisName = "N/A";
        public string YAxisName
        {
            get { return m_yAxisName; }
            set { m_yAxisName = value; NotifyOfPropertyChange(() => YAxisName); }
        }

        public PlotPropertiesViewModel()
        {
            m_legendPositions.Add(LegendPosition.BottomLeft.ToString());
            m_legendPositions.Add(LegendPosition.BottomRight.ToString());
            m_legendPositions.Add(LegendPosition.TopLeft.ToString());
            m_legendPositions.Add(LegendPosition.TopRight.ToString());
            SelectedLegendPosition = LegendPosition.TopRight.ToString();

            m_legendPlacements.Add(LegendPlacement.Inside.ToString());
            m_legendPlacements.Add(LegendPlacement.Outside.ToString());
            SelectedLegendPlacement = LegendPlacement.Inside.ToString();

            m_legendOrientations.Add(LegendOrientation.Horizontal.ToString());
            m_legendOrientations.Add(LegendOrientation.Vertical.ToString());
            SelectedLegendOrientation = LegendOrientation.Vertical.ToString();

            m_fonts.Add("Arial");
            m_fonts.Add("Segoe UI");
            m_fonts.Add("Times New Roman");
            m_fonts.Add("Verdana");
            SelectedFont = "Segoe UI";
        }

        /// <summary>
        ///     Apply some opacity to the original color of the LineSeries.  
        /// </summary>
        /// <param name="lineSeriesProperties"></param>
        public void dimLineSeriesColor(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        {
            OxyColor color = lineSeriesProperties.LineSeries.ActualColor;
            // Apply an opacity of 15% over the original color
            lineSeriesProperties.LineSeries.Color = OxyColor.FromArgb(0x26, color.R, color.G, color.B);
        }

        /// <summary>
        ///     Remove opacity in the original color of the LineSeries.  
        /// </summary>
        /// <param name="lineSeriesProperties"></param>
        public void removeLineSeriesColorOpacity(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        {
            OxyColor color = lineSeriesProperties.LineSeries.ActualColor;
            lineSeriesProperties.LineSeries.Color = OxyColor.FromRgb(color.R, color.G, color.B);
        }
    }
}
