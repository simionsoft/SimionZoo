using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Collections.ObjectModel;
using Badger.Models;
using System.Xml;

namespace Badger.ViewModels
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
        private bool _loadedAndModified=false;
        public bool Loaded{get;set;}
        public bool LoadedAndModified { get { return _loadedAndModified; } set { _loadedAndModified = Loaded & value; } }

        ObservableCollection<TreeNode> _treeItems;
        private readonly WindowViewModel father;

        private TreeNode selectedTreeNode = null;
        public TreeNode SelectedTreeNode { get { return selectedTreeNode; } set { selectedTreeNode = value; } }
        private TreeNode rootNode;

        public TreeNode RootNode { get { return rootNode; } set { } }

        //public void Change(object sender)
        //{
        //    var x = sender as System.Windows.Controls.TreeView;
        //    if(selectedTreeNode!=x.SelectedValue as TreeNode)
        //    {
        //        selectedTreeNode = x.SelectedValue as TreeNode;
        //        father.LoadSelectedNode();
        //        if (selectedTreeNode!=null && !selectedTreeNode.hasChildren())
        //            father.IsSelectedNodeLeafBool = true;
        //        else
        //            father.IsSelectedNodeLeafBool = false;
        //    }
            
            
        //}
        public RightTreeViewModel(TreeNode tree, WindowViewModel father)
        {
            _treeItems = new ObservableCollection<TreeNode>();
            _treeItems.Add(tree);
            rootNode = tree;
            selectedTreeNode = tree;
            this.father = father;
            Loaded = false;
            _loadedAndModified = false;
            LoadedXmlFile = null;
        }
        public ObservableCollection<TreeNode> Tree { get { return _treeItems; } set { _treeItems = value; NotifyOfPropertyChange(() => Tree); } }
        public void AddNode(TreeNode newNode)
        {
            if (selectedTreeNode == null)
                return;
            selectedTreeNode.addChild(newNode);
            Tree = new ObservableCollection<TreeNode> { rootNode };
            _loadedAndModified = Loaded & true;
            NotifyOfPropertyChange(() => Tree);
        }
        public void RemoveSelectedNode()
        {
            if (selectedTreeNode == null||selectedTreeNode==rootNode)
                return;
            SelectedTreeNode.remove();
            Tree = new ObservableCollection<TreeNode> { rootNode };
            _loadedAndModified = Loaded & true;
            NotifyOfPropertyChange(() => Tree);
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

        public string LoadedXmlFile { get; set; }
    }
}
