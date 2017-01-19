using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Collections.ObjectModel;
using Badger.Data;
using Caliburn.Micro;
using Simion;
using System.IO;

namespace Badger.ViewModels
{
    public class ReportsWindowViewModel : Conductor<ReportViewModel>.Collection.OneActive
    {

        private ObservableCollection<ReportViewModel> m_reports = new ObservableCollection<ReportViewModel>();
        public ObservableCollection<ReportViewModel> reports { get { return m_reports; } set { } }

        private bool m_bCanGenerateReports = false;
        public bool bCanGenerateReports { get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => bCanGenerateReports); } }

        //SOURCE OPTIONS
        public const string m_optionLastEvalEpisode = "Last evaluation episode";
        public const string m_optionAllEvalEpisodes = "All evaluation episodes";

        private ObservableCollection<string> m_sourceOptions = new ObservableCollection<string>();
        public ObservableCollection<string> sourceOptions { get { return m_sourceOptions; } set { } }

        private string m_selectedSource= "";
        public string selectedSource
        {
            get { return m_selectedSource; }
            set { m_selectedSource = value; updateCanGenerateReports();}
        }

        //the list of variables we can plot
        private ObservableCollection<LoggedVariableViewModel> m_availableVariables = new ObservableCollection<LoggedVariableViewModel>();
        public ObservableCollection<LoggedVariableViewModel> availableVariables { get { return m_availableVariables; }
            set {}
        }
        private string m_variableListHeader = "";
        public string variableListHeader
        {
            get { return m_variableListHeader; }
            set { m_variableListHeader = value; NotifyOfPropertyChange(() => variableListHeader); }
        }

        //the list of logs we have
        private ObservableCollection<ExperimentLogViewModel> m_experimentLogs = new ObservableCollection<ExperimentLogViewModel>();
        public ObservableCollection<ExperimentLogViewModel> experimentLogs
        {
            get { return m_experimentLogs; }
            set { m_experimentLogs = value; NotifyOfPropertyChange(() => experimentLogs); }
        }
        private string m_logListHeader = "";
        public string logListHeader
        {
            get { return m_logListHeader; }
            set { m_logListHeader = value; NotifyOfPropertyChange(() => logListHeader); }
        }
        private List<ExperimentLogViewModel> m_selectedLogs = new List<ExperimentLogViewModel>();
        private int m_numLogsSelected= 0;
        public void updateLogListHeader()
        {
            m_selectedLogs.Clear();
            foreach(ExperimentLogViewModel exp in m_experimentLogs)
            {
                if (exp.bIsSelected)
                    m_selectedLogs.Add(exp);
            }
            m_numLogsSelected = m_selectedLogs.Count();
            m_logListHeader = m_experimentLogs.Count + " logs (" + m_numLogsSelected + " selected)";
            NotifyOfPropertyChange(() => logListHeader);
            updateCanGenerateReports();
        }
        private List<LoggedVariableViewModel> m_selectedVariables = new List<LoggedVariableViewModel>();
        private int m_numVarsSelected = 0;
        public void updateVariableListHeader()
        {
            m_selectedVariables.Clear();
            foreach (LoggedVariableViewModel var in m_availableVariables)
            {
                if (var.bIsSelected)
                    m_selectedVariables.Add(var);
            }
            m_numVarsSelected = m_selectedVariables.Count();
            m_variableListHeader = m_experimentLogs.Count + " variables (" + m_numVarsSelected + " selected)";
            NotifyOfPropertyChange(() => variableListHeader);
            updateCanGenerateReports();
        }
        private void updateCanGenerateReports()
        {
            if (m_numVarsSelected > 0 && m_numLogsSelected > 0 && m_selectedSource!="")
                bCanGenerateReports = true;
        }

        public ReportsWindowViewModel()
        {
            m_sourceOptions.Add(m_optionAllEvalEpisodes);
            m_sourceOptions.Add(m_optionLastEvalEpisode);
            NotifyOfPropertyChange(() => sourceOptions);

            //we do not initialise the list of variables
            //when an experiment is selected, its variables will be displayed for the user to select
            NotifyOfPropertyChange(() => experimentLogs);
            updateVariableListHeader();
            updateLogListHeader();
        }
        public void updateAvailableVariableList()
        {
            //get selected experiments
            m_availableVariables.Clear();
            foreach (ExperimentLogViewModel exp in m_experimentLogs)
            {
                if (exp.bIsSelected)
                    exp.addVariablesToList(ref m_availableVariables);
            }
            NotifyOfPropertyChange(() => availableVariables);
        }
        public void generatePlots()
        {
            List<PlotViewModel> newPlots = new List<PlotViewModel>();
            //create a new plot for each variable
            foreach(LoggedVariableViewModel variable in m_selectedVariables)
            {
                PlotViewModel newPlot = new PlotViewModel(variable.name, false);
                newPlot.parent = this;
                newPlots.Add(newPlot);
            }

            //draw data from each log
            foreach (ExperimentLogViewModel log in m_selectedLogs)
            {
                log.plotData(newPlots, m_selectedSource);
            }
            //update plots
            foreach (PlotViewModel plot in newPlots)
            {
                reports.Add(plot);
                plot.updateView();
            }

            bCanSaveReports = true;
            selectedReport = reports[reports.Count - 1]; //select the last plot generated
        }
        public void generateStats()
        {
            StatsViewModel statsViewModel = new StatsViewModel("Stats");

            foreach (ExperimentLogViewModel log in m_selectedLogs)
            {
                List<Stat> stats = log.getVariableStats(m_selectedVariables);
                foreach (Stat stat in stats)
                {
                    statsViewModel.addStat(stat);
                }
            }

            bCanSaveReports = true;

            reports.Add(statsViewModel);
            selectedReport = statsViewModel;
            //ActivateItem(statsViewModel);
        }

        //plot selection in tab control
        private ReportViewModel m_selectedReport = null;
        public ReportViewModel selectedReport 
        {
            get { return m_selectedReport; }
            set
            {
                m_selectedReport = value;
                if (m_selectedReport!=null) m_selectedReport.updateView();
                NotifyOfPropertyChange(() => selectedReport);
            }
        }

        private bool m_bCanSaveReports = false;
        public bool bCanSaveReports { get { return m_bCanSaveReports; } set { m_bCanSaveReports = value; NotifyOfPropertyChange(() => bCanSaveReports); } }

        public void saveReports()
        {
            string outputFolder= CaliburnUtility.selectFolder(SimionFileData.imageRelativeDir);
            if (outputFolder!="")
            { 
                foreach(ReportViewModel plot in m_reports)
                {
                    plot.export(outputFolder);
                }
            }
        }

        private void batchNodeLoadFunction(XmlNode node)
        {
            string experimentName = node.Attributes[XMLConfig.nameAttribute].Value;
            string experimentFilePath = node.Attributes[XMLConfig.pathAttribute].Value;
            string logDescFile = ExperimentViewModel.getLogDescriptorsFilePath(experimentFilePath);
            string logFile = ExperimentViewModel.getLogFilePath(experimentFilePath);
            if (File.Exists(logDescFile) && File.Exists(logFile))
            {
                ExperimentLogViewModel newLog = new ExperimentLogViewModel(experimentName, logDescFile, logFile, this);
                experimentLogs.Add(newLog);
                newLog.bIsSelected = true;
            }
        }

        public void loadExperimentBatch()
        {
            SimionFileData.loadExperimentBatch(batchNodeLoadFunction);
        }
        public void loadExperimentBatch(string batchFileName)
        {
            SimionFileData.loadExperimentBatch(batchNodeLoadFunction, batchFileName);
        }

        public void close(ReportViewModel report)
        {
            reports.Remove(report);
        }

        //check all/none buttons
        public void checkAllLogs()
        {
            foreach (ExperimentLogViewModel experiment in m_experimentLogs)
                experiment.bIsSelected = true;
        }
        public void uncheckAllLogs()
        {
            foreach (ExperimentLogViewModel experiment in m_experimentLogs)
                experiment.bIsSelected = false;
        }
        public void checkAllVariables()
        {
            foreach (LoggedVariableViewModel variable in m_availableVariables)
                variable.bIsSelected = true;
        }
        public void uncheckAllVariables()
        {
            foreach (LoggedVariableViewModel variable in m_availableVariables)
                variable.bIsSelected = false;
        }
    }
}
