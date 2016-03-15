using AppXML.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Models
{
    public class CApp:CNode
    {
        private static List<XMLNodeRefViewModel> viewsWithNodeRef = new List<XMLNodeRefViewModel>();

        public static void cleanAll()
        {
            viewsWithNodeRef.Clear();
        }

        public static void addView(XMLNodeRefViewModel view)
        {
            if (!viewsWithNodeRef.Contains(view))
                viewsWithNodeRef.Add(view);
        }
        public static void removeViews(List<XMLNodeRefViewModel> views)
        {
            foreach(XMLNodeRefViewModel view in views)
            {
                viewsWithNodeRef.Remove(view);
            }
        }
        public static void updateViews()
        {
            foreach (XMLNodeRefViewModel view in viewsWithNodeRef)
                view.update();
        }
        public CApp(XmlNode node, string n) 
        { 
            name = n;
            _document = new XmlDocument();
            //_root = _document.CreateElement(name);
            //_document.AppendChild(_root);

        }

        private XmlDocument _document;
        private XmlNode _root;
        public XmlDocument document { get { return _document; } private set { } }
        public XmlNode root { get { return _root; } private set { } }
    }
}


