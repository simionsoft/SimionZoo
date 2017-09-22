using System;
using Caliburn.Micro;
using System.IO;
using Badger.Simion;
using System.Collections.Generic;
using Badger.Data;

namespace Badger.ViewModels
{
    public class ReportViewModel : Screen
    {
        private string m_name = "Unnamed";
        public string name { get { return m_name; } set { m_name = value; NotifyOfPropertyChange(() => name); } }

        //plots
        private PlotViewModel m_plot= null;
        public PlotViewModel Plot
        {
            get { return m_plot; }
            set { m_plot = value; m_plot.UpdateView(); NotifyOfPropertyChange(() => Plot); }
        }
        //private PlotViewModel m_selectedPlot = null;
        //public PlotViewModel selectedPlot
        //{
        //    get { return m_selectedPlot; }
        //    set
        //    {
        //        /*
        //         * workarround to solve the exception:
        //         * "This PlotModel is already in use by some other PlotView control."
        //         * switching the tabs multiple times
        //        */
        //        if (m_selectedPlot != null)
        //        {
        //            ((OxyPlot.IPlotModel)m_selectedPlot.Plot).AttachPlotView(null);
        //        }
        //        m_selectedPlot = value;
        //        m_selectedPlot.UpdateView();
        //        NotifyOfPropertyChange(() => selectedPlot);
        //    }
        //}

        public void updateView() { }

        //stats
        private StatsViewModel m_stats;
        public StatsViewModel Stats
        {
            get { return m_stats; }
            set { m_stats = value; NotifyOfPropertyChange(() => Stats); }
        }
        //private StatsViewModel m_selectedStat;
        //public StatsViewModel selectedStat
        //{
        //    get { return m_selectedStat; }
        //    set { m_selectedStat = value; NotifyOfPropertyChange(() => selectedStat); }
        //}

        public void export(string outputFolder)
        {
            //set culture as invariant to write numbers as in english
            System.Threading.Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            //export plots
            if (Plot!=null) Plot.Export(outputFolder);
            //export stats
            string statsFile = outputFolder + "\\" + name + ".xml";
            try
            {
                using (StreamWriter fileWriter = File.CreateText(statsFile))
                {
                    fileWriter.WriteLine("<" + XMLConfig.statisticsFileTag + ">");
                    Stats.export(fileWriter, "  ");
                    fileWriter.WriteLine("</" + XMLConfig.statisticsFileTag + ">");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error exporting stats file: " + statsFile);
                Console.Write(ex.ToString());
            }
        }

        private PlotType GetVariableReportType(LogQuery query, string variable)
        {
            //TODO: this only returns the first report type for that variable
            //This should be improved
            foreach (LoggedVariableViewModel var in query.loggedVariables)
            {
                if (var.name == variable)
                    return var.SelectedPlotType;
            }
            return PlotType.Undefined;
        }

        public ReportViewModel(LogQuery query, Report report)
        {
            name = report.Name;

            //Create the plot
            PlotViewModel newPlot = new PlotViewModel(report.Name, 0, "Time (s)", report.Name, false, true);
            //Create the stats
            StatsViewModel newStatGroup = new StatsViewModel(report.Name);

            foreach (TrackGroup group in query.ResultTracks)
            {
                //plot data
                SeriesGroup seriesGroup = group.ConsolidatedTrack.SeriesGroups[report];

                foreach (Series series in seriesGroup.SeriesList)
                {
                    string seriesName;
                    if (seriesGroup.SeriesList.Count == 1)
                        seriesName = group.GroupId;
                    else seriesName = group.GroupId + "-" + series.Id;

                    //add data to the plot
                    int lineSeriesId = newPlot.AddLineSeries(seriesName);
                    foreach (XYValue value in series.Values)
                        newPlot.AddLineSeriesValue(lineSeriesId, value.X, value.Y);

                    StatViewModel newStat = new StatViewModel(group.ExperimentId, seriesName, series.Stats);

                    newStatGroup.addStat(newStat);
                }
            }
            Plot = newPlot;
            Stats = newStatGroup;
        }
    }
}
