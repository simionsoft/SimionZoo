using System;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Simion;
using Badger.Data;
using System.Runtime.Serialization;

namespace Badger.ViewModels
{
    [DataContract]
    public class LogQueryResultViewModel : PropertyChangedBase
    {
        static int numQueryResults = 0;

        string getDefaultQueryResultName()
        {
            numQueryResults++;
            return "Query-" + numQueryResults;
        }

        [DataMember]
        public LogQueryViewModel Query { get; set; }

        string m_name = "Unnamed";
        [DataMember]
        public string Name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => Name); }
        }

        //report list
        [DataMember]
        public BindableCollection<ReportViewModel> Reports
        { get; set; } = new BindableCollection<ReportViewModel>();

        //report selection in tab control
        private ReportViewModel m_selectedReport;
        public ReportViewModel SelectedReport
        {
            get { return m_selectedReport; }
            set
            {
                m_selectedReport = value;
                NotifyOfPropertyChange(() => SelectedReport);
            }
        }

        public LogQueryResultViewModel(List<TrackGroup> queryResultTracks, List<Report> reports, LogQueryViewModel query)
        {
            Name = getDefaultQueryResultName();

            Query = query;

            // Display the reports
            foreach (Report report in reports)
            {
                ReportViewModel newReport = new ReportViewModel(queryResultTracks, query, report);
                Reports.Add(newReport);
            }

            //Set the last as selected
            if (Reports.Count > 0)
                SelectedReport = Reports[Reports.Count-1];
        }

        public Dictionary<string,string> ExportNonSerializable(string outputBaseFolder)
        {
            Dictionary<string, string> outputFiles = new Dictionary<string, string>();
            foreach (ReportViewModel report in Reports)
            {
                report.ExportNonSerializable(outputBaseFolder, ref outputFiles);
            }
            return outputFiles;
        }

        public void ImportNonSerializable(string inputBaseFolder)
        {
            foreach (ReportViewModel report in Reports)
            {
                report.ImportNonSerializable(inputBaseFolder);
            }
            if (Reports.Count > 0) SelectedReport = Reports[0];
        }


        public void SetNotifying(bool notifying)
        {
            IsNotifying = notifying;
            foreach (ReportViewModel report in Reports)
                report.IsNotifying = notifying;
        }
    }
}
