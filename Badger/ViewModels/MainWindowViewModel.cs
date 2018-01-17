using Badger.Data;
using System.Collections.ObjectModel;
using System.Xml;
using Caliburn.Micro;
using System.IO;
using System.Collections.Generic;
using System;
using System.Linq;
using System.Text;
using Badger.Simion;

namespace Badger.ViewModels
{
    /// <summary>
    /// 
    /// </summary>
    public class MainWindowViewModel : PropertyChangedBase
    {
        private const string BatchFile = "Batch File";

        static private BindableCollection<ExperimentViewModel> m_experimentViewModels
            = new BindableCollection<ExperimentViewModel>();
        //these two properties interface to the same hidden attribute
        public BindableCollection<ExperimentViewModel> ExperimentViewModels
        {
            get { return m_experimentViewModels; }
            set { m_experimentViewModels = value; NotifyOfPropertyChange(() => ExperimentViewModels); }
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
            return m_experimentViewModels.Any(app => app.name == name);
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

        private ExperimentMonitorWindowViewModel m_monitorWindowViewModel;

        public ExperimentMonitorWindowViewModel monitorWindowViewModel { get { return m_monitorWindowViewModel; } }

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

        public ShepherdViewModel ShepherdViewModel { get { return m_shepherdViewModel; } set { } }

        private bool m_bCanLaunchExperiment;

        public bool CanLaunchExperiment
        {
            get { return m_bCanLaunchExperiment; }
            set
            {
                m_bCanLaunchExperiment = value;
                NotifyOfPropertyChange(() => CanLaunchExperiment);
            }
        }

        private bool m_bIsExperimentListNotEmpty;

        public bool IsExperimentListNotEmpty
        {
            get { return m_bIsExperimentListNotEmpty; }
            set
            {
                m_bIsExperimentListNotEmpty = value;
                NotifyOfPropertyChange(() => IsExperimentListNotEmpty);
            }
        }

        private void CheckEmptyExperimentList()
        {
            bool wasEmpty = !m_bIsExperimentListNotEmpty;
            if (wasEmpty != (m_experimentViewModels.Count == 0))
            {
                m_bIsExperimentListNotEmpty = m_experimentViewModels.Count != 0;
                NotifyOfPropertyChange(() => IsExperimentListNotEmpty);
            }
        }

        private ObservableCollection<string> m_appNames = new ObservableCollection<string>();

        public ObservableCollection<string> AppNames { get { return m_appNames; } set { m_appNames = value; } }

        public ObservableCollection<string> LaunchMode { get; set; }

        // Key element is the apps name, and the value is the .xml definition file
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

        private string m_selectedLaunchMode;

        public string SelectedLaunchMode
        {
            get { return m_selectedLaunchMode; }
            set
            {
                int index = LaunchMode.IndexOf(value);
                if (index == -1)
                    return;

                m_selectedLaunchMode = value;

                if (m_selectedLaunchMode.Equals(BatchFile))
                    CanLaunchExperiment = true;
                else if (m_bIsExperimentListNotEmpty)
                    CanLaunchExperiment = true;
                else
                    CanLaunchExperiment = false;

                NotifyOfPropertyChange(() => LaunchMode);
            }
        }


        public void NewExperiment()
        {
            if (m_selectedAppName == null) return;

            string xmlDefinitionFile = appDefinitions[m_selectedAppName];
            ExperimentViewModel newExperiment = new ExperimentViewModel(xmlDefinitionFile, null, "New");
            ExperimentViewModels.Add(newExperiment);
            NotifyOfPropertyChange(() => ExperimentViewModels);
            CheckEmptyExperimentList();

            CanLaunchExperiment = m_bIsExperimentListNotEmpty;

            selectedExperiment = newExperiment;
        }

        private object m_logFileLock = new object();

        public const string logFilename = SimionFileData.badgerLogFile;

        private bool m_bFirstLog = true;

        public void logToFile(string logMessage)
        {
            lock (m_logFileLock)
            {
                string text = DateTime.Now.ToShortDateString() + " "
                    + DateTime.Now.ToShortTimeString() + ": " + logMessage + "\n";
                FileStream file;

                if (m_bFirstLog)
                {
                    file = File.Create(logFilename);
                    m_bFirstLog = false;
                }
                else
                    file = File.Open(logFilename, FileMode.Append);

                file.Write(Encoding.ASCII.GetBytes(text), 0, Encoding.ASCII.GetByteCount(text));
                file.Close();

                Console.WriteLine(text);
            }
        }

        /// <summary>
        ///     Class constructor.
        /// </summary>
        public MainWindowViewModel()
        {
            m_shepherdViewModel = new ShepherdViewModel();
            LaunchMode = new ObservableCollection<string> { BatchFile, "Loaded Experiment" };
            SelectedLaunchMode = LaunchMode[0];
            LoadAppDefinitions();

            // Check for aditional required configuration files
            if (!File.Exists("..\\config\\definitions.xml"))
                CaliburnUtility.ShowWarningDialog("Unable to find required configuration file.", "Fatal Error");
        }


        private void LoadAppDefinitions()
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
            NotifyOfPropertyChange(() => AppNames);
        }

        public void SaveSelectedExperiment()
        {
            if (selectedExperiment == null || !selectedExperiment.validate())
            {
                CaliburnUtility.ShowWarningDialog("The app can't be validated. See error log.", "Error");
                return;
            }

            SimionFileData.SaveExperiment(selectedExperiment);
        }

        public void SaveAllExperiments()
        {
            SimionFileData.SaveExperiments(m_experimentViewModels);
        }

        /// <summary>
        ///     Load a single experiment and adds it to the experiment list.
        ///     Used from MainWindowView when the Load experiment button is clicked.
        /// </summary>
        public void LoadExperiment()
        {
            ExperimentViewModel newExperiment = SimionFileData.LoadExperiment(appDefinitions);

            if (newExperiment != null)
            {
                ExperimentViewModels.Add(newExperiment);
                CheckEmptyExperimentList();

                CanLaunchExperiment = m_bIsExperimentListNotEmpty;

                selectedExperiment = newExperiment;
            }
        }


        public void clearExperimentQueue()
        {
            selectedExperiment = null;
            if (ExperimentViewModels != null) ExperimentViewModels.Clear();
        }

        /// <summary>
        ///     Close a tab (experiment view) and remove it from experiments list.
        /// </summary>
        /// <param name="e">The experiment to be removed</param>
        public void Close(ExperimentViewModel e)
        {
            ExperimentViewModels.Remove(e);
            CheckEmptyExperimentList();

            CanLaunchExperiment = (m_bIsExperimentListNotEmpty || SelectedLaunchMode.Equals(BatchFile));
        }

        /// <summary>
        ///     Load multiple experiments all at once.
        ///     Used from MainWindowView when the Load experiments button is clicked.
        /// </summary>
        public void LoadExperiments()
        {
            SimionFileData.loadExperiments(ref m_experimentViewModels, appDefinitions, logToFile);
            NotifyOfPropertyChange(() => ExperimentViewModels);

            if (m_experimentViewModels.Count > 0)
                selectedExperiment = m_experimentViewModels[0];

            CheckEmptyExperimentList();
        }


        private List<LoggedExperimentViewModel> m_loggedExperiments = new List<LoggedExperimentViewModel>();

        public List<LoggedExperimentViewModel> LoggedExperiments
        {
            get { return m_loggedExperiments; }
            set { m_loggedExperiments = value; NotifyOfPropertyChange(() => LoggedExperiments); }
        }

        private int LoadLoggedExperiment(XmlNode node, string baseDirectory
            , SimionFileData.LoadUpdateFunction updateFunction)
        {
            LoggedExperimentViewModel newExperiment 
                = new LoggedExperimentViewModel(node, baseDirectory, false, updateFunction);
            LoggedExperiments.Add(newExperiment);
            return newExperiment.ExperimentalUnits.Count;
        }

        /// <summary>
        ///     Pass data to experiment monitor and run experiments defined so far.
        ///     Used from MainWindowView when the launch button is clicked.
        /// </summary>
        public void RunExperiments()
        {
            if (m_shepherdViewModel.herdAgentList.Count == 0)
            {
                CaliburnUtility.ShowWarningDialog(
                    "No Herd agents were detected, so experiments cannot be sent. " +
                    "Consider starting the local agent: \"net start HerdAgent\"", "No agents detected");
                return;
            }

            List<HerdAgentViewModel> freeHerdAgents = new List<HerdAgentViewModel>();

            // Get available herd agents list. Inside the loop to update the list
            ShepherdViewModel.getAvailableHerdAgents(ref freeHerdAgents);
            logToFile("Using " + freeHerdAgents.Count + " agents");

            if (freeHerdAgents.Count == 0)
            {
                CaliburnUtility.ShowWarningDialog(
                    "There is no herd agents availables at this moment. Make sure you have selected at " +
                    "least one available agent and try again.", "No agents detected");
                return;
            }

            int experimentalUnitsCount = 0;
            string batchFileName = "";

            if (SelectedLaunchMode.Equals(BatchFile))
            {
                bool success = SimionFileData.OpenFile(ref batchFileName, SimionFileData.ExperimentBatchFilter
                    , XMLConfig.experimentBatchExtension);
                if (!success)
                    return;

                SimionFileData.LoadExperimentBatchFile(batchFileName, LoadLoggedExperiment);
                experimentalUnitsCount += LoggedExperiments.Sum(experiment => experiment.ExperimentalUnits.Count);
            }
            else
            {
                experimentalUnitsCount = SimionFileData.SaveExperimentBatchFile(ExperimentViewModels,
                    ref batchFileName, logToFile);
            }

            // Experiments will be launched only if exists any experimental unit and the batch file was 
            // safed /loaded succesfully
            if (experimentalUnitsCount > 0 && batchFileName != "")
            {
                logToFile("Running experiment queue remotely: " + experimentalUnitsCount + " experiments");

                m_monitorWindowViewModel = new ExperimentMonitorWindowViewModel(freeHerdAgents, logToFile, batchFileName);

                if (!m_monitorWindowViewModel.RunExperiments())
                    return;

                IsExperimentRunning = true;

                CaliburnUtility.ShowPopupWindow(m_monitorWindowViewModel, "Experiment Monitor", false);
            }
        }

        /// <summary>
        ///     Stops all experiments in progress if there is some.
        /// </summary>
        public void StopExperiments()
        {
            if (!IsExperimentRunning) return;
            m_monitorWindowViewModel.StopExperiments();
            IsExperimentRunning = false;
        }

        /// <summary>
        ///     Show a window that allows you to make reports attending diferent criterias from
        ///     previously completed experiments.
        /// </summary>
        public void ShowReportsWindow()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            CaliburnUtility.ShowPopupWindow(plotEditor, "Badger Reports");
        }
    }
}
