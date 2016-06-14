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
using System.Xml.XPath;
using System.Windows.Media;
using System.Threading;
using System.Globalization;
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
    //[Export(typeof(WindowViewModel))]
    public class WindowViewModel : PropertyChangedBase
    {
        public double ControlHeight
        {
            get
            {
                return (int)System.Windows.SystemParameters.PrimaryScreenHeight;
            }
            set { }
        }
        public double ControlWidth
        {
            get
            {
                return System.Windows.SystemParameters.PrimaryScreenWidth;
            }
            set { }
        }
        //public int MaxHeight
        //{
        //    get
        //    {
        //        return (int)System.Windows.SystemParameters.PrimaryScreenHeight-200;
        //    }
        //    set { }
        //}
        //public double MaxWidth 
        //{
        //    get 
        //    {
        //        return (System.Windows.SystemParameters.PrimaryScreenWidth - 220)/2; 
        //    } 
        //    set { } 
        //}
        private CNode _rootnode;
        private ObservableCollection<BranchViewModel> _branches;
        private XmlDocument _doc;
        //private RightTreeViewModel _graf;
        public ObservableCollection<ValidableAndNodeViewModel> Branch { get { return _branches[0].Class.AllItems; } set { } }
        //public RightTreeViewModel Graf { get { return _graf; }
        //    set 
        //    {
        //        _graf = value; 
        //        NotifyOfPropertyChange(() => Graf); 
        //        NotifyOfPropertyChange(()=> AddChildVisible);
        //        NotifyOfPropertyChange(()=> RemoveChildVisible);

        //    } 
        //}
        private ExperimentQueueViewModel m_experimentQueueViewModel = new ExperimentQueueViewModel();
        public ExperimentQueueViewModel experimentQueueViewModel { get { return m_experimentQueueViewModel; }
            set { m_experimentQueueViewModel = value;
            NotifyOfPropertyChange(() => experimentQueueViewModel);
            }
        }
        private ShepherdViewModel m_shepherdViewModel = new ShepherdViewModel();
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
        private bool _isSelectedNodeLeaf = false;
        public bool IsSelectedNodeLeafBool { get { return _isSelectedNodeLeaf; } set { _isSelectedNodeLeaf = value; NotifyOfPropertyChange(() => IsSelectedNodeLeaf); } }
        public string IsSelectedNodeLeaf { get { if (_isSelectedNodeLeaf)return "Visible"; else return "Hidden"; } set { } }


        
        private string[] apps;
        private string selectedApp;
        //public SolidColorBrush Color { get { return new SolidColorBrush((Color)ColorConverter.ConvertFromString("White")); } set { } }
 
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
                _rootnode = Utility.getRootNode(apps[index]);
                _branches = _rootnode.children;
                _doc = (this._rootnode as CApp).document;
               // _graf = null;
                CApp.IsInitializing = false;
                NotifyOfPropertyChange(() => Branch);
               // NotifyOfPropertyChange(() => Graf);
                NotifyOfPropertyChange(() => rootnode);
             //   NotifyOfPropertyChange(() => RemoveChildVisible);
             //   NotifyOfPropertyChange(() => AddChildVisible);

            } 
        }
       
        public void Change(object sender)
        {
            var x = sender as System.Windows.Controls.TreeView;
            var y = x.SelectedItem;
        }
        public WindowViewModel()
        {
           
             
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
        //public void ModifySelectedLeaf()
        //{
        //    if (!validate())
        //        return;
        //    XmlDocument document = new XmlDocument();
        //    XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
        //    document.AppendChild(newRoot);
        //    if ( Utility.findDifferences(document, Graf.SelectedTreeNode.Doc)==null)
        //        return;
        //    Graf.SelectedTreeNode.Doc = document;
        //    Graf.SelectedTreeNode.updateDif();
        //    NotifyOfPropertyChange(() => Graf.Tree);
        //    Graf.LoadedAndModified = true;
        //}
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
                    DialogViewModel dvm = new DialogViewModel(null, "Error validating de form. Please check form", DialogViewModel.DialogType.Info);
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
            //Graf = null;
            
           
           
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

        private CancellationTokenSource m_cancelTokenSource = new CancellationTokenSource();
        
        public void stopExperiments()
        {
            if (m_cancelTokenSource != null)
                m_cancelTokenSource.Cancel();
            experimentQueueViewModel.resetState();
        }

        void runExperimentQueue()
        {
            Task.Factory.StartNew(() =>
                {
                    if (shepherdViewModel.herdAgentList.Count > 0)
                    {
                        //RUN REMOTELY
                        runExperimentQueueRemotely();
                    }
                    else
                    {
                        //RUN LOCALLY
                    }
                });
        }

        async Task<bool> sendJobAsync(HerdAgentViewModel herdAgentVM,List<ExperimentViewModel> experimentsVM, CancellationToken cancelToken)
        {
            Shepherd shepherd = new Shepherd();
            //Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            bool bSuccess= false;

            try
            {
                experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.WAITING_EXECUTION);
                CJob job = createJob(experimentQueueViewModel.name, experimentsVM);

                bool bConnected= shepherd.connectToHerdAgent(herdAgentVM.ipAddress);
                if (bConnected)
                {
                    experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.SENDING);
                    herdAgentVM.status = "Sending job query";
                    shepherd.SendJobQuery(job);
                    experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RUNNING);
                    herdAgentVM.status = "Executing job query";

                    while (true)
                    {
                        int numBytesRead= await shepherd.readAsync();
                        string xmlItem= shepherd.m_xmlStream.processNextXMLItem();
                        
                        while (xmlItem != "")
                        {

                            cancelToken.ThrowIfCancellationRequested();

                            XmlDocument doc = new XmlDocument();
                            doc.LoadXml(xmlItem);
                            XmlNode e = doc.DocumentElement;
                            string key = e.Name;
                            XmlNode message = e.FirstChild;
                            ExperimentViewModel experimentVM = experimentsVM.Find(exp => exp.name == key);
                            string content = message.InnerText;
                            if (experimentVM != null)
                            {
                                if (message.Name == "Progress")
                                {


                                    double progress = double.Parse(content, CultureInfo.InvariantCulture);


                                    //double progress = Convert.ToDouble(content);
                                    experimentVM.progress = Convert.ToInt32(progress);
                                }
                                else if (message.Name == "Message")
                                {
                                    experimentVM.addStatusInfoLine(content);
                                }
                                else if (message.Name == "End")
                                {
                                    if (content == "Ok")
                                        experimentVM.state = ExperimentViewModel.ExperimentState.WAITING_RESULT;
                                    else experimentVM.state = ExperimentViewModel.ExperimentState.ERROR;
                                }
                            }
                            else
                            {
                                if (key == XMLStream.m_defaultMessageType)
                                {
                                    if (content == CJobDispatcher.m_endMessage)
                                    {
                                        experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RECEIVING);
                                        herdAgentVM.status = "Receiving output files";
                                        shepherd.ReceiveJobResult();
                                        experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.FINISHED);
                                        herdAgentVM.status = "Finished";
                                        bSuccess = true;
                                        break;
                                    }
                                    else if (content == CJobDispatcher.m_errorMessage)
                                    {
                                        herdAgentVM.status = "Error in job";
                                        experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.ERROR);
                                        bSuccess = false;
                                        break;
                                    }
                                }
                            }
                            xmlItem= shepherd.m_xmlStream.processNextXMLItem();
                        }
                    }
                }
            }        
            catch (OperationCanceledException)
            {
                //quit remote jobs
                shepherd.writeMessage(Shepherd.m_quitMessage, true);
                experimentsVM.ForEach((exp) => exp.resetState());
                herdAgentVM.status = "";
                bSuccess = false;
            }
            catch (Exception ex)
            {
                //to do: aqui salta cuando hay cualquier problema. Si hay problema hay que volver a lanzarlo
                //mandar a cualquier maquina que este libre
                //this.reRun(myPipes.Values);
                Console.WriteLine(ex.StackTrace);
                bSuccess = false;
            }
            finally
            {
                shepherdViewModel.shepherd.disconnect();
            }
            return bSuccess;
        }

       //void assignExperiments(List<HerdAgentViewModel> agents
       //    ,ref List<ExperimentViewModel> pendingExperiments
       //    ,ref List<ExperimentViewModel> assignedExperiments
       //    ,ref Dictionary<HerdAgentViewModel, List<ExperimentViewModel>> assignments)
       // {
       //     assignments.Clear();

       //     int numAssignedExperiments = 0;

       //     foreach (HerdAgentViewModel agent in agents)
       //     {
       //         if (pendingExperiments.Count==0) break;

       //         List<ExperimentViewModel> agentExperiments
       //             = new List<ExperimentViewModel>();

       //         int amount= agent.numProcessors - 1;
       //         for (int i = 0; pendingExperiments.Count>0 && i < amount; i++)
       //         {
       //             agentExperiments.Add(pendingExperiments[0]);
       //             assignedExperiments.Add(pendingExperiments[0]); 
       //             pendingExperiments.RemoveAt(0);
       //         }

       //         assignments.Add(agent, agentExperiments);
       //         numAssignedExperiments += amount;
       //     }
       // }

        //bool sendJobAndMonitor(HerdAgentViewModel herdAgentVM, List<ExperimentViewModel> experimentsVM, CancellationToken cancelToken)
        //{
        //    bool bSuccess = false;
        //    experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.WAITING_EXECUTION);
        //    CJob job = createJob(experimentQueueViewModel.name, experimentsVM);

        //    Shepherd shepherd = new Shepherd();
        //    Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

        //    try
        //    {
        //        shepherd.connectToHerdAgent(herdAgentVM.ipAddress);

        //        experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.SENDING);
        //        herdAgentVM.status = "Sending job query";
        //        shepherd.SendJobQuery(job);
        //        experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RUNNING);
        //        herdAgentVM.status = "Executing job query";

        //        string xmlItem;
        //        while (true)
        //        {
        //            shepherd.read();
        //            xmlItem = shepherd.m_xmlStream.processNextXMLItem();

        //            cancelToken.ThrowIfCancellationRequested();

        //            if (xmlItem != "")
        //            {
        //                XmlDocument doc = new XmlDocument();
        //                doc.LoadXml(xmlItem);
        //                XmlNode e = doc.DocumentElement;
        //                string key = e.Name;
        //                XmlNode message = e.FirstChild;
        //                ExperimentViewModel experimentVM = experimentsVM.Find(exp => exp.name == key);
        //                string content = message.InnerText;
        //                if (experimentVM != null)
        //                {
        //                    if (message.Name == "Progress")
        //                    {
        //                        double progress = Convert.ToDouble(content);
        //                        experimentVM.progress = Convert.ToInt32(progress);
        //                    }
        //                    else if (message.Name == "Message")
        //                    {
        //                        experimentVM.addStatusInfoLine(content);
        //                    }
        //                    else if (message.Name == "End")
        //                    {
        //                        if (content == "Ok")
        //                            experimentVM.state = ExperimentViewModel.ExperimentState.WAITING_RESULT;
        //                        else experimentVM.state = ExperimentViewModel.ExperimentState.ERROR;
        //                    }
        //                }
        //                else
        //                {
        //                    if (key == XMLStream.m_defaultMessageType)
        //                    {
        //                        if (content == CJobDispatcher.m_endMessage)
        //                        {
        //                            experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RECEIVING);
        //                            herdAgentVM.status = "Receiving output files";
        //                            shepherd.ReceiveJobResult();
        //                            experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.FINISHED);
        //                            herdAgentVM.status = "Finished";
        //                            bSuccess = true;
        //                            break;
        //                        }
        //                        else if (content == CJobDispatcher.m_errorMessage)
        //                        {
        //                            herdAgentVM.status = "Error in job";
        //                            experimentsVM.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.ERROR);
        //                            bSuccess = false;
        //                            break;
        //                        }
        //                    }
        //                }
        //            }
        //        }
        //    }
        //    catch (OperationCanceledException)
        //    {
        //        //quit remote jobs
        //        shepherd.writeMessage(Shepherd.m_quitMessage, true);
        //        experimentsVM.ForEach((exp) => exp.resetState());
        //        herdAgentVM.status = "";
        //    }
        //    catch (Exception ex)
        //    {
        //        //to do: aqui salta cuando hay cualquier problema. Si hay problema hay que volver a lanzarlo
        //        //mandar a cualquier maquina que este libre
        //        //this.reRun(myPipes.Values);
        //        Console.WriteLine(ex.StackTrace);
        //    }
        //    finally
        //    {
        //        shepherdViewModel.shepherd.disconnect();
        //    }
        //    return bSuccess;
        //}

        //private object m_queueLock = new object();
        //void enqueueExperiments(ref List<ExperimentViewModel> experimentList, List<ExperimentViewModel> experiments)
        //{
        //    lock(m_queueLock)
        //    {
        //        experimentList.AddRange(experiments);
        //    }
        //}

        async void runExperimentQueueRemotely()
        {
            List<HerdAgentViewModel> freeHerdAgents= new List<HerdAgentViewModel>();
            List<HerdAgentViewModel> usedHerdAgents= new List<HerdAgentViewModel>();
            List<ExperimentViewModel> pendingExperiments = new List<ExperimentViewModel>();
            List<ExperimentViewModel> assignedExperiments = new List<ExperimentViewModel>();

            //get experiment list
            experimentQueueViewModel.getEnqueuedExperimentList(ref pendingExperiments);
            experimentQueueViewModel.enableEdition(false);

            List<Task<bool>> remoteTasks = new List<Task<bool>>();
            while (pendingExperiments.Count>0 && !m_cancelTokenSource.IsCancellationRequested)
            {
                //get available herd agents list. Inside the loop to update the list
                shepherdViewModel.getAvailableHerdAgents(ref freeHerdAgents);

                List<ExperimentViewModel> experimentList;
                //assign experiments to free agents
                while (pendingExperiments.Count>0 && freeHerdAgents.Count>0)
                {
                    HerdAgentViewModel agentVM= freeHerdAgents[0];
                    freeHerdAgents.RemoveAt(0);
                    usedHerdAgents.Add(agentVM);
                    int numProcessors= Math.Max(1,agentVM.numProcessors -1); //we free one processor

                    experimentList= new List<ExperimentViewModel>();
                    for (int i = 0; i < Math.Min(numProcessors, pendingExperiments.Count); i++ )
                    {
                        experimentList.Add(pendingExperiments[0]);
                        assignedExperiments.Add(pendingExperiments[0]);
                        pendingExperiments.RemoveAt(0);
                    }
                    remoteTasks.Add(sendJobAsync(agentVM,experimentList,m_cancelTokenSource.Token));
                }
                //wait for any of the tasks to finish to check if we have to send any more jobs
                Task<bool> finishedTask = await Task.WhenAny(remoteTasks);

                bool bReturnValue = await finishedTask;

                if (bReturnValue || m_cancelTokenSource.IsCancellationRequested)
                    remoteTasks.Remove(finishedTask);
                else
                    finishedTask.Start();
            }
            experimentQueueViewModel.enableEdition(true);
        }

        private CJob createJob(string experimentName,List<ExperimentViewModel> experiments)
        {
            CJob job = new CJob();
            job.name = experimentName;
            //prerrequisites
            if (Models.CApp.pre != null)
            {
                foreach (string prerec in Models.CApp.pre)
                    job.inputFiles.Add(prerec);
            }
            //tasks, inputs and outputs
            foreach (ExperimentViewModel experiment in experiments)
            {
                CTask task = new CTask();
                task.name = experiment.name;
                task.exe = Models.CApp.EXE;
                task.arguments = experiment.filePath + " " + experiment.pipeName;
                task.pipe = experiment.pipeName;
                job.tasks.Add(task);
                //add exe file to inputs
                if (!job.inputFiles.Contains(task.exe))
                    job.inputFiles.Add(task.exe);
                //add experiment file to inputs
                if (!job.inputFiles.Contains(experiment.filePath))
                    job.inputFiles.Add(experiment.filePath);
                Utility.getInputsAndOutputs(experiment.filePath, ref job);
            }
            return job;
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
                    Console.WriteLine(e.StackTrace);
                }
            }

            m_experimentQueueViewModel.markModified(true);

        }

        public void saveExperimentQueue()
        {
            m_experimentQueueViewModel.save();
         }
       
    }
}
