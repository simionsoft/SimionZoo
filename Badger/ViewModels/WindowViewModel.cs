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
        static private BindableCollection<AppViewModel> m_appViewModels= new BindableCollection<AppViewModel>();
        //these two properties interface to the same hidden attribute
        public BindableCollection<AppViewModel> tabControlExperiments { get { return m_appViewModels; }
            set { m_appViewModels = value; NotifyOfPropertyChange(() => tabControlExperiments); NotifyOfPropertyChange(() => listControlExperiments); } }
        public BindableCollection<AppViewModel> listControlExperiments { get { return m_appViewModels; } set { } }
        private AppViewModel m_selectedAppViewModel;
        public AppViewModel selectedTabControlExperiment
        {
            get { return m_selectedAppViewModel; }
            set
            {
                m_selectedAppViewModel = value;
                NotifyOfPropertyChange(() => selectedTabControlExperiment);
                NotifyOfPropertyChange(() => selectedAppInQueue);
            }
        }
        public AppViewModel selectedAppInQueue
        {
            get { return m_selectedAppViewModel; }
            set { m_selectedAppViewModel = value;
                NotifyOfPropertyChange(() => selectedTabControlExperiment);
                NotifyOfPropertyChange(() => selectedAppInQueue);
                }
        }

        static private bool appNameExists(string name)
        {
            foreach(AppViewModel app in m_appViewModels)
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

        private ShepherdViewModel m_shepherdViewModel;
        public ShepherdViewModel shepherdViewModel { get { return m_shepherdViewModel; } set { } }

        private bool m_bIsExperimentListNotEmpty = false;
        public bool bIsExperimentListNotEmpty
        {
            get { return m_bIsExperimentListNotEmpty; }
            set { m_bIsExperimentListNotEmpty = value;
            NotifyOfPropertyChange(() => bIsExperimentListNotEmpty);
            }
        }
        private void checkEmptyExperimentList()
        {
            bool wasEmpty = !m_bIsExperimentListNotEmpty;
            if (wasEmpty != (m_appViewModels.Count==0))
            {
                m_bIsExperimentListNotEmpty = !(m_appViewModels.Count == 0);
                NotifyOfPropertyChange(() => bIsExperimentListNotEmpty);
            }
        }

        private ObservableCollection<string> m_appNames= new ObservableCollection<string>();
        public ObservableCollection<string> appNames { get { return m_appNames; } set { } }
      
        //key element is the apps name, and the value is the .xml definition file
        private Dictionary<string,string> appDefinitions= new Dictionary<string,string>();

        private string m_selectedAppName;

        public string selectedAppName { get { return m_selectedAppName; } 
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
            AppViewModel newApp = new AppViewModel(xmlDefinitionFile,null,"New");
            tabControlExperiments.Add( newApp);
            NotifyOfPropertyChange(() => tabControlExperiments);
            NotifyOfPropertyChange(() => listControlExperiments);
            checkEmptyExperimentList();
            selectedTabControlExperiment = newApp;
        }
       
        private object m_logFileLock = new object();
        public const string logFilename= SimionFileData.badgerLogFile;
        
        public void logToFile(string logMessage)
        {
            lock (m_logFileLock)
            {
                string text = DateTime.Now.ToShortDateString() + " " +
                                DateTime.Now.ToShortTimeString() + ": " + logMessage + "\n";
                FileStream file;
                if (!File.Exists(logFilename))
                    file = File.Create(logFilename);
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

            loadAppDefinitions();
        }
        private void loadAppDefinitions()
        {
#if DEBUG
            foreach(string app in Directory.GetFiles(SimionFileData.appConfigRelativeDir,"*.Debug.xml"))
#else
            foreach (string app in Directory.GetFiles(SimionFileData.appConfigRelativeDir))
#endif
            {
                char[] spliter = "\\".ToCharArray();
                string[] tmp = app.Split(spliter);
                tmp = tmp[tmp.Length - 1].Split('.');
                string name =tmp[0];
                appDefinitions.Add(name, app);
                m_appNames.Add(name);
            }
            selectedAppName = m_appNames[0];
            NotifyOfPropertyChange(() => appNames);
        }

        public void saveExperimentInEditor()
        {
            if (selectedTabControlExperiment==null || !selectedTabControlExperiment.validate())
            {
                CaliburnUtility.showWarningDialog("The app can't be validated. See error log.","Error");
                return;
            }

            SimionFileData.saveExperiment(selectedTabControlExperiment);
        }

        public void saveExperiments()
        {
            SimionFileData.saveExperiments(m_appViewModels);
        }


        public void loadExperiment()
        {
            AppViewModel newApp = SimionFileData.loadExperiment(appDefinitions);
            tabControlExperiments.Add(newApp);
            checkEmptyExperimentList();
            selectedTabControlExperiment = newApp;
        }
        

        public void clearExperimentQueue()
        {
            selectedTabControlExperiment = null;
            if (tabControlExperiments != null) tabControlExperiments.Clear();
            if (listControlExperiments != null) listControlExperiments.Clear();
        }

        public void removeSelectedExperiments()
        {
            if (selectedTabControlExperiment != null)
            {
                tabControlExperiments.Remove(selectedTabControlExperiment);
                NotifyOfPropertyChange(() => listControlExperiments);
                checkEmptyExperimentList();
            }
        }

        //BADGER files
        public void loadExperiments()
        {
            SimionFileData.loadExperiments(ref m_appViewModels, appDefinitions, logToFile);
            NotifyOfPropertyChange(() => tabControlExperiments);
            NotifyOfPropertyChange(() => listControlExperiments);
            if (m_appViewModels.Count > 0)
                selectedTabControlExperiment = m_appViewModels[0];
            checkEmptyExperimentList();
        }

        public void runExperiments()
        {
            List<Experiment> experiments = new List<Experiment>();
            experiments= SimionFileData.saveExperimentBatchFile(tabControlExperiments, logToFile);

            if (experiments!=null && experiments.Count>0)
            {
                List<HerdAgentViewModel> freeHerdAgents = new List<HerdAgentViewModel>();

                logToFile("Running experiment queue remotely: " + experiments.Count + " experiments");

                //get available herd agents list. Inside the loop to update the list
                shepherdViewModel.getAvailableHerdAgents(ref freeHerdAgents);
                logToFile("Using " + freeHerdAgents.Count + " agents");

                MonitorWindowViewModel monitorVM = new MonitorWindowViewModel(freeHerdAgents, experiments, logToFile);

                monitorVM.runExperiments(true, true);

                CaliburnUtility.showVMDialog(monitorVM, "Experiment execution monitor");
            }      
        }



        public void showPlotWindow()
        {
            PlotEditorWindowViewModel plotEditor = new PlotEditorWindowViewModel();
            CaliburnUtility.showVMDialog(plotEditor, "Plot editor");
        }
    }
}
