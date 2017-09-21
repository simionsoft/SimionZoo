using System;
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

        //per-line series properties
        private BindableCollection<PlotLineSeriesPropertiesViewModel> m_lineSeriesProperties
            = new BindableCollection<PlotLineSeriesPropertiesViewModel>();

        public BindableCollection<PlotLineSeriesPropertiesViewModel> LineSeriesProperties
        {
            get { return m_lineSeriesProperties; }
            set { m_lineSeriesProperties = value; NotifyOfPropertyChange(() => LineSeriesProperties); }
        }

        public void AddLineSeries(string name, OxyPlot.Series.LineSeries series)
        {
            LineSeriesProperties.Add(new PlotLineSeriesPropertiesViewModel(name, series));
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
            set { LegendVisible = value; NotifyOfPropertyChange(() => LegendVisible); }
        }
        private bool m_bLegendBorder = true;
        public bool LegendBorder
        {
            get { return m_bLegendBorder; }
            set { LegendBorder = value; NotifyOfPropertyChange(() => LegendBorder); }
        }

        public PlotPropertiesViewModel()
        {
            m_legendPositions.Add(OxyPlot.LegendPosition.BottomLeft.ToString());
            m_legendPositions.Add(OxyPlot.LegendPosition.BottomRight.ToString());
            m_legendPositions.Add(OxyPlot.LegendPosition.TopLeft.ToString());
            m_legendPositions.Add(OxyPlot.LegendPosition.TopRight.ToString());
            SelectedLegendPosition = OxyPlot.LegendPosition.TopRight.ToString();

            m_legendPlacements.Add(OxyPlot.LegendPlacement.Inside.ToString());
            m_legendPlacements.Add(OxyPlot.LegendPlacement.Outside.ToString());
            SelectedLegendPlacement = OxyPlot.LegendPlacement.Inside.ToString();

            m_legendOrientations.Add(OxyPlot.LegendOrientation.Horizontal.ToString());
            m_legendOrientations.Add(OxyPlot.LegendOrientation.Vertical.ToString());
            SelectedLegendOrientation = OxyPlot.LegendOrientation.Vertical.ToString();
        }

        /// <summary>
        ///     Apply some opacity to the original color of the LineSeries.  
        /// </summary>
        /// <param name="lineSeriesProperties"></param>
        public void dimLineSeriesColor(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        {
            OxyColor color = lineSeriesProperties.lineSeries.ActualColor;
            // Apply an opacity of 15% over the original color
            lineSeriesProperties.lineSeries.Color = OxyColor.FromArgb(0x26, color.R, color.G, color.B);
        }

        /// <summary>
        ///     Remove opacity in the original color of the LineSeries.  
        /// </summary>
        /// <param name="lineSeriesProperties"></param>
        public void removeLineSeriesColorOpacity(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        {
            OxyColor color = lineSeriesProperties.lineSeries.ActualColor;
            lineSeriesProperties.lineSeries.Color = OxyColor.FromRgb(color.R, color.G, color.B);
        }
    }
}
