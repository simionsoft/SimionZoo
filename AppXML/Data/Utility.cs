using AppXML.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Data
{
    public class Utility
    {
        
        public static Dictionary<string,XmlNode> getDefinitions(string filePath)
        {

            Dictionary<string, XmlNode> result = new Dictionary<string, XmlNode>();
            XmlDocument doc = new XmlDocument();
            doc.Load(filePath);
            if (doc.HasChildNodes)
            {
                XmlNode root = doc.ChildNodes[0];
                if (root.HasChildNodes)
                {

                    foreach (XmlNode node in root.ChildNodes)
                    {
                        if (node.Name.Equals("CLASS") || node.Name.Equals("ENUMERATED"))
                        {
                            string key = node.Attributes["Name"].Value;
                            result.Add(key, node);
                        }

                    }

                }
            }
            return result;
        }
        public static CNode getRootNode(string filePath)
        {
            XmlDocument xmldoc = new XmlDocument();
            xmldoc.Load(filePath);
            XmlNode element = xmldoc.DocumentElement;
            CNode rootnode = CNode.getInstance(element);
            foreach (XmlNode node in element.ChildNodes)
                rootnode.children.Add(CNode.getInstance(node));
            return rootnode;
        }
    }
}
