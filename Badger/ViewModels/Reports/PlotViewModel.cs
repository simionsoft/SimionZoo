
using System.Collections.Generic;
using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Wpf;
using System.Threading;
using Caliburn.Micro;
using System.Windows.Forms;
using System.IO;
using Badger.Data;

namespace Badger.ViewModels
{
    public class PlotViewModel : PropertyChangedBase
    {
        string m_name = "";

        public string name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }
        private const int m_updateFreq = 1000; //plot refresh freq in millseconds
        private System.Threading.Timer m_timer;

        double m_minX = double.MaxValue;
        double m_maxX = double.MinValue;
        double m_minY = double.MaxValue;
        double m_maxY = double.MinValue;

        private PlotModel m_plot;
        public PlotModel plot { get { return m_plot; } set { } }

        private PlotPropertiesViewModel m_properties = new PlotPropertiesViewModel();
        public PlotPropertiesViewModel properties
        {
            get { return m_properties; }
        }

        public PlotViewModel(string title, bool bRefresh = true, bool bShowOptions = false)
        {
            name = title;
            m_plot = new PlotModel { Title = title };
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
                m_timer = new System.Threading.Timer(updatePlot);
                m_timer.Change(m_updateFreq, m_updateFreq);
            }
            m_bShowOptions = bShowOptions;

        }

        private void updatePlot(object state)
        {
            m_plot.InvalidatePlot(true);
        }

        public void updateView()
        {
            m_plot.InvalidatePlot(true);
        }

        private object m_lineSeriesLock = new object();

        public int addLineSeries(string title)
        {
            lock (m_lineSeriesLock)
            {
                OxyPlot.Series.LineSeries newSeries =
                    new OxyPlot.Series.LineSeries { Title = title, MarkerType = MarkerType.None };
                m_plot.Series.Add(newSeries);

                properties.addLineSeries(newSeries, this);

                return m_plot.Series.Count - 1; ;
            }
        }

        public void addLineSeriesValue(int seriesIndex, double xValue, double yValue)
        {
            if (seriesIndex < 0 || seriesIndex >= m_plot.Series.Count)
            {
                //at least, we should log the error   
                return;
            }

            OxyPlot.Series.LineSeries series = (OxyPlot.Series.LineSeries)m_plot.Series[seriesIndex];
            updatePlotBounds(xValue, yValue);
            series.Points.Add(new DataPoint(xValue, yValue));
        }

        /// <summary>
        ///     Identify which LineSeries is hovered and make a call to the dimLineSeriesColor method 
        ///     passing the correct LineSeriesProperties object as parameter.
        ///     In order to highlight a LineSeries what we actually do is to dim, that is, apply
        ///     certain opacity, to all the other LineSeries.
        /// </summary>
        /// <param name="name">Name of the hovered LineSeries which is gonna be highlighted</param>
        public void highlightLineSeries(string name)
        {
            bool found = false;

            foreach (PlotLineSeriesPropertiesViewModel p in properties.lineSeriesProperties)
            {
                if (!p.lineSeries.Title.Equals(name))
                    properties.dimLineSeriesColor(p);
                else
                {
                    properties.removeLineSeriesColorOpacity(p);
                    found = true;
                }
            }

            if (!found)
                resetLineSeriesColors();
        }

        /// <summary>
        ///     Reset all LineSeries color to its original, removing the opacity in case that some
        ///     was applied before by the highlightLineSeries method.  
        /// </summary>
        public void resetLineSeriesColors()
        {
            foreach (PlotLineSeriesPropertiesViewModel p in properties.lineSeriesProperties)
                properties.removeLineSeriesColorOpacity(p);
        }


        private void updatePlotBounds(double x, double y)
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
            if (y < m_minY)
            {
                m_minY = y;
                bMustUpdate = true;
            }
            if (y > m_maxY)
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
                if (maxY - minY == 0.0) { minY -= 0.01; maxY += 0.01; }
                m_plot.Axes[0].Maximum = maxX;
                m_plot.Axes[0].Minimum = minX;
                m_plot.Axes[1].Maximum = maxY;
                m_plot.Axes[1].Minimum = minY;
            }
        }
        public void saveImage()
        {
            FolderBrowserDialog sfd = new FolderBrowserDialog();

            sfd.ShowNewFolderButton = true;
            //sfd.RootFolder = new Environment.SpecialFolder(Path.Combine(Directory.GetCurrentDirectory(), SimionFileData.experimentRelativeDir));
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                export(sfd.SelectedPath);
            }
        }

        public void showProperties()
        {
            CaliburnUtility.showVMDialog(properties, "Plot properties");
            setProperties();
        }

        public void setProperties()
        {
            plot.IsLegendVisible = properties.bLegendVisible;
            updateView();
        }
        public void export(string outputFolder)
        {
            string fileName;
            //as png
            fileName = outputFolder + "\\" + Utility.removeSpecialCharacters(name) + ".png";
            var pngExporter = new PngExporter { Width = 600, Height = 400, Background = OxyColors.Transparent };
            pngExporter.ExportToFile(m_plot, fileName);
            //as svg
            fileName = outputFolder + "\\" + Utility.removeSpecialCharacters(name) + ".svg";
            var svgExporter = new OxyPlot.Wpf.SvgExporter { Width = 600, Height = 400 };
            svgExporter.ExportToFile(m_plot, fileName);
        }

        private bool m_bShowOptions = false;
        public bool bShowOptions { get { return m_bShowOptions; } set { m_bShowOptions = value; NotifyOfPropertyChange(() => bShowOptions); } }


    }
}
