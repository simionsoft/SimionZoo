using System;
using Caliburn.Micro;
using System.IO;
using Badger.Simion;
using Badger.Data;


namespace Badger.ViewModels
{
    public class ReportViewModel : Screen
    {
        private string m_name = "Unnamed";
        public string Name { get { return m_name; } set { m_name = value; NotifyOfPropertyChange(() => Name); } }

        //plots
        private PlotViewModel m_plot= null;
        public PlotViewModel Plot
        {
            get { return m_plot; }
            set { m_plot = value; m_plot.UpdateView(); NotifyOfPropertyChange(() => Plot); }
        }
 
        //stats
        private StatsViewModel m_stats;
        public StatsViewModel Stats
        {
            get { return m_stats; }
            set { m_stats = value; NotifyOfPropertyChange(() => Stats); }
        }

        public void Export(string outputFolder)
        {
            //export plots
            if (Plot!=null) Plot.Export(outputFolder);
            //export stats
            string statsFile = outputFolder + "\\" + Name + ".xml";
            try
            {
                using (StreamWriter fileWriter = File.CreateText(statsFile))
                {
                    fileWriter.WriteLine("<" + XMLConfig.statisticsFileTag + ">");
                    Stats.Export(fileWriter, "  ");
                    fileWriter.WriteLine("</" + XMLConfig.statisticsFileTag + ">");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error exporting stats file: " + statsFile);
                Console.Write(ex.ToString());
            }
        }

        private ReportType GetVariableReportType(LogQueryViewModel query, string variable)
        {
            //TODO: this only returns the first report type for that variable
            //This should be improved
            foreach (LoggedVariableViewModel var in query.VariablesVM)
            {
                if (var.name == variable)
                    return var.SelectedPlotType;
            }
            return ReportType.Undefined;
        }

        public ReportViewModel(LogQueryViewModel query, Report report)
        {
            Name = report.Name;

            //Create the plot
            PlotViewModel newPlot = new PlotViewModel(report.Name, 0, "Time (s)", report.Name, false, true);
            //Create the stats
            StatsViewModel newStatGroup = new StatsViewModel(report.Name);

            foreach (TrackGroup group in query.ResultTracks)
            {
                //plot data
                if (group.ConsolidatedTrack != null)
                {
                    SeriesGroup seriesGroup = group.ConsolidatedTrack.SeriesGroups[report];

                    foreach (Series series in seriesGroup.SeriesList)
                    {
                        string seriesName;
                        if (seriesGroup.SeriesList.Count == 1)
                        {
                            //only one series per track group, no multi-series track group
                            seriesName = group.ConsolidatedTrack.TrackId;
                        }
                        else seriesName = group.ConsolidatedTrack.TrackId + "-" + series.Id;

                        //add data to the plot
                        int lineSeriesId = newPlot.AddLineSeries(seriesName);
                        foreach (XYValue value in series.Values)
                            newPlot.AddLineSeriesValue(lineSeriesId, value.X, value.Y);

                        StatViewModel newStat =
                            new StatViewModel(group.ExperimentId, seriesName, series.Stats
                                , group.ConsolidatedTrack.LogBinaryFile
                                , group.ConsolidatedTrack.LogDescriptorFile
                                , group.ConsolidatedTrack.ExperimentalUnitConfigFile);

                        newStatGroup.addStat(newStat);
                    }
                }
            }
            Plot = newPlot;
            Stats = newStatGroup;
        }

    }
}
