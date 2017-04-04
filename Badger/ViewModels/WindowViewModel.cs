using Badger.Data;
using System.Collections.ObjectModel;
using System.Xml;
using Caliburn.Micro;
using System.IO;
using System.Collections.Generic;
using System;
using System.Text;


namespace Badger.ViewModels
{

    public class WindowViewModel : PropertyChangedBase
    {
        static private BindableCollection<ExperimentViewModel> m_experiments = new BindableCollection<ExperimentViewModel>();
        //these two properties interface to the same hidden attribute
        public BindableCollection<ExperimentViewModel> experiments
        {
            get { return m_experiments; }
            set { m_experiments = value; NotifyOfPropertyChange(() => experiments); }
        }

        private ExperimentViewModel m_selectedExperiment;
        public ExperimentViewModel selectedExperiment
        {
            get { return m_selectedExperiment; }
            set
            {
                m_selectedExperiment = value;
                NotifyOfPropertyChange(() => selectedExperiment);
            }
        }


        static private bool appNameExists(string name)
        {
            foreach (ExperimentViewModel app in m_experiments)
            {
                if (app.name == name)
                    return true;
            }
            return false;
        }
        static public string getValidAppName(string originalName)
        {
            int id = 1;
            string newName = originalName;
            bool bNameExists = appNameExists(newName);
            while (bNameExists)
            {
                newName = originalName + "-" + id;
                id++;
                bNameExists = appNameExists(newName);
            }
            return newName;
        }

        private ExperimentMonitorViewModel m_monitorWindowViewModel;

        public ExperimentMonitorViewModel monitorWindowViewModel { get { return m_monitorWindowViewModel; } }

        // This is need it in order to enable/disable the StopExperiments button
        private bool m_bExperimentRunning = false;

        public bool IsExperimentRunning
        {
            get { return m_bExperimentRunning; }
            set
            {
                m_bExperimentRunning = value;
                NotifyOfPropertyChange(() => IsExperimentRunning);
            }
        }

        private ShepherdViewModel m_shepherdViewModel;
        public ShepherdViewModel shepherdViewModel { get { return m_shepherdViewModel; } set { } }

        private bool m_bIsExperimentListNotEmpty = false;
        public bool bIsExperimentListNotEmpty
        {
            get { return m_bIsExperimentListNotEmpty; }
            set
            {
                m_bIsExperimentListNotEmpty = value;
                NotifyOfPropertyChange(() => bIsExperimentListNotEmpty);
            }
        }
        private void checkEmptyExperimentList()
        {
            bool wasEmpty = !m_bIsExperimentListNotEmpty;
            if (wasEmpty != (m_experiments.Count == 0))
            {
                m_bIsExperimentListNotEmpty = !(m_experiments.Count == 0);
                NotifyOfPropertyChange(() => bIsExperimentListNotEmpty);
            }
        }

        private ObservableCollection<string> m_appNames = new ObservableCollection<string>();
        public ObservableCollection<string> appNames { get { return m_appNames; } set { } }

        //key element is the apps name, and the value is the .xml definition file
        private Dictionary<string, string> appDefinitions = new Dictionary<string, string>();

        private string m_selectedAppName;

        public string selectedAppName
        {
            get { return m_selectedAppName; }
            set
            {
                int index = m_appNames.IndexOf(value);
                if (index == -1)
                    return;
                m_selectedAppName = value;
                NotifyOfPropertyChange(() => selectedAppName);
            }
        }
        public void newExperiment()
        {
            if (m_selectedAppName == null) return;

            string xmlDefinitionFile = appDefinitions[m_selectedAppName];
            ExperimentViewModel newExperiment = new ExperimentViewModel(this, xmlDefinitionFile, null, "New");
            experiments.Add(newExperiment);
            NotifyOfPropertyChange(() => experiments);
            checkEmptyExperimentList();
            selectedExperiment = newExperiment;
        }

        private object m_logFileLock = new object();
        public const string logFilename = SimionFileData.badgerLogFile;
        private bool m_bFirstLog = true;

        public void logToFile(string logMessage)
        {
            lock (m_logFileLock)
            {
                string text = DateTime.Now.ToShortDateString() + " " +
                                DateTime.Now.ToShortTimeString() + ": " + logMessage + "\n";
                FileStream file;
                if (m_bFirstLog)
                {
                    file = File.Create(logFilename);
                    m_bFirstLog = false;
                }
                else file = File.Open(logFilename, FileMode.Append);
                if (file != null)
                {
                    file.Write(Encoding.ASCII.GetBytes(text), 0, Encoding.ASCII.GetByteCount(text));
                    file.Close();
                }
                Console.WriteLine(text);
            }
        }

        public WindowViewModel()
        {
            m_shepherdViewModel = new ShepherdViewModel();
            m_monitorWindowViewModel = new ExperimentMonitorViewModel(null, null, null, null);

            loadAppDefinitions();
        }

        private void loadAppDefinitions()
        {
            foreach (string app in Directory.GetFiles(SimionFileData.appConfigRelativeDir))
            {
                char[] spliter = "\\".ToCharArray();
                string[] tmp = app.Split(spliter);
                tmp = tmp[tmp.Length - 1].Split('.');
                string name = tmp[0];
                appDefinitions.Add(name, app);
                m_appNames.Add(name);
            }

            selectedAppName = m_appNames[0];
            NotifyOfPropertyChange(() => appNames);
        }

        public void saveExperimentInEditor()
        {
            if (selectedExperiment == null || !selectedExperiment.validate())
            {
                CaliburnUtility.showWarningDialog("The app can't be validated. See error log.", "Error");
                return;
            }

            SimionFileData.saveExperiment(selectedExperiment);
        }

        public void saveExperiments()
        {
            SimionFileData.saveExperiments(m_experiments);
        }


        public void loadExperiment()
        {
            ExperimentViewModel newApp = SimionFileData.loadExperiment(this, appDefinitions);
            if (newApp != null)
            {
                experiments.Add(newApp);
                checkEmptyExperimentList();
                selectedExperiment = newApp;
            }
        }


        public void clearExperimentQueue()
        {
            selectedExperiment = null;
            if (experiments != null) experiments.Clear();
        }
        public void close(ExperimentViewModel app)
        {
            experiments.Remove(app);
            checkEmptyExperimentList();
        }
        public void removeSelectedExperiments()
        {
            if (selectedExperiment != null)
            {
                experiments.Remove(selectedExperiment);
                checkEmptyExperimentList();
            }
        }

        //BADGER files
        public void loadExperiments()
        {
            SimionFileData.loadExperiments(this, ref m_experiments, appDefinitions, logToFile);
            NotifyOfPropertyChange(() => experiments);
            if (m_experiments.Count > 0)
                selectedExperiment = m_experiments[0];
            checkEmptyExperimentList();
        }

        public void runExperiments()
        {
            if (m_shepherdViewModel.herdAgentList.Count == 0)
            {
                CaliburnUtility.showWarningDialog("No Herd agents were detected, so experiments cannot be sent. Consider starting the local agent: \"net start HerdAgent\"", "No agents detected");
                return;
            }
            string batchFilename = "";
            List<Experiment> experiments = new List<Experiment>();
            experiments = SimionFileData.saveExperimentBatchFile(this.experiments, ref batchFilename, this.logToFile);

            if (experiments != null && experiments.Count > 0)
            {
                List<HerdAgentViewModel> freeHerdAgents = new List<HerdAgentViewModel>();

                logToFile("Running experiment queue remotely: " + experiments.Count + " experiments");

                //get available herd agents list. Inside the loop to update the list
                shepherdViewModel.getAvailableHerdAgents(ref freeHerdAgents);
                logToFile("Using " + freeHerdAgents.Count + " agents");

                m_monitorWindowViewModel.FreeHerdAgents = freeHerdAgents;
                m_monitorWindowViewModel.PendingExperiments = experiments;
                m_monitorWindowViewModel.LogFunction = logToFile;
                m_monitorWindowViewModel.BatchFileName = batchFilename;

                m_monitorWindowViewModel.runExperiments(true, true);
                IsExperimentRunning = true;
            }
        }

        /// <summary>
        ///     Stops all experiments in progress.
        /// </summary>
        public void stopExperiments()
        {
            if (IsExperimentRunning)
            {
                m_monitorWindowViewModel.stopExperiments();
                IsExperimentRunning = false;
            }
        }

        public void showPlotWindow()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            CaliburnUtility.showVMDialog(plotEditor, "Plot editor");
        }
    }
}
