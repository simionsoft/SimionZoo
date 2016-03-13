using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Models
{
    public class TreeNode
    {
        private XmlDocument _document;
        private List<TreeNode> _children;
        private string _text;
        private TreeNode _father;

        public string Text { get { return _text; } set { _text = value; } }
        public List<TreeNode> ChildNodes { get { return _children; } set { } }


        public void remove()
        {
            _father.removeElement(this);
        }

        public TreeNode(string text,XmlDocument document, TreeNode father)
        {
            _text = text;
            _document = document;
            _father = father;
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
        }
        public void clean()
        {
            _children.Clear();
        }
        public void removeElement(TreeNode child)
        {
            //first we must know if child has childNodes or not
            if(child.hasChildren())
            {
                //we need to add all children to the father of this node
                foreach(TreeNode node in child.ChildNodes)
                {
                    child._father.addChild(node);
                }
            }
            if (_children.Contains(child))
                _children.Remove(child);
        }

    }
}
