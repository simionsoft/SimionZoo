using AppXML.Models;
using AppXML.Data;
using System.Collections.ObjectModel;
using System.Xml;
using System.Windows;
using Caliburn.Micro;
using System.Windows.Forms;
using System.IO;
using System.Dynamic;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;
using System.IO.Pipes;
using System.Xml.Linq;
using System.Linq;
using System.Xml.XPath;
using System.Windows.Media;
using System.Threading;
using System.Globalization;
using System.Collections.Concurrent;
using Herd;
using AppXML.ViewModels;


namespace AppXML.ViewModels
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
        
        private CNode _rootnode;
        private ObservableCollection<BranchViewModel> _branches;
        private XmlDocument _doc;
        //private RightTreeViewModel _graf;
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

        private ObservableCollection<string> _apps = new ObservableCollection<string>();
        public ObservableCollection<string> Apps { get { return _apps; } set { } }
      
        private string[] apps;
        private string selectedApp;

        public string SelectedApp { get { return selectedApp; } 
            set 
            {
                CNode.cleanAll();
                CApp.cleanApp();

                int index = _apps.IndexOf(value);
                if (index == -1)
                    return;
                selectedApp = value;
                CApp.IsInitializing = true;
                //_rootnode = Utility.getRootNode(apps[index]);
                //_branches = _rootnode.children;
                //_doc = (this._rootnode as CApp).document;
                CApp.IsInitializing = false;
                NotifyOfPropertyChange(() => Branch);
                NotifyOfPropertyChange(() => rootnode);
            } 
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
             //_windowManager = windowManager;
            CApp.IsInitializing = true;
            apps = Directory.GetFiles("..\\config\\apps");
            getAppsNames();
            selectedApp = Apps[0];
            _rootnode = Utility.getRootNode(apps[0]);
            _branches = _rootnode.children;
            _doc = (this._rootnode as CApp).document;
            CApp.IsInitializing = false;
            m_experimentQueueViewModel.setParent(this);
        }
        private void getAppsNames()
        {
            foreach(string app in apps)
            {
                char[] spliter = "\\".ToCharArray();
                string[] tmp = app.Split(spliter);
                tmp = tmp[tmp.Length - 1].Split('.');
                string name =tmp[0];
                _apps.Add(name);
            
            }
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
            if (!validate())
                return;
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment | *.experiment";
            sfd.InitialDirectory = "../experiments";
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../experiments");
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath); 
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                _doc.Save(sfd.FileName);
            }
        }
        
        private bool validate()
        {
            _doc.RemoveAll();
            XmlNode rootNode = _doc.CreateElement(_rootnode.name);
            foreach (BranchViewModel branch in _branches)
            {
                if (!branch.validate())
                {
                    DialogViewModel dvm = new DialogViewModel(null, "Error validating the form. Please check form", DialogViewModel.DialogType.Info);
                    dynamic settings = new ExpandoObject();
                    settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
                    settings.ShowInTaskbar = true;
                    settings.Title = "ERROR";

                    new WindowManager().ShowDialog(dvm, null, settings);

                    return false;
                }
                else
                {
                    rootNode.AppendChild(branch.getXmlNode()[0]);
                }
            }
            _doc.AppendChild(rootNode);
            return true;
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
            
            XmlDocument loadedDocument = new XmlDocument();
            loadedDocument.Load(fileDoc);

            loadExperimentInEditor(loadedDocument);       
        }
       
        public void loadExperimentInEditor(XmlDocument experimentXML)
        {
            _doc.RemoveAll();

            //update the app if we need to
            XmlNode experimentNode = experimentXML.FirstChild;
            if (!experimentNode.Name.Equals(selectedApp))
            {
                SelectedApp = experimentNode.Name;
                NotifyOfPropertyChange(() => SelectedApp);

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
            if (!validate())
                return;
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
            if (!validate())
                return;
            
            XmlDocument document = new XmlDocument();

            XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
            document.AppendChild(newRoot);
            //document.Save("copia.tree");
            AppXML.ViewModels.ExperimentViewModel experiment = new AppXML.ViewModels.ExperimentViewModel("Experiment", document);
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
            }      
        }
        

        public void updateHerdAgents()
        {
            CJob updateJob= new CJob();
            CTask updateTask = new CTask();
            updateTask.name = "RunServiceUpdater";
            updateTask.arguments = "";
            updateTask.pipe = "";
            updateTask.exe = "../Debug/HerdAgentUnattendedUpdater.msi";
            updateJob.name = "HerdAgentServiceUpdate";
            updateJob.tasks.Add(updateTask);
            updateJob.inputFiles.Add(updateTask.exe);
            List<HerdAgentViewModel> agentList= new List<HerdAgentViewModel>();
            m_shepherdViewModel.getAvailableHerdAgents(ref agentList);

            CancellationTokenSource cancelTokenSource = new CancellationTokenSource();

           Task.Factory.StartNew(() =>
            {
                foreach (var agent in agentList)
                {
                    Shepherd shepherd= new Shepherd();
                    
                    if (agent.isAvailable)
                    {
                        if (shepherd.connectToHerdAgent(agent.ipAddress))
                        {
                            try
                            {
                                shepherd.startEnqueueingAsyncWriteOps(); //we enqueue them to be able to wait for them to finish in waitAsyncWriteOpsToFinish()
                                shepherd.SendJobQuery(updateJob, cancelTokenSource.Token);
                                shepherd.waitAsyncWriteOpsToFinish();
                            }
                            catch (Exception ex)
                            {
                                logToFile("Exception in update task");
                                logToFile(ex.ToString());
                            }
                            Thread.Sleep(1000);
                            shepherd.disconnect();
                        }
                    }
                }
            });
           cancelTokenSource.Dispose();
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

            ExperimentQueueMonitorViewModel monitorVM = new ExperimentQueueMonitorViewModel(freeHerdAgents,pendingExperiments);
            monitorVM.setLogFunction(logToFile);
            monitorVM.runExperiments(experimentQueueViewModel.name, true, true);

            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = false;
            settings.Title = "Experiment Monitor";
            new WindowManager().ShowDialog(monitorVM, null, settings);
        }
     
        public void loadExperimentQueue()
        {
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Experiment batch | *.exp-batch";
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else
                return;
            //this doesn't seem to work
            //Cursor.Current = Cursors.WaitCursor;
            //System.Windows.Forms.Application.DoEvents();

            //LOAD THE EXPERIMENT BATCH IN THE QUEUE
            XmlDocument batchDoc = new XmlDocument();
            batchDoc.Load(fileDoc);
            XmlElement fileRoot = batchDoc.DocumentElement;
            if (fileRoot.Name != "Experiments")
                return;

            foreach (XmlElement element in fileRoot.ChildNodes)
            {
                try
                {
                    string expName = element.Name;
                    string path = element.Attributes["Path"].Value;
                    if (File.Exists(path))
                    {
                        XmlDocument expDocument = new XmlDocument();
                        expDocument.Load(path);
                        m_experimentQueueViewModel.addExperiment(element.Name, expDocument);
                        checkStackEmpty();
                    }
                }
                catch (Exception e)
                {
                    logToFile("Exception in loadExperimentQueue()");
                    logToFile(e.ToString());
                }
            }

            m_experimentQueueViewModel.markModified(true);

        }

        public void saveExperimentQueue()
        {
            bool result= m_experimentQueueViewModel.save();
            if (result) logToFile("Succesfully saved " + m_experimentQueueViewModel.experimentQueue.Count + " experiments");
            else logToFile("Error saving the experiment queue");
         }
       
    }
}
