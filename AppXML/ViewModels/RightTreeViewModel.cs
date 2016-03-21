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
    public class NodeAndName
    {
        public string name;
        public XmlDocument doc;

        public NodeAndName(string n,XmlDocument t)
        {
            name = n;
            doc = t;
        }
    }
    public class RightTreeViewModel:PropertyChangedBase
    {
        BindableCollection<TreeNode> _treeItems;
        

        private TreeNode selectedTreeNode = null;
        public TreeNode SelectedTreeNode { get { return selectedTreeNode; } set { } }
        private TreeNode rootNode;

        public void Change(object sender)
        {
            var x = sender as System.Windows.Controls.TreeView;
            selectedTreeNode = x.SelectedValue as TreeNode;
            
        }
        public RightTreeViewModel(TreeNode tree)
        {
            _treeItems = new BindableCollection<TreeNode>();
            _treeItems.Add(tree);
            rootNode = tree;
            selectedTreeNode = tree;
        }
        public BindableCollection<TreeNode> Tree { get { return _treeItems; } set { _treeItems = value; NotifyOfPropertyChange(() => Tree); } }
        public void AddNode(TreeNode newNode)
        {
            if (selectedTreeNode == null)
                return;
            selectedTreeNode.addChild(newNode);
            _treeItems.Clear(); 
            _treeItems.Add(rootNode);
            NotifyOfPropertyChange(() => Tree);
        }
        public void RemoveSelectedNode()
        {
            if (selectedTreeNode == null||selectedTreeNode==rootNode)
                return;
            SelectedTreeNode.remove();
            _treeItems.Clear();
            _treeItems.Add(rootNode);
            NotifyOfPropertyChange(() => Tree);
            selectedTreeNode = rootNode;
        }

        internal List<NodeAndName> getAllLeafs()
        {
            List<NodeAndName> result = new List<NodeAndName>();
            findLeafs(ref result, rootNode);
            return result;
        }
        private void findLeafs(ref List<NodeAndName> result, TreeNode nodo)
        {
            if (nodo.hasChildren())
            {
                foreach (TreeNode child in nodo.ChildNodes)
                {
                    findLeafs(ref result, child);
                }
            }
            else
                result.Add(new NodeAndName(nodo.Text,nodo.Doc));
        }
    }
}
