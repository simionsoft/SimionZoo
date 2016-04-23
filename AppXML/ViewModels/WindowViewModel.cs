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
        public ObservableCollection<ValidableAndNodeViewModel> Branch { get { return _branches[0].Class.AllItems; } set { } }
        public RightTreeViewModel Graf { get { return _graf; } 
            set 
            {
                _graf = value; 
                NotifyOfPropertyChange(() => Graf); 
                NotifyOfPropertyChange(()=> AddChildVisible);
                NotifyOfPropertyChange(()=> RemoveChildVisible);

            } 
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
                CApp.cleanAll();

                int index = _apps.IndexOf(value);
                if (index == -1)
                    return;
                selectedApp = value;
                CApp.IsInitializing = true;
                _rootnode = Utility.getRootNode(apps[index]);
                _branches = _rootnode.children;
                _doc = (this._rootnode as CApp).document;
                _graf = null;
                CApp.IsInitializing = false;
                NotifyOfPropertyChange(() => Branch);
                NotifyOfPropertyChange(() => Graf);
                NotifyOfPropertyChange(() => rootnode);
                NotifyOfPropertyChange(() => RemoveChildVisible);
                NotifyOfPropertyChange(() => AddChildVisible);

            } 
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
        public void ModifySelectedLeaf()
        {
            if (!validate())
                return;
            XmlDocument document = new XmlDocument();
            XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
            document.AppendChild(newRoot);
            if ( Utility.findDifferences(document, Graf.SelectedTreeNode.Doc)==null)
                return;
            Graf.SelectedTreeNode.Doc = document;
            Graf.SelectedTreeNode.updateDif();
            NotifyOfPropertyChange(() => Graf.Tree);
            Graf.LoadedAndModified = true;
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
            sfd.Filter = "Experiment | *.node";
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
        public List<string> SaveTree()
        {
            Models.TreeNode root = Graf.RootNode;
            List<NodeAndName>  leafs = Graf.getAllLeafs();
            if (root == null || leafs == null || leafs.Count<1)
                return null;
            List<string> result = new List<string>();
            string stamp = DateTime.Now.ToString("yyyyMMddHHmmssffff");
            XmlDocument treeDoc = new XmlDocument();
            XmlElement treeRootNode = treeDoc.CreateElement("ExperimentsTree");
            treeRootNode.SetAttribute("App", SelectedApp);
            XmlElement rootDef = treeDoc.CreateElement("Root");
            string rootFolder = "../experiments/experiment" + stamp + "/root";
            Directory.CreateDirectory(rootFolder);
            string rootPath = rootFolder + "/root.xml";
            root.Doc.Save(rootPath);
            rootDef.InnerText = rootPath;
            treeRootNode.AppendChild(rootDef);
            XmlElement leafFather = treeDoc.CreateElement("Experiments");
            List<string> names = new List<string>();
            foreach(NodeAndName item in leafs)
            {
                while (names.Contains(item.name))
                    item.name += "c";
                names.Add(item.name);
                string folderName = "../experiments/experiment"+stamp+"/" + item.name;
                Directory.CreateDirectory(folderName);
                string xmlPath = folderName +"/"+ item.name + ".xml";
                item.doc.Save(xmlPath);
                XmlElement leaf = treeDoc.CreateElement(item.name);
                leaf.InnerText = xmlPath;
                leafFather.AppendChild(leaf);
                result.Add(Path.GetFullPath(xmlPath));
            }
            treeRootNode.AppendChild(leafFather);
            treeDoc.AppendChild(treeRootNode);
            String fileName = "../experiments/experiment" + stamp + ".xml";
            treeDoc.Save(fileName);
            return result;

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
            cvm.setAsNull();
            bool dataSet = false;
            bool multiStarted = false;
            string currentMulti = "";
            int multiIndex = 0;
            bool res = false;
            if(cvm.ResumeClass!=null)
            {
                cvm = cvm.ResumeClass;
                res = true;
            }
            foreach (XmlNode data in dataNode)
            {
                dataSet = false;
                string dataTag = data.Name;
                string branchTag = null;
                string[] splitedTag = null;
                //once we have the tag we have to find out if the branch has this tag somewhere
                if (!dataSet&&cvm.Choice != null)
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
                                    if (res)
                                        cvm.validate(false);
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
                    if (dataSet)
                        break;
                }
                foreach (ValidableAndNodeViewModel view in cvm.AllItems)
                {
                    if (!dataSet)
                    {
                        MultiXmlNodeRefViewModel itemMultiXml = view as MultiXmlNodeRefViewModel;
                        if (!dataSet && itemMultiXml != null)
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
                                            dataSet = true;
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
                            if (dataSet)
                                break;
                        }
                            XMLNodeRefViewModel itemXml = view as XMLNodeRefViewModel;
                            if (!dataSet && itemXml != null)
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
                                if (dataSet)
                                    break;
                            }
                            MultiValuedViewModel itemMulti = view as MultiValuedViewModel;
                            if (!dataSet && itemMulti != null)
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
                                                    if(itemMulti.Aded!=null)
                                                        itemMulti.Aded.Clear();
                                                }
                                                else
                                                {
                                                    fillTheClass(itemMulti.HeaderClass, tmp);
                                                    if(itemMulti.AdedClasses!=null)
                                                        itemMulti.AdedClasses.Clear();
                                                }
                                                dataSet = true;
                                                break;
                                            }
                                            else
                                            {
                                                if (itemMulti.HeaderClass == null)
                                                {
                                                    //int index = itemMulti.Aded.Count;
                                                    //if (index == -0 || multiIndex >= index)
                                                        itemMulti.AddNew();
                                                    itemMulti.Aded[multiIndex].Value = tmp.InnerText;
                                                }

                                                else
                                                {
                                                    //int index = itemMulti.AdedClasses.Count;
                                                    //if (index == 0 || multiIndex >= index)
                                                        itemMulti.Add();
                                                    fillTheClass(itemMulti.AdedClasses[multiIndex], tmp);

                                                }
                                                dataSet = true;
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
                                if (dataSet)
                                    break;
                            }
                            IntegerViewModel itemInteger = view as IntegerViewModel;
                            if (!dataSet && itemInteger != null)
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
                                if (dataSet)
                                    break;
                            }
                            BranchViewModel itemBranch = view as BranchViewModel;
                            if (!dataSet && itemBranch != null)
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
                                                itemBranch.IsNull = false;
                                                //item.Value = tmp.InnerText;
                                                if (itemBranch.Class.ItemName == null)
                                                    fillTheClass(itemBranch.Class, tmp);
                                                else
                                                {
                                                    fillTheClass(itemBranch.Class.ResumeClass, tmp);
                                                    itemBranch.Class.validate(false); 
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
                            if (dataSet)
                                break;
                        }
                    }
                
            }
            if (res)
                cvm.validate(false);
        }
        public void Load()
        {
            
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Experiment | *.node";
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()),"experiments");
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else
                return;
            
            XmlDocument loadedDocument = new XmlDocument();
            loadedDocument.Load(fileDoc);
            XmlNode loadedDocumentRoot = loadedDocument.DocumentElement;
            if (!loadedDocumentRoot.Name.Equals(selectedApp))
            {
                SelectedApp = loadedDocumentRoot.Name;
                NotifyOfPropertyChange(() => SelectedApp);

            }
            LoadDocument(loadedDocumentRoot);
            Graf = null;
            
           
           
        }
       
        private void LoadDocument(XmlNode loadedDocumentRoot)
        {
            //we are goingo to set as null every viewModel that is optional. If they are optional are the document has a value for them the will be set in the process
            //setAsNull();
            _doc.RemoveAll();
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

        private void setAsNull()
        {
            foreach(BranchViewModel branch in _branches)
            {
                branch.setAsNull();
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
            //document.Save("copia.tree");
            AppXML.Models.TreeNode rootNode = new Models.TreeNode("root", document, null);
            _graf = new RightTreeViewModel(rootNode,this);
            NotifyOfPropertyChange(() => Graf);
            NotifyOfPropertyChange(() => AddChildVisible);
        }
        public string AddChildVisible { get { if (_graf!=null && _graf.Tree != null) return "Visible"; else return "Hidden"; } set { } }
        public string RemoveChildVisible { get { if (_graf!=null &&  _graf.Tree!=null && _graf.Tree[0].hasChildren()) return "Visible"; else return "Hidden"; } }
        public void AddChild()
        {
            if (!validate() || Graf.SelectedTreeNode==null)
                return;
            DialogViewModel dvm = new DialogViewModel(null, "Which name do you want for the new node?", DialogViewModel.DialogType.Answer);
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = "Info";
            WindowManager wm = new WindowManager();
            string name = null;
            do
            {
                wm.ShowDialog(dvm, null, settings);
                if (dvm.DialogResult == DialogViewModel.Result.OK)
                    name = dvm.Text;
                else
                    return;
               
                    
                dvm.Text = "The name "+name+" is not valid. ";
            } while (name==null||!Utility.checkName(name,Graf.SelectedTreeNode));
            
            XmlDocument document = new XmlDocument();
            XmlNode newRoot = document.ImportNode(_doc.DocumentElement, true);
            document.AppendChild(newRoot);
            Models.TreeNode node = new Models.TreeNode(name, document, Graf.SelectedTreeNode);
            Graf.AddNode(node);
            NotifyOfPropertyChange(() => Graf.Tree);
            NotifyOfPropertyChange(() => RemoveChildVisible);
           
        }
        public void RemoveChild()
        {
            Graf.RemoveSelectedNode();
            NotifyOfPropertyChange(() => Graf);
            NotifyOfPropertyChange(() => RemoveChildVisible);
        }
        public void SaveAll()
        {
            //List<NodeAndName> myList = Graf.getAllLeafs();
            List<string> paths = null;
            if (!Graf.Loaded)
                paths=SaveAllTheNodes(true);
            else if (Graf.LoadedAndModified)
                paths = SaveAllTheNodes(true);
            else
                paths = SaveAllTheNodes(false);
            if (paths!=null && paths.Count > 0)
                initExperimentas(paths);
         
        }
        private void executeLoad(string fileDoc)
        {
         
            
                XmlDocument treeDoc = new XmlDocument();
                treeDoc.Load(fileDoc);
                XmlElement fileRoot = treeDoc.DocumentElement;
                if (fileRoot.Name != "ExperimentsTree")
                    return;
                if (fileRoot.Attributes["App"] == null || !_apps.Contains(fileRoot.Attributes["App"].Value))
                    return;
                SelectedApp = fileRoot.Attributes["App"].Value;
                NotifyOfPropertyChange(() => SelectedApp);
                foreach (XmlElement element in fileRoot.ChildNodes)
                {
                    try
                    {


                        if (element.Name == "Root")
                        {
                            string path = element.Attributes["Path"].Value;
                            if (File.Exists(path))
                            {
                                XmlDocument rootDocument = new XmlDocument();
                                rootDocument.Load(path);
                                LoadDocument(rootDocument.DocumentElement);
                                this._graf = null;
                                SetAsRoot();
                            }
                        }
                        else if (element.Name == "Experiments" && element.HasChildNodes)
                        {
                            Models.TreeNode top = Graf.SelectedTreeNode;
                            foreach (XmlElement exp in element.ChildNodes)
                            {
                                string nodeName = exp.Name;
                                string path = exp.Attributes["Path"].Value;
                                if (File.Exists(path))
                                {
                                    XmlDocument leafDoc = new XmlDocument();
                                    leafDoc.Load(path);
                                    Models.TreeNode treeNode = new Models.TreeNode(nodeName, leafDoc, top);
                                    Graf.SelectedTreeNode = top;
                                    Graf.AddNode(treeNode);
                                    if (exp.HasChildNodes)
                                    {
                                        foreach (XmlElement child in exp.ChildNodes)
                                        {
                                            LoadChildren(child, treeNode);
                                        }
                                    }
                                }
                            }
                            NotifyOfPropertyChange(() => RemoveChildVisible);
                           
                        }
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.StackTrace);
                    }
                }
            
        }
        public void LoadTree()
        {
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Tree | *.tree";
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else
                return;
            Cursor.Current = Cursors.WaitCursor;
            System.Windows.Forms.Application.DoEvents();
           
            executeLoad(fileDoc);
            Cursor.Current = Cursors.Default;
            if (_graf != null)
            {
                Graf.Loaded = true;
                Graf.LoadedAndModified = false;
                Graf.LoadedXmlFile = fileDoc;
            }

        }

        private void LoadChildren(XmlElement child, Models.TreeNode father)
        {
            try
            {
                Graf.SelectedTreeNode = father;
                string name = child.Name;
                string path = child.Attributes["Path"].Value;
                if (File.Exists(path))
                {
                    XmlDocument leafDoc = new XmlDocument();
                    leafDoc.Load(path);
                    Models.TreeNode treeNode = new Models.TreeNode(name, leafDoc, father);
                    Graf.AddNode(treeNode);
                    if (child.HasChildNodes)
                    {
                        foreach (XmlElement node in child.ChildNodes)
                        {
                            LoadChildren(node, treeNode);
                        }
                    }
                }


            }catch(Exception e)
            {
                Console.WriteLine(e.StackTrace);
            }

        }
        
        private List<String> getPaths(List<NodeAndName> myList)
        {
            List<string> pahts = new List<string>();
            string folderName = "../experiments/" + "experiment" + DateTime.Now.ToString("yyyyMMddHHmmssffff");
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), folderName);
            Directory.CreateDirectory(CombinedPath);
            List<string> names = new List<string>();
            foreach (NodeAndName item in myList)
            {
                string path = CombinedPath + "/" + item.name;
                while (names.Contains(path.ToUpper()))
                {
                    path += 'c';
                }
                names.Add(path.ToUpper());
                path += ".xml";
                item.doc.Save(path);
                pahts.Add(Path.GetFullPath(path));
            }
            return pahts;

        }
        private void initExperimentas(List<string> myList)
        {
            ProcessManagerViewModel pwvm = new ProcessManagerViewModel(myList);
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = false;
            settings.Title = "Process Manager";
            ProcessesWindowViewModel pwvm2 = new ProcessesWindowViewModel(pwvm);
            new WindowManager().ShowDialog(pwvm2, null, settings);
            pwvm2.Manager.closeAll();
            
            
            
        }
        public void SaveAllTheNodes()
        {
            SaveAllTheNodes(true);
        }
        public List<string> SaveAllTheNodes(bool askAndSave)
        {
            string xmlFileName = null;
            if(askAndSave)
            {
                SaveFileDialog sfd = new SaveFileDialog();
                sfd.Filter = "Tree | *.tree";
                sfd.InitialDirectory = "../experiments";
                string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../experiments");
                if (!Directory.Exists(CombinedPath))
                    System.IO.Directory.CreateDirectory(CombinedPath);
                sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath);
              
                if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    xmlFileName = sfd.FileName;
                }
                else
                    return new List<string>();
            }
            else
            {
                xmlFileName = Graf.LoadedXmlFile;
                xmlFileName = xmlFileName.Split('.')[0];
                xmlFileName = Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), xmlFileName);
                return Utility.removeAndGetLeafs(xmlFileName);
            }
            xmlFileName = xmlFileName.Split('.')[0];
            xmlFileName = Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), xmlFileName);
            if(Directory.Exists(xmlFileName))
            {
                Directory.Delete(xmlFileName,true);
            }

            Models.TreeNode root = Graf.RootNode;
            List<NodeAndName> leafs = Graf.getAllLeafs();
            List<string> result = new List<string>();
           // string stamp = DateTime.Now.ToString("yyyyMMddHHmmssffff");
            XmlDocument treeDoc = new XmlDocument();
            XmlElement treeRootNode = treeDoc.CreateElement("ExperimentsTree");
            treeRootNode.SetAttribute("App", SelectedApp);
            XmlElement rootDef = treeDoc.CreateElement("Root");
            string rootFolder = xmlFileName + "/root";
            Directory.CreateDirectory(rootFolder);
            string rootPath = rootFolder + "/root.node";
            root.Doc.Save(rootPath);
            rootDef.SetAttribute("Path", rootPath);
            treeRootNode.AppendChild(rootDef);
            
            List<string> names = new List<string>();
            XmlElement leafFather = treeDoc.CreateElement("Experiments");
            if (root.ChildNodes != null)
            {
                foreach (Models.TreeNode child in root.ChildNodes)
                {
                    string name = child.Text;
                    while (names.Contains(name))
                        name += "c";
                    names.Add(name);
                    XmlDocument docume = child.Doc;
                    string folderPath = xmlFileName + "/" + name;
                    Directory.CreateDirectory(folderPath);
                    string filePath = folderPath + "/" + name + ".node";
                    docume.Save(filePath);
                    //crear carpeta para archivo xml y carpetas para sus hijos
                    //añadir el nodo al fichero xml
                    XmlElement docElement = treeDoc.CreateElement(name);
                    docElement.SetAttribute("Path", filePath);
                    //docElement.InnerText = filePath;
                    leafFather.AppendChild(docElement);
                    //si tiene hijos hay que recorrerlos 
                    if (child.hasChildren())
                    {
                        foreach (Models.TreeNode item in child.ChildNodes)
                            result.AddRange(ResolverChildNode(item, folderPath, docElement));
                    }
                    else
                        result.Add(filePath);
                }
            }
            else
                result.Add(rootPath);
            treeRootNode.AppendChild(leafFather);
            treeDoc.AppendChild(treeRootNode);
            treeDoc.Save(xmlFileName+ ".tree");
            return result;
        }
      
        public List<string> ResolverChildNode(Models.TreeNode node,string fatherPath, XmlElement father)
        {
            string name = node.Text;
            XmlDocument docume = node.Doc;
            List<string> names = new List<string>();
            while (names.Contains(name))
                name += "c";
            XmlDocument xmlDocument = father.OwnerDocument;
            //crear la carpeta que va a contener el xml y sus hijos si los tuviera
            string folderPath = fatherPath+"/"+name;
            Directory.CreateDirectory(folderPath);
            string filePath = folderPath + "/" + name + ".node";
            docume.Save(filePath);
            //crear el xmlElement y añadirlo al padre
            XmlElement element = xmlDocument.CreateElement(name);
            element.SetAttribute("Path", filePath);

            //element.InnerText = filePath;
            father.AppendChild(element);
            List<string> result = new List<string>();
            if (node.hasChildren())
            {
                foreach (Models.TreeNode child in node.ChildNodes)
                    result.AddRange(ResolverChildNode(child, folderPath, element));
            }
            else
                result.Add(filePath);
            return result;
                

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
