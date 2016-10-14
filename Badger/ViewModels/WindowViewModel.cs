using Badger.Models;
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
    public interface IValidable
    {
        bool validate();
    }
    public interface IGetXml
    {
        List<XmlNode> getXmlNode();
    }

    public class WindowViewModel : PropertyChangedBase
    {
        private BindableCollection<AppViewModel> m_appViewModels= new BindableCollection<AppViewModel>();
        public BindableCollection<AppViewModel> appViewModels { get { return m_appViewModels; }
            set { m_appViewModels = value; NotifyOfPropertyChange(() => appViewModels); } }
        private AppViewModel m_selectedAppViewModel;
        public AppViewModel selectedAppViewModel
        {
            get { return m_selectedAppViewModel; }
            set { m_selectedAppViewModel = value;  NotifyOfPropertyChange(() => selectedAppViewModel); }
        }
        //public ObservableCollection<ConfigNodeViewModel> appConfigNodes
        //{
        //    get { if (m_appViewModel !=null)
        //            return m_appViewModel.children;
        //        return null; }
        //    set { }
        //}



        private CNode _rootnode;
        private ObservableCollection<BranchViewModel> _branches;
        private XmlDocument _doc;

        public ObservableCollection<ValidableAndNodeViewModel> Branch { get { return _branches[0].Class.AllItems; } set { } }

        private ExperimentQueueViewModel m_experimentQueueViewModel = new ExperimentQueueViewModel();
        public ExperimentQueueViewModel experimentQueueViewModel { get { return m_experimentQueueViewModel; }
            set { m_experimentQueueViewModel = value;
            NotifyOfPropertyChange(() => experimentQueueViewModel);            
            }
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
        private void checkStackEmpty()
        {
            bool wasEmpty = !m_bIsExperimentQueueNotEmpty;
            if (wasEmpty != m_experimentQueueViewModel.isEmpty())
            {
                m_bIsExperimentQueueNotEmpty = !m_experimentQueueViewModel.isEmpty();
                NotifyOfPropertyChange(() => bIsExperimentQueueNotEmpty);
            }
        }

        private bool m_bIsExperimentRunning = false;
        public bool bIsExperimentRunning
        {
            get { return m_bIsExperimentRunning; }
            set{m_bIsExperimentRunning= value;
            NotifyOfPropertyChange(()=>bIsExperimentRunning);
            NotifyOfPropertyChange(()=>bIsExperimentNotRunning);} }
        public bool bIsExperimentNotRunning
        {
            get { return !m_bIsExperimentRunning; }
            set { }
        }

        private ObservableCollection<string> _appNames = new ObservableCollection<string>();
        public ObservableCollection<string> appNames { get { return _appNames; } set { } }
      
        //key element is the apps name, and the value is the .xml definition file
        private Dictionary<string,string> apps= new Dictionary<string,string>();

        private string m_selectedApp;

        public string selectedApp { get { return m_selectedApp; } 
            set 
            {
                int index = _appNames.IndexOf(value);
                if (index == -1)
                    return;
                m_selectedApp = value;
                NotifyOfPropertyChange(() => selectedApp);
            } 
        }
        public void newExperiment()
        {
            string xmlDefinitionFile = apps[m_selectedApp];
            AppViewModel newApp = new AppViewModel(xmlDefinitionFile);
            appViewModels.Add( newApp);
            selectedAppViewModel = newApp;
        }
       
        public void Change(object sender)
        {
            var x = sender as System.Windows.Controls.TreeView;
            var y = x.SelectedItem;
        }
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
            CApp.IsInitializing = true;

            getAppsNames();
           
            CApp.IsInitializing = false;
            m_experimentQueueViewModel.setParent(this);
        }
        private void getAppsNames()
        {
            foreach(string app in Directory.GetFiles("..\\config\\apps"))
            {
                char[] spliter = "\\".ToCharArray();
                string[] tmp = app.Split(spliter);
                tmp = tmp[tmp.Length - 1].Split('.');
                string name =tmp[0];
                apps.Add(name, app);
                _appNames.Add(name);
            }
            selectedApp = _appNames[0];
            NotifyOfPropertyChange(() => appNames);
        }

        public ObservableCollection<BranchViewModel> Branches { get { return _branches; } set { } }
        public CNode rootnode
        {
            get
            {
                return _rootnode;
            }
            set
            {
                _rootnode = value;
            }
        }
        
        public void saveExperimentInEditor()
        {
            if (selectedAppViewModel==null || !selectedAppViewModel.validate())
            {
                CaliburnUtility.showWarningDialog("The app can't be validated. See error log.","Error");
                return;
            }

            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment | *.experiment";
            sfd.InitialDirectory = "../experiments";
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../experiments");
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath); 
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                selectedAppViewModel.save(sfd.FileName);
            }
        }
        
       
        public void loadExperiment()
        {
            
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Experiment | *.experiment";
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()),"experiments");
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else
                return;
            
            XmlDocument configDocument = new XmlDocument();
            configDocument.Load(fileDoc);
            XmlNode rootNode = configDocument.LastChild;
            appViewModels.Add( new AppViewModel(apps[rootNode.Name], configDocument));
        }

        public void loadExperimentInEditor(XmlDocument experimentXML)
        {
            _doc.RemoveAll();

            //update the app if we need to
            XmlNode experimentNode = experimentXML.FirstChild;
            if (!experimentNode.Name.Equals(m_selectedApp))
            {
                selectedApp = experimentNode.Name;
                NotifyOfPropertyChange(() => selectedApp);

            }
            foreach (BranchViewModel branch in _branches)
            {
                //we have to find the correct data input for every branch we have in the form. If we do not have data we do nothing
                if (experimentNode.HasChildNodes)
                {
                    foreach (XmlNode dataNode in experimentNode.ChildNodes)
                    {
                        if (dataNode.Name == branch.Name)
                        {
                            Utility.fillTheClass(branch.Class, dataNode);
                        }
                    }
                }
            }

        }

        private void setAsNull()
        {
            foreach(BranchViewModel branch in _branches)
            {
                branch.setAsNull();
            }
        }

        public void clearExperimentQueue()
        {
            if (m_experimentQueueViewModel!=null)
            {
                m_experimentQueueViewModel.clear();
                NotifyOfPropertyChange(() => experimentQueueViewModel);
            }
        }
        public void modifySelectedExperiment()
        {
            if (selectedAppViewModel == null || !selectedAppViewModel.validate())
            {
                CaliburnUtility.showWarningDialog("The app can't be validated. See error log.", "Error");
                return;
            }
            if (m_experimentQueueViewModel!= null)
            {
                XmlDocument document = new XmlDocument();
                XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
                document.AppendChild(newRoot);

                m_experimentQueueViewModel.modifySelectedExperiment(document);
                NotifyOfPropertyChange(() => experimentQueueViewModel);
            }
        }
        public void removeSelectedExperiments()
        {
            if (m_experimentQueueViewModel != null)
            {
                m_experimentQueueViewModel.removeSelectedExperiments();
                NotifyOfPropertyChange(() => experimentQueueViewModel);
                checkStackEmpty();
            }

        }
        public void addExperiment()
        {
            if (selectedAppViewModel == null || !selectedAppViewModel.validate())
            {
                CaliburnUtility.showWarningDialog("The app can't be validated. See error log.", "Error");
                return;
            }

            XmlDocument document = new XmlDocument();

            XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
            document.AppendChild(newRoot);
            //document.Save("copia.tree");
            ExperimentViewModel experiment = new ExperimentViewModel("Experiment", document);
            m_experimentQueueViewModel.addExperiment(experiment);
            NotifyOfPropertyChange(() => experimentQueueViewModel);
            checkStackEmpty();
        }
        public void runExperiments()
        {
            bool bSuccesfulSave= experimentQueueViewModel.save();

            if (bSuccesfulSave)
            {
                bIsExperimentRunning = true;

                runExperimentQueue();

                bIsExperimentRunning = false;

                experimentQueueViewModel.checkLogFilesAlreadyExist();
            }      
        }

        private void runExperimentQueue()
        {
            List<HerdAgentViewModel> freeHerdAgents= new List<HerdAgentViewModel>();
            List<ExperimentViewModel> pendingExperiments = new List<ExperimentViewModel>();

            logToFile("Running experiment queue remotely");

            //get experiment list
            experimentQueueViewModel.getEnqueuedExperimentList(ref pendingExperiments);
            logToFile("Running " + pendingExperiments.Count + " experiments");

            //get available herd agents list. Inside the loop to update the list
            shepherdViewModel.getAvailableHerdAgents(ref freeHerdAgents);
            logToFile("Using " + freeHerdAgents.Count + " agents");

            MonitorWindowViewModel monitorVM = new MonitorWindowViewModel(freeHerdAgents,pendingExperiments,logToFile);

            monitorVM.runExperiments(experimentQueueViewModel.name, true, true);

            CaliburnUtility.showVMDialog(monitorVM, "Experiment execution monitor");
        }
     
        public void loadExperimentQueue()
        {
            try
            {
                m_experimentQueueViewModel.load();
                Task.Run(() => m_experimentQueueViewModel.checkLogFilesAlreadyExist());
            }
            catch (Exception e)
            {
                logToFile("Exception in loadExperimentQueue()");
                logToFile(e.ToString());
            }
            checkStackEmpty();
        }

        public void saveExperimentQueue()
        {
            bool result= m_experimentQueueViewModel.save();
            if (result) logToFile("Succesfully saved " + m_experimentQueueViewModel.experimentQueue.Count + " experiments");
            else logToFile("Error saving the experiment queue");
        }

        public void showPlotWindow()
        {
            List<ExperimentViewModel> experiments= new List<ExperimentViewModel>();
            experimentQueueViewModel.getLoggedExperimentList(ref experiments);
            PlotEditorWindowViewModel plotEditor = new PlotEditorWindowViewModel(experiments);
            CaliburnUtility.showVMDialog(plotEditor, "Plot editor");
        }
    }
}
