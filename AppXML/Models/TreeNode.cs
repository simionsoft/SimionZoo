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
        public TreeNode _father;
        private string _dif;
        public string Difference { get { return _dif; } set { _dif = value; } }

        public string Text { get { return _text; } set { _text = value; } }
        public List<TreeNode> ChildNodes { get { return _children; } set { } }
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
            _dif = "None\n tu puta madre \n hola pepe";
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
                    node._father = child._father;
                }
            }
            if (_children.Contains(child))
                _children.Remove(child);
        }

    }
}
