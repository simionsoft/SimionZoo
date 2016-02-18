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
namespace AppXML.ViewModels
{
    public class MainWindowViewModel: PropertyChangedBase
    {
        private CNode _rootnode;
        private BranchViewModel _branch;
        public MainWindowViewModel()
        {
            _rootnode = Utility.getRootNode("../config/RLSimion.xml");
            _branch = new BranchViewModel(_rootnode.children[1]);
        }
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
        public BranchViewModel Branch
        {
            get
            {
                return _branch;
            }
            set
            {
                _branch = value;
            }
        }
        
        
       
        
       
    }
}
