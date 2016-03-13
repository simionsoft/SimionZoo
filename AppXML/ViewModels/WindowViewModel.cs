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
namespace AppXML.ViewModels
{
    /*[Export(typeof(WindowViewModel))]*/
    public class WindowViewModel : PropertyChangedBase
    {
        private CNode _rootnode;
        private ObservableCollection<BranchViewModel> _branches;
        private XmlDocument _doc;
        private RightTreeViewModel _graf;

        public RightTreeViewModel Graf { get { return _graf; } set { } }



        public WindowViewModel()
        {
            _rootnode = Utility.getRootNode("../config/RLSimion.xml");
            _branches = _rootnode.children;
            _doc = (this._rootnode as CApp).document;

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
                    rootNode.AppendChild(branch.getXmlNode());
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
                }
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
            List<XmlDocument> myList = Graf.getAllLeafs();
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
        private XmlDocument copyDocument(XmlDocument source)
        {
            XmlDocument doc = new XmlDocument();

            return doc;
        }
    }
}
