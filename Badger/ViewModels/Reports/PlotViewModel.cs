using System.Runtime.Serialization;
using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.IO;
using System.Xml;

using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Wpf;

using Caliburn.Micro;

using Badger.Data;

using Herd.Files;

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

        public SafePlotModel Plot { get; set; } = new SafePlotModel();

        [DataMember]
        public PlotPropertiesViewModel Properties
        {
            get; set;
        } = new PlotPropertiesViewModel();

        [DataMember]
        public bool LineSeriesSelectionVisible
        {
            get { return Properties.LineSeriesProperties.Count > 1; }
            set { }
        }

        public PlotViewModel(string title, string xAxisName = "", string yAxisName = "", bool bRefresh = true, bool bShowOptions = false)
        {
            name = title;
            m_bShowOptions = bShowOptions;
            if (bRefresh)
            {
                m_timer = new System.Threading.Timer(UpdatePlot);
                m_timer.Change(m_updateFreq, m_updateFreq);
            }

            InitPlot(title, xAxisName, yAxisName);
        }

        void ResetAxes()
        {
            foreach (OxyPlot.Axes.Axis axis in Plot.Axes)
            {
                axis.Minimum = 0.0;
                axis.AbsoluteMinimum = 0.0;
                axis.Maximum = 1.0;
                axis.AbsoluteMaximum = 1.0;
            }
        }

        void InitPlot(string title, string xAxisName, string yAxisName)
        {
            var xAxis = new OxyPlot.Axes.LinearAxis();
            xAxis.Position = AxisPosition.Bottom;
            xAxis.MajorGridlineStyle = LineStyle.Solid;
            Plot.Axes.Add(xAxis);

            var yAxis = new OxyPlot.Axes.LinearAxis();
            yAxis.Position = AxisPosition.Left;
            yAxis.MajorGridlineStyle = LineStyle.Solid;
            Plot.Axes.Add(yAxis);

            ResetAxes();
            //default properties
            Plot.LegendBorder = OxyColors.Black;
            Plot.LegendBackground = OxyColors.White;

            Properties.Title = Herd.Utils.OxyPlotMathNotation(title);
            Properties.XAxisName = Herd.Utils.OxyPlotMathNotation(xAxisName);
            Properties.YAxisName = Herd.Utils.OxyPlotMathNotation(yAxisName);

            UpdatePlotProperties();

            //Add a listener to "PropertiesChanged" event from Properties
            Properties.PropertyChanged += PropertiesChanged;
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
            while (Plot.Axes.Count<2)
            {
                Plot.Axes.Add(new OxyPlot.Axes.LinearAxis());
            }

            UpdatePlotBoundsFromScratch();

            //Texts
            Plot.Axes[0].Title = Properties.XAxisName;
            Plot.Axes[1].Title = Properties.YAxisName;

            Plot.Title = Properties.Title;
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

        private void UpdatePlot(object state)
        {
            if (Plot!=null)
                Plot.InvalidatePlot(true);
        }

        public void UpdateView()
        {
            if (Plot!=null)
                Plot.InvalidatePlot(true);
        }

        private object m_lineSeriesLock = new object();

        public int AddLineSeries(string title, string description ="", bool isVisible = true)
        {
            //TODO: improve how to limit the number of plots
            //For now, we just ignore series after the max number has been reached
            if (Plot.Series.Count < m_maxNumSeries)
            {
                if (m_lineSeriesLock == null) //might be null if loaded from file
                    m_lineSeriesLock = new object();
                lock (m_lineSeriesLock)
                {
                    OxyPlot.Series.LineSeries newSeries = new OxyPlot.Series.LineSeries { Title = title, MarkerType = MarkerType.None };
                    newSeries.IsVisible = isVisible;
                    Plot.Series.Add(newSeries);

                    PlotLineSeriesPropertiesViewModel lineProps = Properties.LineSeriesByName(title);

                    if (lineProps == null)
                        Properties.AddLineSeries(title, description, newSeries);
                    else
                        lineProps.LineSeries = newSeries;

                    return Plot.Series.Count - 1; ;
                }
            }
            return -1;
        }

        public void AddLineSeriesValue(int seriesIndex, double xValue, double yValue)
        {
            if (seriesIndex < 0 || seriesIndex >= Plot.Series.Count)
            {
                //TODO: at least, we should log the error
                return;
            }

            OxyPlot.Series.LineSeries series = (OxyPlot.Series.LineSeries)Plot.Series[seriesIndex];
            UpdatePlotBounds(xValue, yValue);
            series.Points.Add(new DataPoint(xValue, yValue));
        }

        public void ClearLineSeries()
        {
            Plot.Series.Clear();
            ResetAxes();
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

        private void UpdatePlotBoundsFromScratch()
        {
            double minY = double.MaxValue, maxY = double.MinValue;
            double minX = double.MaxValue, maxX = double.MinValue;
            bool atLeastOneLineSeries = false;
            foreach (OxyPlot.Series.LineSeries series in Plot.Series)
            {
                if (series.IsVisible && series.Points.Count>0)
                {
                    atLeastOneLineSeries = true;
                    foreach (DataPoint dataPoint in series.Points)
                    {
                        if (dataPoint.Y < minY)
                            minY = dataPoint.Y;
                        if (dataPoint.Y > maxY)
                            maxY = dataPoint.Y;
                        if (dataPoint.X < minX)
                            minX = dataPoint.X;
                        if (dataPoint.X > maxX)
                            maxX = dataPoint.X;
                    }
                }
            }
            if (!atLeastOneLineSeries)
            {
                minX = 0.0;
                maxX = 1.0;
                minY = 0.0;
                minY = 1.0;
            }
            if (maxY - minY > 0)
            {
                Plot.Axes[1].Maximum = maxY;
                Plot.Axes[1].Minimum = minY;
                Plot.Axes[1].AbsoluteMaximum = maxY;
                Plot.Axes[1].AbsoluteMinimum = minY;
            }
            else
            {
                Plot.Axes[1].Maximum = minY + 0.001;
                Plot.Axes[1].Minimum = minY;
                Plot.Axes[1].AbsoluteMaximum = minY + 0.001;
                Plot.Axes[1].AbsoluteMinimum = minY;
            }
            if (maxX - minX > 0)
            {
                Plot.Axes[0].Maximum = maxX;
                Plot.Axes[0].Minimum = minX;
                Plot.Axes[0].AbsoluteMaximum = maxX;
                Plot.Axes[0].AbsoluteMinimum = minX;
            }
            else
            {
                Plot.Axes[0].Maximum = minX + 0.001;
                Plot.Axes[0].Minimum = minX;
                Plot.Axes[0].AbsoluteMaximum = minX + 0.001;
                Plot.Axes[0].AbsoluteMinimum = minX;
            }
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
                UpdatePlotBoundsFromScratch();

                //to update the boundaries in the view
                UpdateView();
            }
        }

        public void SaveImage()
        {
            string outputFolder = Files.SelectOutputDirectoryDialog();
            Dictionary<string, string> outputFiles = new Dictionary<string, string>();
            if (outputFolder!=null)
            {
                Export(outputFolder, ref outputFiles); //we do nothing with the list of output files
            }
        }

        public void ShowProperties()
        {
            CaliburnUtility.ShowPopupWindow(Properties, "Plot properties");
        }

        public void Export(string outputFolder, ref Dictionary<string, string> outputFiles)
        {
            string baseFilename = outputFolder + "/" + Herd.Utils.RemoveSpecialCharacters(name);

            //1st save in common formats: png and svg
            string fileName;
            //as png
            fileName = baseFilename + ".png";
            var pngExporter = new PngExporter { Width = 600, Height = 400, Background = OxyColors.Transparent };
            pngExporter.ExportToFile(Plot, fileName);
            //as svg
            fileName = baseFilename + ".svg";
            var svgExporter = new OxyPlot.Wpf.SvgExporter { Width = 600, Height = 400 };
            svgExporter.ExportToFile(Plot, fileName);

            //2nd save data from the model for importing
            fileName = baseFilename + Herd.Files.Extensions.PlotDataExtension;

            using (TextWriter writer = File.CreateText(fileName))
            {
                writer.WriteLine("<" + XMLTags.PlotNodeTag + " " 
                    + XMLTags.nameAttribute + "=\"" + Herd.Utils.RemoveSpecialCharacters(name) + "\">");
                foreach(OxyPlot.Series.LineSeries lineSeries in Plot.Series)
                {
                    writer.WriteLine("  <" + XMLTags.LineSeriesTag + " " + XMLTags.nameAttribute + "=\"" + lineSeries.Title + "\">");

                    foreach(DataPoint dataPoint in lineSeries.Points)
                    {
                        writer.WriteLine("    <" + XMLTags.DataPointTag + ">");
                        writer.WriteLine("      <" + XMLTags.DataPointXTag + ">" + dataPoint.X + "</" + XMLTags.DataPointXTag + ">");
                        writer.WriteLine("      <" + XMLTags.DataPointYTag + ">" + dataPoint.Y + "</" + XMLTags.DataPointYTag + ">");
                        writer.WriteLine("    </" + XMLTags.DataPointTag + ">");
                    }

                    writer.WriteLine("  </" + XMLTags.LineSeriesTag + ">");
                }
                writer.WriteLine("</" + XMLTags.PlotNodeTag + ">");

                string relPlotFilename = Herd.Utils.RemoveSpecialCharacters(name) + Herd.Files.Extensions.PlotDataExtension;
                outputFiles[relPlotFilename] = XMLTags.PlotNodeTag;
            }
        }

        public void Import(string inputFolder)
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(inputFolder + Herd.Utils.RemoveSpecialCharacters(name) + Herd.Files.Extensions.PlotDataExtension);

            XmlNode root = xmlDoc.FirstChild;
            string plotName = root.Attributes[XMLTags.nameAttribute].Value;

            if (plotName == Herd.Utils.RemoveSpecialCharacters(name))
            {
                Plot = new SafePlotModel();

                InitPlot(plotName, Properties.XAxisName, Properties.YAxisName);

                foreach (XmlNode lineSeries in root.ChildNodes)
                {
                    if (lineSeries.Name == XMLTags.LineSeriesTag)
                    {
                        string lineSeriesTitle = lineSeries.Attributes[XMLTags.nameAttribute].Value;

                        int lineSeriesId = AddLineSeries(lineSeriesTitle);
                        foreach (XmlNode dataPoint in lineSeries.ChildNodes)
                        {
                            if (dataPoint.Name == XMLTags.DataPointTag)
                            {
                                double x = 0.0, y = 0.0;
                                foreach (XmlNode coord in dataPoint.ChildNodes)
                                {
                                    if (coord.Name == XMLTags.DataPointXTag) x = Convert.ToDouble(coord.InnerText);
                                    if (coord.Name == XMLTags.DataPointYTag) y = Convert.ToDouble(coord.InnerText);
                                }
                                AddLineSeriesValue(lineSeriesId, x, y);
                            }
                        }
                    }
                }

                //Set Properties as Notifying. Deserializing doesn't do it
                Properties.SetNotifying(true);
                //Add a listener to "PropertiesChanged" event from Properties
                Properties.PropertyChanged += PropertiesChanged;

                UpdatePlotProperties();
            }
        }

        private bool m_bShowOptions = false;
        [DataMember]
        public bool bShowOptions { get { return m_bShowOptions; } set { m_bShowOptions = value; NotifyOfPropertyChange(() => bShowOptions); } }


    }
}
