
using System.Collections.Generic;
using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Wpf;
using System.Threading;
using Caliburn.Micro;

namespace Badger.ViewModels
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
    public class PlotViewModel: PropertyChangedBase
    {
        string m_name = "";
        public string name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }
        private const int m_updateFreq= 1000; //plot refresh freq in millseconds
        private Timer m_timer;

        double m_minX = double.MaxValue;
        double m_maxX = double.MinValue;
        double m_minY = double.MaxValue;
        double m_maxY = double.MinValue;

        private PlotModel m_plot;
        public PlotModel plot { get { return m_plot; } set { } }

        private List<PlotLineSeriesPropertiesViewModel> m_lineSeriesProperties;
        public List<PlotLineSeriesPropertiesViewModel> lineSeriesProperties { get { return m_lineSeriesProperties; } set { } }

        public PlotViewModel(string title, bool bRefresh=true, bool bShowOptions=false)
        {
            name= title;
            m_plot = new PlotModel { Title=title};
            var xAxis = new OxyPlot.Axes.LinearAxis();
            xAxis.Position = AxisPosition.Bottom;
            xAxis.MajorGridlineStyle = LineStyle.Solid;
            xAxis.Minimum = 0.0;
            xAxis.Maximum = 1.0;
            m_plot.Axes.Add(xAxis);
            var yAxis = new OxyPlot.Axes.LinearAxis();
            yAxis.Position = AxisPosition.Left;
            yAxis.MajorGridlineStyle = LineStyle.Solid;
            yAxis.Minimum = 0.0;
            yAxis.Maximum = 1.0;
            m_plot.Axes.Add(yAxis);

            if (bRefresh)
            {
                m_timer = new Timer(updatePlot);
                m_timer.Change(m_updateFreq, m_updateFreq);
            }
            m_bShowOptions = bShowOptions;
            if (bShowOptions)
            {
                m_lineSeriesProperties = new List<PlotLineSeriesPropertiesViewModel>();
            }
        }

        private void updatePlot(object state)
        {
            m_plot.InvalidatePlot(true);
        }

        public void updateView()
        {
            m_plot.InvalidatePlot(true);
        }
        
        public int addLineSeries(string title)
        {
            var newSeries = new OxyPlot.Series.LineSeries { Title = title, MarkerType = MarkerType.None };
            m_plot.Series.Add(newSeries);

            if (bShowOptions)
            {
                //if options are displayed in this view, then we have to store the properties
                m_lineSeriesProperties.Add(new PlotLineSeriesPropertiesViewModel(title, m_plot.Series.Count - 1, this));
            }

            return m_plot.Series.Count-1;
        }
        public void addLineSeriesValue(int seriesIndex,double xValue, double yValue)
        {
            if (seriesIndex < 0 || seriesIndex >= m_plot.Series.Count)
            {
                //at least, we should log the error   
                return;
            }

            OxyPlot.Series.LineSeries series = (OxyPlot.Series.LineSeries) m_plot.Series[seriesIndex];
            updatePlotBounds(xValue, yValue);
            series.Points.Add(new DataPoint(xValue,yValue));
        }
        private void updatePlotBounds(double x,double y)
        {
            bool bMustUpdate = false;
            if (x < m_minX)
            {
                m_minX = x;
                bMustUpdate = true;
            }
            if (x > m_maxX)
            {
                m_maxX = x;
                bMustUpdate = true;
            }
            if (y<m_minY)
            {
                m_minY = y;
                bMustUpdate = true;
            }
            if (y>m_maxY)
            {
                m_maxY = y;
                bMustUpdate = true;
            }

            if (bMustUpdate)
            {
                double maxX = m_maxX;
                double minX = m_minX;
                double maxY = m_maxY;
                double minY = m_minY;
                if (maxX - minX == 0.0) { minX -= 0.01; maxX += 0.01; }
                if (maxY - minY==0.0) { minY -= 0.01; maxY += 0.01; }
                m_plot.Axes[0].Maximum = maxX;
                m_plot.Axes[0].Minimum = minX;
                m_plot.Axes[1].Maximum= maxY;
                m_plot.Axes[1].Minimum= minY;
            }
        }
        public void export(string outputFolder)
        {
            string fileName;
            //as png
            fileName = outputFolder + "\\" + name + ".png";
            var pngExporter = new PngExporter { Width = 600, Height = 400, Background = OxyColors.Transparent };
            pngExporter.ExportToFile(m_plot, fileName);
            //as svg
            fileName = outputFolder + "\\" + name + ".svg";
            var svgExporter = new OxyPlot.Wpf.SvgExporter { Width = 600, Height = 400 };
            svgExporter.ExportToFile(m_plot, fileName);
        }

        private bool m_bShowOptions = false;
        public bool bShowOptions { get { return m_bShowOptions; } set { m_bShowOptions = value; NotifyOfPropertyChange(() => bShowOptions); } }

        public void updateLineSeriesVisibility(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        {
            m_plot.Series[lineSeriesProperties.lineSeriesId].IsVisible= lineSeriesProperties.bVisible;
            updateView();
        }

        public void soloLineSeries(PlotLineSeriesPropertiesViewModel lineSeriesProperties)
        {
            foreach(PlotLineSeriesPropertiesViewModel seriesProperties in m_lineSeriesProperties)
            {
                seriesProperties.bVisible = (seriesProperties.lineSeriesId == lineSeriesProperties.lineSeriesId);
                m_plot.Series[seriesProperties.lineSeriesId].IsVisible = seriesProperties.bVisible;
            }
            updateView();
        }
    }
}
