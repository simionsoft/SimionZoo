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
        ObservableCollection<TreeNode> _treeItems;
        

        private TreeNode selectedTreeNode = null;
        public TreeNode SelectedTreeNode { get { return selectedTreeNode; } set { } }
        private TreeNode rootNode;

        public void Change(object sender)
        {
            var x = sender as System.Windows.Controls.TreeView;
            selectedTreeNode = x.SelectedValue as TreeNode;
            /*if (selectedTreeNode == null)
                return;
            TreeNode xx = new TreeNode("prueba",null,selectedTreeNode);
            selectedTreeNode.addChild(xx);
            //Tree.Clear();
            Tree.Remove(selectedTreeNode);
            Tree.Add(selectedTreeNode);
            NotifyOfPropertyChange(() => Tree);*/
        }
        public RightTreeViewModel(TreeNode tree)
        {
            _treeItems = new ObservableCollection<TreeNode>();
            _treeItems.Add(tree);
            rootNode = tree;
            selectedTreeNode = tree;
        }
        public ObservableCollection<TreeNode> Tree { get { return _treeItems; } set { _treeItems = value; NotifyOfPropertyChange(() => Tree); } }
        public void AddNode(TreeNode newNode)
        {
            if (selectedTreeNode == null)
                return;
            TreeNode tmp = selectedTreeNode;
            selectedTreeNode.addChild(newNode);
            Tree = Tree;
            //_treeItems = new ObservableCollection<TreeNode>();
            //_treeItems.Add(rootNode);
            NotifyOfPropertyChange(() => Tree);
        }

    }
}
