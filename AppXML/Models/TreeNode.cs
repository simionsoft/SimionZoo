using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Models
{
    public class TreeNode:Caliburn.Micro.PropertyChangedBase
    {
        private XmlDocument _document;
        private List<TreeNode> _children;
        private string _text;
        public TreeNode _father;
        private string _dif;
        public string Difference { get { return _dif; } set { _dif = value; NotifyOfPropertyChange(() => Difference); } }

        public string Text { get { return _text; } set { _text = value; } }
        public List<TreeNode> ChildNodes { get { return _children; } set { _children = value; NotifyOfPropertyChange(() => ChildNodes); } }
        public XmlDocument Doc { get { return _document; } set { _document = value; } }

        public void remove()
        {
            _father.removeElement(this);
        }

        public TreeNode(string text,XmlDocument document, TreeNode father)
        {
            _text = text;
            _document = document;
            _father = father;
            if(father!=null)
                _dif = Data.Utility.findDifferences(_document,father.Doc);
        }

        public bool hasChildren()
        {
            if (_children == null || _children.Count < 1)
                return false;
            return true;
        }
        public void addChild(TreeNode child)
        {
            if (_children == null)
                _children = new List<TreeNode>();
            _children.Add(child);
            NotifyOfPropertyChange(() => ChildNodes);
        }
        public void clean()
        {
            _children.Clear();
        }
        public void updateDif()
        {
            Difference = Data.Utility.findDifferences(_document, _father.Doc);
        }
        public void removeElement(TreeNode child)
        {
            //first we must know if child has childNodes or not
            if(child.hasChildren())
            {
                //we need to add all children to the father of this node
                foreach(TreeNode node in child.ChildNodes)
                {
                    this.addChild(node);
                    node._father = this;
                    node.updateDif();
                }
            }
            _children.Remove(child);
            NotifyOfPropertyChange(() => ChildNodes);
            
        }

    }
}
