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
        static private BindableCollection<ExperimentViewModel> m_experimentViewModels
            = new BindableCollection<ExperimentViewModel>();
        //these two properties interface to the same hidden attribute
        public BindableCollection<ExperimentViewModel> ExperimentViewModels
        {
            get { return m_experimentViewModels; }
            set { m_experimentViewModels = value; NotifyOfPropertyChange(() => ExperimentViewModels); }
        }

        private ExperimentViewModel m_selectedExperiment;

        public ExperimentViewModel SelectedExperiment
        {
            get { return m_selectedExperiment; }
            set
            {
                m_selectedExperiment = value;
                NotifyOfPropertyChange(() => SelectedExperiment);
            }
        }

        static private bool appNameExists(string name)
        {
            return m_experimentViewModels.Any(app => app.Name == name);
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

        public void NewExperiment()
        {
            if (m_selectedAppName == null) return;

            string xmlDefinitionFile = appDefinitions[m_selectedAppName];
            ExperimentViewModel newExperiment = new ExperimentViewModel(xmlDefinitionFile, null, "New");
            ExperimentViewModels.Add(newExperiment);
            NotifyOfPropertyChange(() => ExperimentViewModels);
            CheckEmptyExperimentList();

            CanLaunchExperiment = m_bIsExperimentListNotEmpty;

            SelectedExperiment = newExperiment;
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
            m_shepherdViewModel.SetLogFunction(logToFile);

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

        public void SaveSelectedExperimentOrProject()
        {
            if (SelectedExperiment == null || !SelectedExperiment.Validate())
            {
                CaliburnUtility.ShowWarningDialog("The app can't be validated. See error log.", "Error");
                return;
            }

            string savedFilename = null;
            if (SelectedExperiment.getNumForkCombinations() > 1)
                SimionFileData.SaveExperiments(m_experimentViewModels);
            else
                savedFilename= SimionFileData.SaveExperiment(SelectedExperiment);

            if (savedFilename != null)
                SelectedExperiment.Name = Utility.GetFilename(savedFilename, true, 2);
        }


        public void LoadExperimentOrProject()
        {
            string extension = SimionFileData.ExperimentDescription;
            string filter = SimionFileData.ExperimentOrProjectFilter;

            List<string> filenames = SimionFileData.OpenFileDialogMultipleFiles(extension, filter);
            foreach(string filename in filenames)
            {
                string fileExtension = Utility.GetExtension(filename, 2);
                if (fileExtension== SimionFileData.ExperimentExtension)
                {
                    ExperimentViewModel newExperiment = 
                        SimionFileData.LoadExperiment(appDefinitions, filename);

                    if (newExperiment != null)
                    {
                        ExperimentViewModels.Add(newExperiment);

                        SelectedExperiment = newExperiment;
                    }

                }
                else if (fileExtension == SimionFileData.ProjectExtension)
                {
                    SimionFileData.loadExperiments(ref m_experimentViewModels, appDefinitions, logToFile
                        ,filename);
                    NotifyOfPropertyChange(() => ExperimentViewModels);

                    if (m_experimentViewModels.Count > 0)
                        SelectedExperiment = m_experimentViewModels[0];
                }
            }

            CheckEmptyExperimentList();

            CanLaunchExperiment = m_bIsExperimentListNotEmpty;
        }

        public void ClearExperiments()
        {
            SelectedExperiment = null;
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

            CanLaunchExperiment = (m_bIsExperimentListNotEmpty);
        }

        /// <summary>
        ///     Runs locally the experiment with its currently selected parameters
        /// </summary>
        /// <param name="experiment">The experiment to be run</param>
        public void RunExperimentalUnitLocallyWithCurrentParameters(ExperimentViewModel experiment)
        {
            experiment.RunLocallyCurrentConfiguration();
        }

        /// <summary>
        ///     Shows the wires defined in the current experiment
        /// </summary>
        /// <param name="experiment">The selected experiment</param>
        public void ShowWires(ExperimentViewModel experiment)
        {
            experiment.ShowWires();
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
                = new LoggedExperimentViewModel(node, baseDirectory, false, false, updateFunction);
            LoggedExperiments.Add(newExperiment);
            return newExperiment.ExperimentalUnits.Count;
        }

        
        /// <summary>
        ///     Runs all the experiments open in the editor. Saves a batch file read by the experiment monitor, and also a project to be
        ///     able to modify the experiment and rerun it later
        /// </summary>
        public void RunExperimentsInEditor()
        {
            //Validate all the experiments
            foreach (ExperimentViewModel experiment in m_experimentViewModels)
            {
                if (!experiment.Validate())
                {
                    CaliburnUtility.ShowWarningDialog("The app can't be validated. See error log.", "Error");
                    return;
                }
            }

            string batchFileName = "";

            //Save the experiment batch, read by the Experiment Monitor
            int experimentalUnitsCount = SimionFileData.SaveExperimentBatchFile(ExperimentViewModels,
                ref batchFileName, logToFile);

            //Save the badger project to allow later changes and re-runs of the experiment
            string badgerProjFileName = Utility.RemoveExtension(batchFileName, Utility.NumParts(SimionFileData.ProjectExtension, '.'))
                + SimionFileData.ProjectExtension;
            SimionFileData.SaveExperiments(m_experimentViewModels, badgerProjFileName);

            //Experiments are sent and executed remotely
            logToFile("Running experiment queue remotely");

            m_monitorWindowViewModel = new ExperimentMonitorWindowViewModel(ShepherdViewModel, logToFile);

            CaliburnUtility.ShowPopupWindow(m_monitorWindowViewModel, "Experiment Monitor", false);

            m_monitorWindowViewModel.LoadExperimentBatch(batchFileName);
            m_monitorWindowViewModel.RunExperimentBatch();
        }

        /// <summary>
        /// Shows the experiment monitor in a new window
        /// </summary>
        public void ShowExperimentMonitor()
        {
            ExperimentMonitorWindowViewModel experimentMonitor
                = new ExperimentMonitorWindowViewModel(ShepherdViewModel, logToFile);
            CaliburnUtility.ShowPopupWindow(experimentMonitor, "Experiment Monitor");
        }

        /// <summary>
        ///     Shows the report viewer
        /// </summary>
        public void ShowReportViewer()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            CaliburnUtility.ShowPopupWindow(plotEditor, "Report Viewer");
        }
    }
}
