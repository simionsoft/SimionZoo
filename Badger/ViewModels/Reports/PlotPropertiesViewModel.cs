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

        public BindableCollection<PlotLineSeriesPropertiesViewModel> lineSeriesProperties
        {
            get { return m_lineSeriesProperties; }
            set { m_lineSeriesProperties = value; NotifyOfPropertyChange(() => lineSeriesProperties); }
        }

        // private OxyPlot.Series.LineSeries m_lineSeries;

        public void addLineSeries(OxyPlot.Series.LineSeries series, PlotViewModel parent)
        {
            // m_lineSeries = series;
            lineSeriesProperties.Add(new PlotLineSeriesPropertiesViewModel(series, parent));
        }

        private BindableCollection<string> m_legendOptions = new BindableCollection<string>();
        public BindableCollection<string> legendOptions
        {
            get { return m_legendOptions; }
            set { m_legendOptions = value; NotifyOfPropertyChange(() => legendOptions); }
        }
        public string selectedLegendOption { get; set; }
        private bool m_bLegendVisible = true;
        public bool bLegendVisible
        {
            get { return m_bLegendVisible; }
            set { m_bLegendVisible = value; NotifyOfPropertyChange(() => bLegendVisible); }
        }
        private bool m_bLegendBorder = true;
        public bool bLegendBorder
        {
            get { return m_bLegendBorder; }
            set { m_bLegendBorder = value; NotifyOfPropertyChange(() => m_bLegendBorder); }
        }

        public PlotPropertiesViewModel()
        {
            m_legendOptions.Add(OxyPlot.LegendPosition.BottomLeft.ToString());
            m_legendOptions.Add(OxyPlot.LegendPosition.BottomRight.ToString());
            m_legendOptions.Add(OxyPlot.LegendPosition.TopLeft.ToString());
            m_legendOptions.Add(OxyPlot.LegendPosition.TopRight.ToString());
            selectedLegendOption = OxyPlot.LegendPosition.TopRight.ToString();
        }

        //public void updateLineSeriesVisibility(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        //{
        //    m_plot.Series[lineSeriesProperties.lineSeriesId].IsVisible = lineSeriesProperties.bVisible;
        //    updateView();
        //}

        //public void soloLineSeries(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        //{
        //    foreach (PlotLineSeriesPropertiesViewModel seriesProperties in m_lineSeriesProperties)
        //    {
        //        seriesProperties.bVisible = (seriesProperties.lineSeriesId == lineSeriesProperties.lineSeriesId);
        //        m_plot.Series[seriesProperties.lineSeriesId].IsVisible = seriesProperties.bVisible;
        //    }
        //    updateView();
        //}

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
