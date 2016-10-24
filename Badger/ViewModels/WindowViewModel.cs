using Badger.Data;
using System.Collections.ObjectModel;
using System.Xml;
using Caliburn.Micro;
using System.IO;
using System.Collections.Generic;
using System;
using System.Text;
using System.Windows.Forms;
using System.Threading.Tasks;
using Simion;


namespace Badger.ViewModels
{

    public class WindowViewModel : PropertyChangedBase
    {
        static private BindableCollection<AppViewModel> m_appViewModels= new BindableCollection<AppViewModel>();
        public BindableCollection<AppViewModel> appViewModels { get { return m_appViewModels; }
            set { m_appViewModels = value; NotifyOfPropertyChange(() => appViewModels); NotifyOfPropertyChange(() => appQueue); } }
        public BindableCollection<AppViewModel> appQueue { get { return m_appViewModels; } set { } }
        private AppViewModel m_selectedAppViewModel;
        public AppViewModel selectedAppViewModel
        {
            get { return m_selectedAppViewModel; }
            set
            {
                m_selectedAppViewModel = value;
                NotifyOfPropertyChange(() => selectedAppViewModel);
                NotifyOfPropertyChange(() => selectedAppInQueue);
            }
        }
        public AppViewModel selectedAppInQueue
        {
            get { return m_selectedAppViewModel; }
            set { m_selectedAppViewModel = value;
                NotifyOfPropertyChange(() => selectedAppViewModel);
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

        private bool m_bIsExperimentQueueNotEmpty = false;
        public bool bIsExperimentQueueNotEmpty
        {
            get { return m_bIsExperimentQueueNotEmpty; }
            set { m_bIsExperimentQueueNotEmpty = value;
            NotifyOfPropertyChange(() => bIsExperimentQueueNotEmpty);
            }
        }
        private void checkEmptyQueue()
        {
            bool wasEmpty = !m_bIsExperimentQueueNotEmpty;
            if (wasEmpty != (m_appViewModels.Count==0))
            {
                m_bIsExperimentQueueNotEmpty = !(m_appViewModels.Count == 0);
                NotifyOfPropertyChange(() => bIsExperimentQueueNotEmpty);
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
            AppViewModel newApp = new AppViewModel(xmlDefinitionFile);
            appViewModels.Add( newApp);
            NotifyOfPropertyChange(() => appViewModels);
            NotifyOfPropertyChange(() => appQueue);
            checkEmptyQueue();
            selectedAppViewModel = newApp;
        }
       
        //public void Change(object sender)
        //{
        //    var x = sender as System.Windows.Controls.TreeView;
        //    var y = x.SelectedItem;
        //}
        private object m_logFileLock = new object();
        public const string logFilename= "badger-log.txt";
        
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
            foreach(string app in Directory.GetFiles("..\\config\\apps"))
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
            if (selectedAppViewModel==null || !selectedAppViewModel.validate())
            {
                CaliburnUtility.showWarningDialog("The app can't be validated. See error log.","Error");
                return;
            }

            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment | *." + XMLConfig.experimentExtension;
            sfd.InitialDirectory = "../experiments";
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../experiments");
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath); 
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                selectedAppViewModel.save(sfd.FileName,SaveMode.SaveForks);
            }
        }
        
       
        public void loadExperiment()
        {
            
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Experiment | *." + XMLConfig.experimentExtension;
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()),"experiments");
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else
                return;
            
            //open the config file to retrive the app's name before loading it
            XmlDocument configDocument = new XmlDocument();
            configDocument.Load(fileDoc);
            XmlNode rootNode = configDocument.LastChild;
            AppViewModel newApp = new AppViewModel(appDefinitions[rootNode.Name], fileDoc);
            appViewModels.Add(newApp);
            checkEmptyQueue();
            selectedAppViewModel = newApp;
        }
        

        public void clearExperimentQueue()
        {
            if (appViewModels!=null)
            {
                appViewModels.Clear();
            }
        }

        public void removeSelectedExperiments()
        {
            if (selectedAppViewModel != null)
            {
                appViewModels.Remove(selectedAppViewModel);
               // NotifyOfPropertyChange(() => experimentQueueViewModel);
                checkEmptyQueue();
            }
        }

        private List<Experiment> saveExperimentsAsBatch()
        {
            List<Experiment> experimentBatch = new List<Experiment>();

            if (appViewModels.Count == 0) return experimentBatch;
            string batchFilename;

            //Save dialog -> returns the experiment batch file
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment batch | *." + XMLConfig.experimentBatchExtension;
            sfd.InitialDirectory = "../experiments";
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../experiments");
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath);

            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                batchFilename = sfd.FileName;
            }
            else
            {
                logToFile("Error saving the experiment queue");
                return null;
            }

            //clean output directory if it exists
            batchFilename = batchFilename.Split('.')[0];
            batchFilename = Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), batchFilename);
            if (Directory.Exists(batchFilename))
            {
                try
                {
                    Directory.Delete(batchFilename, true);
                }
                catch
                {
                    CaliburnUtility.showWarningDialog("It has not been possible to remove the directory: "
                        + batchFilename + ". Make sure that it's not been using for other app."
                        , "ERROR");
                    logToFile("Error saving the experiment queue");
                    return null;
                }
            }

            XmlDocument experimentXMLDoc = new XmlDocument();
            XmlElement experimentBatchesNode = experimentXMLDoc.CreateElement("Experiments");
            experimentXMLDoc.AppendChild(experimentBatchesNode);

            List<string> names = new List<string>();
            int numCombinations;
            string filePath, folderPath;
            string experimentName;

            foreach (AppViewModel experiment in appViewModels)
            {
                numCombinations = experiment.getNumForkCombinations();
                for (int i = 0; i < numCombinations; i++)
                {
                    experiment.setForkCombination(i);
                    experimentName = experiment.getForkCombinationBaseName(i);
                    folderPath = batchFilename + "/" + experimentName;
                    Directory.CreateDirectory(folderPath);
                    filePath = folderPath + "/" + experimentName + "." + XMLConfig.experimentExtension;
                    experiment.save(filePath, SaveMode.CombineForks);
                    //folders for the batch file and its children experiments
                    XmlElement experimentNode = experimentXMLDoc.CreateElement(experiment.name);
                    experimentNode.SetAttribute(XMLConfig.pathAttribute, filePath);
                    experimentBatchesNode.AppendChild(experimentNode);

                    experimentBatch.Add(new Experiment(experimentName, filePath, experiment.getExeFilename()
                        , experiment.getPrerrequisites()));
                }
            }

            experimentXMLDoc.Save(batchFilename + "." + XMLConfig.experimentBatchExtension);
            logToFile("Succesfully saved " + appViewModels.Count + " experiments");
            
            return experimentBatch;
        }

        //public void loadExperimentQueue()
        //{
        //    string fileDoc = null;
        //    OpenFileDialog ofd = new OpenFileDialog();
        //    ofd.Filter = "Experiment batch | *." + XMLConfig.experimentBatchExtension;
        //    ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");
        //    if (ofd.ShowDialog() == DialogResult.OK)
        //    {
        //        fileDoc = ofd.FileName;
        //    }
        //    else return;
        //    //this doesn't seem to work
        //    //Cursor.Current = Cursors.WaitCursor;
        //    //System.Windows.Forms.Application.DoEvents();

        //    //LOAD THE EXPERIMENT BATCH IN THE QUEUE
        //    XmlDocument batchDoc = new XmlDocument();
        //    batchDoc.Load(fileDoc);
        //    XmlElement fileRoot = batchDoc.DocumentElement;
        //    if (fileRoot.Name != "Experiments")
        //    {
        //        logToFile("ERROR: malformed XML in experiment batch file.");
        //        return;
        //    }

        //    foreach (XmlElement element in fileRoot.ChildNodes)
        //    {
        //        string expName = element.Name;
        //        string path = element.Attributes[XMLConfig.pathAttribute].Value;
        //        if (File.Exists(path))
        //            appViewModels.Add( new AppViewModel(appDefinitions[element.Name], path));
        //    }

        //    Task.Run(() => checkLogFilesAlreadyExist());
        //    checkEmptyQueue();
        //}

        public void runExperiments()
        {
            List<Experiment> experiments= saveExperimentsAsBatch();

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


        private void getEnqueuedExperimentList(ref List<AppViewModel> outList)
        {
            outList.Clear();
            foreach (AppViewModel experiment in appViewModels)
            {
                //(for now) all the experiments are in the queue are considered ready for execution
                outList.Add(experiment);
            }
        }



        public void getLoggedExperimentList(ref List<AppViewModel> outList)
        {
            outList.Clear();
            foreach (AppViewModel experiment in m_appViewModels)
            {
                if (experiment.bLogDataAvailable)
                    outList.Add(experiment);
            }
        }




        public void showPlotWindow()
        {
            List<AppViewModel> experiments= new List<AppViewModel>();
            getLoggedExperimentList(ref experiments);
            PlotEditorWindowViewModel plotEditor = new PlotEditorWindowViewModel(experiments);
            CaliburnUtility.showVMDialog(plotEditor, "Plot editor");
        }
    }
}
