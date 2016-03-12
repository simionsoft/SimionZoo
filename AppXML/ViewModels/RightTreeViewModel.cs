using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Collections.ObjectModel;
using AppXML.Models;
using System.Xml;

namespace AppXML.ViewModels
{
    public class RightTreeViewModel:PropertyChangedBase
    {
        private ObservableCollection<TreeNode> _treeItems;
        private TreeNode _tree;

        private TreeNode selectedTreeNode = null;

        public void Change(object sender)
        {
            var x = sender as System.Windows.Controls.TreeView;
            selectedTreeNode = x.SelectedValue as TreeNode;
        }

        public RightTreeViewModel()
        {
            XmlDocument doc = new XmlDocument();
            doc.Load("../experiments/twoMulti.xml");
            _tree = new TreeNode("root", doc, null);
           // _tree.addChild(_tree);
            TreeNode x = new TreeNode("hijo", doc, _tree);
            _tree.addChild(x);
           // TreeNode x = new TreeNode("hijo", doc, _tree);
            _tree.addChild(x);
            //TreeNode x = new TreeNode("hijo", doc, _tree);
            _tree.addChild(x);
            _treeItems = new ObservableCollection<TreeNode>();
            _treeItems.Add(_tree);
        }
        public RightTreeViewModel(TreeNode tree)
        {
            _tree = tree;
        }

        public ObservableCollection<TreeNode> Tree { get { return _treeItems; } set { _treeItems = value; NotifyOfPropertyChange(() => Tree); } }
    }
}
