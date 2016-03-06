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
namespace AppXML.ViewModels
{
    /*[Export(typeof(WindowViewModel))]*/
    public class WindowViewModel: PropertyChangedBase
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
            XmlNode rootNode = _doc.CreateElement(_rootnode.name);
            foreach(BranchViewModel branch in _branches)
            {
                if(!branch.validate())
                {
                    //mostrar error
                    //return;
                }
                else
                {
                    rootNode.AppendChild(branch.getXmlNode());
                }
            }
            
        }
       
        
       
    }
}
