using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Collections.ObjectModel;
using Badger.Data;
using System.Windows.Forms;
using Simion;
using System.IO;

namespace Badger.ViewModels
{
    public class PlotEditorWindowViewModel : Caliburn.Micro.Screen
    {

        private ObservableCollection<PlotViewModel> m_plots = new ObservableCollection<PlotViewModel>();
        public ObservableCollection<PlotViewModel> plots { get { return m_plots; } set { } }

        private bool m_bCanGeneratePlots = false;
        public bool bCanGeneratePlots { get { return m_bCanGeneratePlots; }
            set { m_bCanGeneratePlots = value; NotifyOfPropertyChange(() => bCanGeneratePlots); } }

        //SOURCE OPTIONS
        public const string m_optionLastEvalEpisode = "Last evaluation episode";
        public const string m_optionAllEvalEpisodes = "All evaluation episodes";

        private ObservableCollection<string> m_sourceOptions = new ObservableCollection<string>();
        public ObservableCollection<string> sourceOptions { get { return m_sourceOptions; } set { } }

        private string m_selectedSource= "";
        public string selectedSource
        {
            get { return m_selectedSource; }
            set { m_selectedSource = value; updateCanGeneratePlots();}
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
            updateCanGeneratePlots();
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
            updateCanGeneratePlots();
        }
        private void updateCanGeneratePlots()
        {
            if (m_numVarsSelected > 0 && m_numLogsSelected > 0 && m_selectedSource!="")
                bCanGeneratePlots = true;
        }

        public PlotEditorWindowViewModel()
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
                newPlots.Add(new PlotViewModel(variable.name, false));
            }

            //draw data from each log
            foreach (ExperimentLogViewModel log in m_selectedLogs)
            {
                log.plotData(newPlots, m_selectedSource);
            }
            //update plots
            foreach (PlotViewModel plot in newPlots)
            {
                plots.Add(plot);
                plot.update();
            }
            //plot tabs can't be closed yet, so we can simplify it for now
            bCanSavePlots = true;
        }

        //plot selection in tab control
        private PlotViewModel m_selectedPlot = null;
        public PlotViewModel selectedPlot 
        {
            get { return m_selectedPlot; }
            set
            {
                m_selectedPlot = value;
                m_selectedPlot.update();
            }
        }

        private bool m_bCanSavePlots = false;
        public bool bCanSavePlots { get { return m_bCanSavePlots; } set { m_bCanSavePlots = value; NotifyOfPropertyChange(() => bCanSavePlots); } }

        public void savePlots()
        {
            string outputFolder= CaliburnUtility.selectFolder("../images");
            if (outputFolder!="")
            {
                foreach(PlotViewModel plot in m_plots)
                {
                    plot.export(outputFolder);
                }
            }
        }

        public void loadExperimentBatch()
        {
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Experiment batch | *." + XMLConfig.experimentBatchExtension;
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else return;

            //LOAD THE EXPERIMENT BATCH IN THE QUEUE
            XmlDocument batchDoc = new XmlDocument();
            batchDoc.Load(fileDoc);
            XmlElement fileRoot = batchDoc.DocumentElement;
            string experimentName;
            string experimentFilePath;
            string logDescFile;
            string logFile;
            if (fileRoot.Name == XMLConfig.batchNodeTag)
            {
                foreach (XmlNode experiment in fileRoot.ChildNodes)
                {
                    if (experiment.Name == XMLConfig.experimentNodeTag)
                    {
                        experimentName = experiment.Attributes[XMLConfig.nameAttribute].Value;
                        experimentFilePath = experiment.Attributes[XMLConfig.pathAttribute].Value;
                        logDescFile = ExperimentViewModel.getLogDescriptorsFilePath(experimentFilePath);
                        logFile = ExperimentViewModel.getLogFilePath(experimentFilePath);
                        if (File.Exists(logDescFile) && File.Exists(logFile))
                        {
                            experimentLogs.Add(new ExperimentLogViewModel(experimentName, logDescFile, logFile, this));
                        }
                    }
                }
            }
            CaliburnUtility.showWarningDialog("Malformed XML in experiment queue file. No badger node.","ERROR");
            return;
        }
    }
}
