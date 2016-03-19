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
        private CNode _rootnode;
        private ObservableCollection<BranchViewModel> _branches;
        private XmlDocument _doc;
        private RightTreeViewModel _graf;

        public RightTreeViewModel Graf { get { return _graf; } set { } }
        private ObservableCollection<string> _apps = new ObservableCollection<string>();
        public ObservableCollection<string> Apps { get { return _apps; } set { } }
        private string[] apps;
        private string selectedApp;
        public string SelectedApp { get { return selectedApp; } 
            set 
            {
                CNode.cleanAll();
                CApp.cleanAll();

                int index = _apps.IndexOf(value);
                selectedApp = Apps[index];
                _rootnode = Utility.getRootNode(apps[index]);
                _branches = _rootnode.children;
                _doc = (this._rootnode as CApp).document;
                Graf = null;
                NotifyOfPropertyChange(() => Branches);
                NotifyOfPropertyChange(() => Graf);
            } 
        }
        /*
        public WindowViewModel(string[] apps, ObservableCollection<string> Apps, int index)
        {
            CNode.cleanAll();
            CApp.cleanAll();
            this.apps = apps;
            _apps = Apps;
            selectedApp = Apps[index];
            _rootnode = Utility.getRootNode(apps[index]);
            _branches = _rootnode.children;
            _doc = (this._rootnode as CApp).document;
        }
        private readonly IWindowManager _windowManager;
 
        [ImportingConstructor]*/
        public WindowViewModel()
        {
             //_windowManager = windowManager;
            apps = Directory.GetFiles("..\\apps");
            getAppsNames();
            selectedApp = Apps[0];
            _rootnode = Utility.getRootNode(apps[0]);
            _branches = _rootnode.children;
            _doc = (this._rootnode as CApp).document;

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

        public void Save()
        {
            if (!validate())
                return;
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "XML-File | *.xml";
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../experiments");
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath); if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
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
        public void fillTheClass(ClassViewModel cvm, XmlNode dataNode)
        {
            
            //here we have to set the new data such as: selected choice element, default values for every *.value, etc.
            bool dataSet = false;
            bool multiStarted = false;
            string currentMulti = "";
            int multiIndex = 0;
            foreach (XmlNode data in dataNode)
            {
                dataSet = false;
                string dataTag = data.Name;
                string branchTag = null;
                string[] splitedTag = null;
                //once we have the tag we have to find out if the branch has this tag somewhere
                if (cvm.Choice != null)
                {
                    branchTag = cvm.Choice.Tag;
                    splitedTag = branchTag.Split('/');
                    XmlNode tmp = data;
                    foreach (string tag in splitedTag)
                    {
                        if (tag == dataTag)
                        {
                            if (tag == splitedTag[splitedTag.Length - 1])
                            {
                                if (tmp.HasChildNodes)
                                {
                                    XmlNode choiceElementData = tmp.FirstChild;
                                    string name = choiceElementData.Name;
                                    cvm.Choice.setSelectedItem(name);
                                    fillTheClass(cvm.Choice.Class, choiceElementData);
                                    dataSet = true;

                                }
                            }
                            else
                            {
                                if (data.HasChildNodes)
                                {
                                    tmp = data.FirstChild;
                                    dataTag = tmp.Name;
                                }

                            }
                        }
                    }
                }
                foreach (ValidableAndNodeViewModel view in cvm.AllItems)
                {
                    if (!dataSet)
                    {
                        MultiXmlNodeRefViewModel itemMultiXml = view as MultiXmlNodeRefViewModel;
                        if (itemMultiXml != null)
                        {
                            string itemTag = itemMultiXml.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {

                                        if (multiStarted == false || currentMulti != tag)
                                        {
                                            currentMulti = tag;
                                            multiIndex = 0;
                                            multiStarted = true;
                                            itemMultiXml.Header.SelectedOption = tmp.InnerText;
                                            if (itemMultiXml.AddedXml != null)
                                                itemMultiXml.AddedXml.Clear();
                                            break;
                                        }
                                        else
                                        {
                                            if (itemMultiXml.AddedXml != null)
                                            {
                                                int index = itemMultiXml.AddedXml.Count;
                                                if (index == 0 || multiIndex >= index)
                                                    itemMultiXml.AddNew();
                                            }
                                            else
                                                itemMultiXml.AddNew();
                                            itemMultiXml.AddedXml[multiIndex].SelectedOption = tmp.InnerText;
                                            multiIndex++;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                        }
                            XMLNodeRefViewModel itemXml = view as XMLNodeRefViewModel;
                            if (itemXml != null)
                            {
                                string itemTag = itemXml.Tag;
                                splitedTag = itemTag.Split('/');
                                XmlNode tmp = data;
                                foreach (string tag in splitedTag)
                                {
                                    if (itemTag == dataTag)
                                    {
                                        if (tag == splitedTag[splitedTag.Length - 1])
                                        {

                                            itemXml.SelectedOption = tmp.InnerText;
                                            dataSet = true;
                                            break;

                                        }
                                        else
                                        {
                                            if (data.HasChildNodes)
                                            {
                                                tmp = data.FirstChild;
                                                dataTag = tmp.Name;
                                            }

                                        }
                                    }
                                }
                            }
                            MultiValuedViewModel itemMulti = view as MultiValuedViewModel;
                            if (itemMulti != null)
                            {
                                string itemTag = itemMulti.Tag;
                                splitedTag = itemTag.Split('/');
                                XmlNode tmp = data;
                                foreach (string tag in splitedTag)
                                {
                                    if (itemTag == dataTag)
                                    {
                                        if (tag == splitedTag[splitedTag.Length - 1])
                                        {

                                            if (multiStarted == false || currentMulti != tag)
                                            {
                                                currentMulti = tag;
                                                multiIndex = 0;
                                                multiStarted = true;
                                                if (itemMulti.HeaderClass == null)
                                                {
                                                    itemMulti.Header.Value = tmp.InnerText;
                                                    //item.Aded.Clear();
                                                }
                                                else
                                                {
                                                    fillTheClass(itemMulti.HeaderClass, tmp);
                                                    //item.AdedClasses.Clear();
                                                }
                                                break;
                                            }
                                            else
                                            {
                                                if (itemMulti.HeaderClass == null)
                                                {
                                                    int index = itemMulti.Aded.Count;
                                                    if (index == -0 || multiIndex >= index)
                                                        itemMulti.AddNew();
                                                    itemMulti.Aded[multiIndex].Value = tmp.InnerText;
                                                }

                                                else
                                                {
                                                    int index = itemMulti.AdedClasses.Count;
                                                    if (index == 0 || multiIndex >= index)
                                                        itemMulti.Add();
                                                    fillTheClass(itemMulti.AdedClasses[multiIndex], tmp);

                                                }
                                                multiIndex++;
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            if (data.HasChildNodes)
                                            {
                                                tmp = data.FirstChild;
                                                dataTag = tmp.Name;
                                            }

                                        }
                                    }
                                }
                            }
                            IntegerViewModel itemInteger = view as IntegerViewModel;
                            if (itemInteger != null)
                            {
                                string itemTag = itemInteger.Tag;
                                splitedTag = itemTag.Split('/');
                                XmlNode tmp = data;
                                foreach (string tag in splitedTag)
                                {
                                    if (itemTag == dataTag)
                                    {
                                        if (tag == splitedTag[splitedTag.Length - 1])
                                        {

                                            itemInteger.Value = tmp.InnerText;
                                            dataSet = true;
                                            break;

                                        }
                                        else
                                        {
                                            if (data.HasChildNodes)
                                            {
                                                tmp = data.FirstChild;
                                                dataTag = tmp.Name;
                                            }

                                        }
                                    }
                                }
                            }
                            BranchViewModel itemBranch = view as BranchViewModel;
                            if (itemBranch != null)
                            {
                                string itemTag = itemBranch.Tag;
                                splitedTag = itemTag.Split('/');
                                XmlNode tmp = data;
                                foreach (string tag in splitedTag)
                                {
                                    if (itemTag == dataTag)
                                    {
                                        if (tag == splitedTag[splitedTag.Length - 1])
                                        {
                                            if (tmp.HasChildNodes)
                                            {
                                                //item.Value = tmp.InnerText;
                                                if (itemBranch.Class.Resume == null)
                                                    fillTheClass(itemBranch.Class, tmp);
                                                else
                                                {
                                                    fillTheClass(itemBranch.Class.ResumeClass, tmp);
                                                    itemBranch.Class.setResumeInClassView();

                                                }
                                                dataSet = true;
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            if (data.HasChildNodes)
                                            {
                                                tmp = data.FirstChild;
                                                dataTag = tmp.Name;
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    }
                    /*
                    if(!dataSet && cvm.Branches!=null)
                    {
                        foreach (BranchViewModel item in cvm.Branches)
                        {
                            if (dataSet)
                                break;
                            string itemTag = item.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {
                                        if (tmp.HasChildNodes)
                                        {
                                            //item.Value = tmp.InnerText;
                                            if (item.Class.Resume == null)
                                                fillTheClass(item.Class, tmp);
                                            else
                                            {
                                                fillTheClass(item.Class.ResumeClass, tmp);
                                                item.Class.setResumeInClassView();

                                            }
                                            dataSet = true;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                        }
                    }
                    if (!dataSet && cvm.Multis != null)
                    {
                        foreach (MultiValuedViewModel item in cvm.Multis)
                        {
                            if (dataSet)
                                break;
                            string itemTag = item.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {

                                       if(multiStarted==false || currentMulti!=tag)
                                       {
                                           currentMulti = tag;
                                           multiIndex = 0;
                                           multiStarted = true;
                                           if(item.HeaderClass==null)
                                           {
                                               item.Header.Value = tmp.InnerText;
                                               //item.Aded.Clear();
                                           }
                                           else
                                           {
                                               fillTheClass(item.HeaderClass, tmp);
                                               //item.AdedClasses.Clear();
                                           }
                                           break;
                                       }
                                       else
                                       {
                                           if(item.HeaderClass==null)
                                           {
                                               int index = item.Aded.Count;
                                               if(index==-0 || multiIndex>=index)
                                                    item.AddNew();
                                               item.Aded[multiIndex].Value = tmp.InnerText;    
                                           }
                                                                            
                                           else
                                           {
                                               int index = item.AdedClasses.Count;
                                               if (index ==0 ||multiIndex >= index)
                                                   item.Add();
                                               fillTheClass(item.AdedClasses[multiIndex], tmp);

                                           }
                                           multiIndex++;
                                           break;
                                       }
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                        }
                    }
                    if (!dataSet && cvm.XMLNODE != null)
                    {
                        foreach (XMLNodeRefViewModel item in cvm.XMLNODE)
                        {
                            if (dataSet)
                                break;
                            string itemTag = item.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {
                                    
                                            item.SelectedOption = tmp.InnerText;
                                            dataSet = true;
                                            break;
                                   
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                        }
                    }
                    if (!dataSet && cvm.Items != null)
                    {
                        foreach (IntegerViewModel item in cvm.Items)
                        {
                            if (dataSet)
                                break;
                            string itemTag = item.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {
                                    
                                            item.Value = tmp.InnerText;
                                            dataSet = true;
                                            break;
                                    
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                        }
                    }*/
                
            }
        }
        public void Load()
        {
            //si hubiera mas de una aplicacion habria que tenerlo en cuenta
            //el include no se trata ya que al cargar ya se hace el include si hubiera un selector de app habria que tenerlo en cuenta
            //por lo que el definitions que se va a utilizar es el que ya esta cargado
            //CApp.cleanAll();
            //Utility.cleanAll();
            //CNode newRootNode = Utility.getRootNode("../config/RLSimion.xml");
            //string fileApp = "../config/RLSimion.xml";
            //XmlDocument appFile = new XmlDocument();
            //appFile.Load(fileApp);
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "XML-File | *.xml";
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else
                return;
            XmlDocument loadedDocument = new XmlDocument();
            loadedDocument.Load(fileDoc);
            XmlNode loadedDocumentRoot = loadedDocument.DocumentElement;
            LoadDocument(loadedDocumentRoot);
           
        }
        private void LoadDocument(XmlNode loadedDocumentRoot)
        {
            foreach (BranchViewModel branch in _branches)
            {
                //we have to find the correct data input for every branch we have in the form. If we do not have data we do nothing
                if (loadedDocumentRoot.HasChildNodes)
                {
                    foreach (XmlNode dataNode in loadedDocumentRoot.ChildNodes)
                    {
                        if (dataNode.Name == branch.Name)
                        {
                            fillTheClass(branch.Class, dataNode);
                        }
                    }
                }
            }

        }
        public void SetAsRoot()
        {
            if (!validate())
                return;
            if(this.Graf!=null)
            {
                DialogViewModel dvm = new DialogViewModel(null, "There is already a root node. Do you want to overwrite?", DialogViewModel.DialogType.YesNo);
                dynamic settings = new ExpandoObject();
                settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
                settings.ShowInTaskbar = true;
                settings.Title = "ERROR";

                new WindowManager().ShowDialog(dvm, null, settings);
                if (dvm.DialogResult == DialogViewModel.Result.CANCEL)
                    return;
            }
            
            XmlDocument document = new XmlDocument();
            XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
            document.AppendChild(newRoot);
            AppXML.Models.TreeNode rootNode = new Models.TreeNode("root", document, null);
            if (this._graf == null)
                _graf = new RightTreeViewModel(rootNode);
            NotifyOfPropertyChange(() => Graf);
            NotifyOfPropertyChange(() => AddChildVisible);
        }
        public string AddChildVisible { get { if (_graf!=null && _graf.Tree != null) return "Visible"; else return "Hidden"; } set { } }
        public string RemoveChildVisible { get { if (_graf!=null &&  _graf.Tree!=null && _graf.Tree[0].hasChildren()) return "Visible"; else return "Hidden"; } }
        public void AddChild()
        {
            if (!validate())
                return;
            DialogViewModel dvm = new DialogViewModel(null, "Which name do you want for the new node?", DialogViewModel.DialogType.Answer);
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = "Info";

            new WindowManager().ShowDialog(dvm, null, settings);
            string name = null;
            if (dvm.DialogResult == DialogViewModel.Result.OK)
                name = dvm.Text;
            if(name==null)
                name="Node";
            XmlDocument document = new XmlDocument();
            XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
            document.AppendChild(newRoot);
            Models.TreeNode node = new Models.TreeNode(name, document, Graf.SelectedTreeNode);
            Graf.AddNode(node);
            NotifyOfPropertyChange(() => RemoveChildVisible);
            //NotifyOfPropertyChange(() => Graf);
        }
        public void RemoveChild()
        {
            Graf.RemoveSelectedNode();
            NotifyOfPropertyChange(() => RemoveChildVisible);
        }
        public void SaveAll()
        {
            List<NodeAndName> myList = Graf.getAllLeafs();
            List<string> pahts = new List<string>();
            string folderName = "../experiments/" +"experiment" +DateTime.Now.ToString("yyyyMMddHHmmssffff");
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), folderName);
            Directory.CreateDirectory(CombinedPath);
            foreach(NodeAndName item in myList)
            {
                string path = CombinedPath + "/" + item.name + ".xml";
                item.doc.Save(path);
                pahts.Add(path);
            }
            runExperimentas(pahts);
        }
        private void runExperimentas(List<string> myList)
        {
            int max = Environment.ProcessorCount;
            string[] tmp = (myList[0].Split('/'));
            string pipeName = @"preuba";
            Process myProces = new Process();
            myProces.StartInfo.FileName = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../debug/RLSimion");
            myProces.StartInfo.Arguments = myList[0] + ","+pipeName;
            myProces.Start();
            
            Data.NamedPipeServer pipe = new NamedPipeServer(pipeName, 0);
            pipe.Start();
            
            Console.Write("pipe");
            //pipe.StopServer();

            //ffmpeg.StartInfo.FileName = "../cmd.exe";
            //ffmpeg.StartInfo.Arguments = "/k " + ffmpegPath + " " + ffmpegParams
            //ffmpeg.Start();
        }
        public void LoadSelectedNode()
        {
            if (Graf.SelectedTreeNode == null || Graf.SelectedTreeNode.Doc==null)
                return;
            XmlDocument doc = Graf.SelectedTreeNode.Doc;
            XmlNode loadedDocumentRoot = doc.DocumentElement;
            LoadDocument(loadedDocumentRoot);
            //LoadDocument(doc.DocumentElement);
            //NotifyOfPropertyChange(() => Branches);
        }
       
    }
}
