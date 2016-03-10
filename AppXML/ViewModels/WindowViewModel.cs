using AppXML.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AppXML.Data;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.Xml;
using System.Windows;
using Caliburn.Micro;
using System.ComponentModel.Composition;
using System.Windows.Forms;
using System.IO;
using System.Dynamic;
namespace AppXML.ViewModels
{
    /*[Export(typeof(WindowViewModel))]*/
    public class WindowViewModel : PropertyChangedBase
    {
        private CNode _rootnode;
        private ObservableCollection<BranchViewModel> _branches;
        private XmlDocument _doc;



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

        public void Validate()
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


                    return;
                }
                else
                {
                    rootNode.AppendChild(branch.getXmlNode());
                }
            }
            _doc.AppendChild(rootNode);
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
        public void fillTheClass(ClassViewModel cvm, XmlNode dataNode)
        {
            //here we have to set the new data such as: selected choice element, default values for every *.value, etc.
           
            foreach(XmlNode data in dataNode)
            {
                string dataTag = data.Name;
                string branchTag = null;
                string[] splitedTag = null;
                //once we have the tag we have to find out if the branch has this tag somewhere
                if(cvm.Choice!=null)
                {
                    branchTag = cvm.Choice.Tag;
                    splitedTag = branchTag.Split('/');
                    XmlNode tmp = data;
                    foreach(string tag in splitedTag)
                    {
                        if(tag==dataTag)
                        {
                            if(tag==splitedTag[splitedTag.Length-1])
                            {
                                if(tmp.HasChildNodes)
                                {
                                    XmlNode choiceElementData = tmp.FirstChild;
                                    string name = choiceElementData.Name;
                                    cvm.Choice.setSelectedItem(name);
                                    fillTheClass(cvm.Choice.Class, choiceElementData);
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
            foreach(BranchViewModel branch in _branches)
            {
                //we have to find the correct data input for every branch we have in the form. If we do not have data we do nothing
                if(loadedDocumentRoot.HasChildNodes)
                {
                     foreach(XmlNode dataNode in loadedDocumentRoot.ChildNodes)
                     {
                         if(dataNode.Name==branch.Name)
                         {
                             fillTheClass(branch.Class, dataNode);
                         }
                     }
                }
            }

        }
    }
}
