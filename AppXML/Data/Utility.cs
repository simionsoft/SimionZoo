using AppXML.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Data
{
    public class Utility
    {
        //used to avoid readings of worl-denitions xml
        private static Dictionary<string, List<string>> xmlDic = new Dictionary<string, List<string>>();
        
        public static Boolean validate(string value, validTypes type)
        {
            Boolean result = false;
            if (value == "0")
                return true;
            Regex regex;
            switch (type)
            {
                case validTypes.DecimalValue:
                    regex = new Regex(@"^(-|)((0\.\d+)|[1-9]\d*(\.\d+))$");
                    if (regex.IsMatch(value))
                    {
                        return true;
                    }
                    return validate(value, validTypes.IntergerValue);
                case validTypes.IntergerValue:
                    regex = new Regex(@"^(-|)[1-9][0-9]*$");
                    return regex.IsMatch(value);                    
            }
            return result;
        }
        public static List<string> getEnumItems(XmlNode father)
        {
            List<string> result = null;
            if (father != null && father.HasChildNodes)
            {
                result = new List<string>();
                foreach (XmlNode child in father.ChildNodes)
                {
                    result.Add(child.InnerText);
                }
            }
            return result;
        }
        public static Dictionary<string,XmlNode> getDefinitions(string filePath)
        {

            Dictionary<string, XmlNode> result = new Dictionary<string, XmlNode>();
            XmlDocument doc = new XmlDocument();
            doc.Load(filePath);
            if (doc.HasChildNodes)
            {
                XmlNode root = doc.ChildNodes[1];
                if (root.HasChildNodes)
                {

                    foreach (XmlNode node in root.ChildNodes)
                    {
                        if (node.Name.Equals("CLASS"))
                        {
                            string key = node.Attributes["Name"].Value;
                            result.Add(key, node);
                        }
                        else if(node.Name.Equals("ENUMERATED-TYPES"))
                        {
                            foreach(XmlNode child in node.ChildNodes)
                            {
                                result.Add(child.Attributes["Name"].Value, child);
                            }
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
            {
                CNode cn = CNode.getInstance(node);
                bool isOptional = false;
                if (node.Attributes["Optional"] != null)
                    isOptional = Convert.ToBoolean(node.Attributes["Optional"].Value);
                string comment = null;
                if (node.Attributes["Comment"] != null)
                    comment = node.Attributes["Comment"].Value;
                if(cn is CBranch)
                    rootnode.AddBranch(new ViewModels.BranchViewModel(node.Attributes["Name"].Value, node.Attributes["Class"].Value,comment,isOptional,(rootnode as CApp).document));
            }
            return rootnode;
        }
        public static List<string> getComboFromXML(string file, string action)
        {
            string xmlFile = CNode.XML[file];
            string key = xmlFile + action;
            if(xmlDic.ContainsKey(key))
                return xmlDic[xmlFile+action];
            else
            {
                List<string> result = new List<string>();
                XmlDocument doc = new XmlDocument();
                doc.Load(xmlFile);
                foreach(XmlNode child in doc.ChildNodes)
                {
                    if(child.Name =="World-Definition")
                    {
                        foreach(XmlNode type in child.ChildNodes)
                        {
                            if(type.Name == action)
                            {
                                foreach(XmlNode variable in type.ChildNodes)
                                {
                                    result.Add(variable.ChildNodes[0].InnerText);
                                }
                            }
                        }
                    }
                }
                
                xmlDic.Add(key,result);
                return result;
            }
        }
    }
}
