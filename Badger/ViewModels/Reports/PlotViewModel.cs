using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Wpf;
using Caliburn.Micro;
using System.Runtime.Serialization;
using Badger.Data;
using System;
using System.ComponentModel;

namespace Badger.ViewModels
{
    [DataContract]
    public class PlotViewModel : PropertyChangedBase, IDisposable
    {
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // dispose managed resources
                m_timer.Dispose();
            }
        }
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        string m_name = "";

        [DataMember]
        public string name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }
        private const int m_updateFreq = 1000; //plot refresh freq in millseconds
        private System.Threading.Timer m_timer;

        [DataMember]
        double m_minX = double.MaxValue;
        [DataMember]
        double m_maxX = double.MinValue;
        [DataMember]
        double m_minY = double.MaxValue;
        [DataMember]
        double m_maxY = double.MinValue;
        [DataMember]
        double m_maxNumSeries = 20;

        private PlotModel m_plot;
        [DataMember]
        public PlotModel Plot { get { return m_plot; } set { } }

        [DataMember]
        public PlotPropertiesViewModel Properties
        {
            get; set;
        } = new PlotPropertiesViewModel();

        public bool LineSeriesSelectionVisible
        {
            get { return Properties.LineSeriesProperties.Count > 1; }
        }

        public PlotViewModel(string title, double xMax = 0, string xName = "", string yName = "", bool bRefresh = true, bool bShowOptions = false)
        {
            name = title;
            m_plot = new PlotModel();
            var xAxis = new OxyPlot.Axes.LinearAxis();
            xAxis.Position = AxisPosition.Bottom;
            xAxis.MajorGridlineStyle = LineStyle.Solid;
            xAxis.Minimum = 0.0;
            xAxis.Maximum = 1.0;

            //If no absolute maximum x is provided, we don't set it and it will take the default value double.MaxValue
            if (xMax>0)
                xAxis.AbsoluteMaximum = xMax;
            xAxis.AbsoluteMinimum = 0.0;

            m_plot.Axes.Add(xAxis);

            var yAxis = new OxyPlot.Axes.LinearAxis();
            yAxis.Position = AxisPosition.Left;
            yAxis.MajorGridlineStyle = LineStyle.Solid;
            yAxis.Minimum = 0.0;
            yAxis.Maximum = 1.0;

            m_plot.Axes.Add(yAxis);
            //default properties
            m_plot.LegendBorder = OxyColors.Black;
            m_plot.LegendBackground = OxyColors.White;

            Properties.Title = Utility.OxyPlotMathNotation(title);
            Properties.XAxisName = Utility.OxyPlotMathNotation(xName);
            Properties.YAxisName = Utility.OxyPlotMathNotation(yName);

            UpdatePlotProperties();

            //Add a listener to "PropertiesChanged" event from Properties
            Properties.PropertyChanged += PropertiesChanged;

            if (bRefresh)
            {
                m_timer = new System.Threading.Timer(updatePlot);
                m_timer.Change(m_updateFreq, m_updateFreq);
            }
            m_bShowOptions = bShowOptions;
        }

        public void PropertiesChanged(object sender, PropertyChangedEventArgs e)
        {
            if (sender is PlotPropertiesViewModel properties)
            {
                UpdatePlotProperties();

                UpdateView();
            }
        }

        private void UpdatePlotProperties()
        {
            //Boundaries
            if (m_plot.Axes.Count == 2)
            {
                double minY= double.MaxValue, maxY= double.MinValue;
                foreach(OxyPlot.Series.LineSeries series in m_plot.Series)
                {
                    if (series.IsVisible && series.MinY < minY)
                        minY = series.MinY;
                    if (series.IsVisible && series.MaxY > maxY)
                        maxY = series.MaxY;
                }
                if (maxY - minY > 0)
                {
                    m_plot.Axes[1].Maximum = maxY;
                    m_plot.Axes[1].Minimum = minY;
                }
                else
                {
                    m_plot.Axes[1].Maximum = 1.0;
                    m_plot.Axes[1].Minimum = 0.0;
                }
            }
            //Texts
            if (m_plot.Axes.Count == 2)
            {
                m_plot.Axes[0].Title = Properties.XAxisName;
                m_plot.Axes[1].Title = Properties.YAxisName;
            }
            m_plot.Title = Properties.Title;
            //font
            Plot.DefaultFont = Properties.SelectedFont;

            //legend
            Plot.IsLegendVisible = Properties.LegendVisible;
            bool showLegend = Properties.LegendBorder;
            Plot.LegendBorderThickness = showLegend ? 1 : 0;

            Plot.LegendOrientation = (OxyPlot.LegendOrientation)Enum.Parse(typeof(OxyPlot.LegendOrientation)
                , Properties.SelectedLegendOrientation);
            Plot.LegendPlacement = (OxyPlot.LegendPlacement)Enum.Parse(typeof(OxyPlot.LegendPlacement)
                , Properties.SelectedLegendPlacement);
            Plot.LegendPosition = (OxyPlot.LegendPosition)Enum.Parse(typeof(OxyPlot.LegendPosition)
                , Properties.SelectedLegendPosition);

            //to update the boundaries
            UpdateView();
        }

        private void updatePlot(object state)
        {
            m_plot.InvalidatePlot(true);
        }

        public void UpdateView()
        {
            m_plot.InvalidatePlot(true);
        }

        private object m_lineSeriesLock = new object();

        public int AddLineSeries(string title, string description ="", bool isVisible = true)
        {
            //TODO: improve how to limit the number of plots
            //For now, we just ignore series after the max number has been reached
            if (m_plot.Series.Count<m_maxNumSeries)
                lock (m_lineSeriesLock)
                {
                    OxyPlot.Series.LineSeries newSeries = new OxyPlot.Series.LineSeries { Title = title, MarkerType = MarkerType.None };
                    newSeries.IsVisible = isVisible;
                    m_plot.Series.Add(newSeries);

                    Properties.AddLineSeries(title, description, newSeries);

                    return m_plot.Series.Count - 1; ;
                }
            return -1;
        }

        public void AddLineSeriesValue(int seriesIndex, double xValue, double yValue)
        {
            if (seriesIndex < 0 || seriesIndex >= m_plot.Series.Count)
            {
                //TODO: at least, we should log the error
                return;
            }

            OxyPlot.Series.LineSeries series = (OxyPlot.Series.LineSeries)m_plot.Series[seriesIndex];
            UpdatePlotBounds(xValue, yValue);
            series.Points.Add(new DataPoint(xValue, yValue));
        }

        public void ClearLineSeries()
        {
            m_plot.Series.Clear();
            UpdateView();
            NotifyOfPropertyChange("Series");
        }

        /// <summary>
        ///     Identify which LineSeries is hovered and make a call to the dimLineSeriesColor method
        ///     passing the correct LineSeriesProperties object as parameter.
        ///     In order to highlight a LineSeries what we actually do is to dim, that is, apply
        ///     certain opacity, to all the other LineSeries.
        /// </summary>
        /// <param name="seriesId">Id of the LineSeries to be highlighted</param>
        public void HighlightLineSeries(int seriesId)
        {
            if (seriesId < 0) ResetLineSeriesColors();

            Properties.HighlightSeries(seriesId);
        }

        /// <summary>
        ///     Reset all LineSeries color to its original, removing the opacity in case that some
        ///     was applied before by the highlightLineSeries method.
        /// </summary>
        public void ResetLineSeriesColors()
        {
            foreach (PlotLineSeriesPropertiesViewModel p in Properties.LineSeriesProperties)
                Properties.ResetLineSeriesOpacity(p);
        }


        private void UpdatePlotBounds(double x, double y)
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

        public void SaveImage()
        {
            string outputFolder = Simion.SimionFileData.SelectOutputDirectoryDialog();

            if (outputFolder!=null)
            {
                Export(outputFolder);
            }
        }

        public void ShowProperties()
        {
            CaliburnUtility.ShowPopupWindow(Properties, "Plot properties");
        }

        public void Export(string outputFolder)
        {
            string fileName;
            //as png
            fileName = outputFolder + "\\" + Utility.RemoveSpecialCharacters(name) + ".png";
            var pngExporter = new PngExporter { Width = 600, Height = 400, Background = OxyColors.Transparent };
            pngExporter.ExportToFile(m_plot, fileName);
            //as svg
            fileName = outputFolder + "\\" + Utility.RemoveSpecialCharacters(name) + ".svg";
            var svgExporter = new OxyPlot.Wpf.SvgExporter { Width = 600, Height = 400 };
            svgExporter.ExportToFile(m_plot, fileName);
        }

        private bool m_bShowOptions = false;
        [DataMember]
        public bool bShowOptions { get { return m_bShowOptions; } set { m_bShowOptions = value; NotifyOfPropertyChange(() => bShowOptions); } }


    }
}
