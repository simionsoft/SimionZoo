using AppXML.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;

namespace AppXML.Data
{
    public static class Utility
    {
        //used to avoid readings of worl-denitions xml
        private static Dictionary<string, List<string>> xmlDic = new Dictionary<string, List<string>>();

        public static string findDifferences(XmlDocument childDocument,XmlDocument fatherDocument)
        {

            XDocument xdoc = XDocument.Parse(fatherDocument.OuterXml);
            //Dictionary<string,string> x = Utility.GetLeaves(xdoc);
            Dictionary<string, List<string>> father = Utility.getNodesWithMulti(xdoc);
            XDocument xdoc2 = XDocument.Parse(childDocument.OuterXml);
            //Dictionary<string, string> x2 = Utility.GetLeaves(xdoc2);
            Dictionary<string, List<string>> child = Utility.getNodesWithMulti(xdoc2);
            string lastMulti = " ";
            List<string> message = new List<string>();
            List<string> missingMessage = new List<string>();
            foreach (string key in child.Keys)
            {
                if (!father.ContainsKey(key))
                    missingMessage.Add(key + " has been added to this child");
                else
                {
                    List<string> list1 = child[key];
                    List<string> list2 = father[key];
                    if (list1.Count == list2.Count)
                    {
                        for (int i = 0; i < list2.Count; i++)
                        {
                            if (!list1[i].Equals(list2[i]))
                            {
                                message.Add(key);
                                int index = message.IndexOf(key);
                                if (index > 0)
                                {
                                    if (key.StartsWith(message[index - 1]))
                                    {
                                        message.RemoveAt(index - 1);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!key.StartsWith(lastMulti))
                        {
                            int dif = list1.Count - list2.Count;
                            if (dif > 0)
                            {
                                missingMessage.Add("This child has " + dif + " more " + key);
                            }
                            else
                            {
                                missingMessage.Add("This child has " + dif + " less " + key);
                                dif = -dif;
                            }
                            for (int i = 0; i < list1.Count - dif; i++)
                            {
                                if (list1[i].Equals(list2[i]))
                                    missingMessage.Add("The element number " + i + " has the same parameters");
                                else
                                    missingMessage.Add("The element number " + i + " has not the same parameters");
                            }
                            lastMulti = key;
                        }
                    }
                    father.Remove(key);
                }
            }
            foreach (string key in father.Keys)
            {
                missingMessage.Add("The child does not have " + key);
            }
            string result = "";
            foreach (string key in message)
                result += key + " is different\n";
            foreach (string sms in missingMessage)
                result += sms + "\n";
            if (result == "")
                return null;
            return result;

        }

        public static Dictionary<string, string> GetLeaves(XDocument doc)
        {
            var dict = doc
                .Descendants()
                //.Where(e => !e.HasElements)
                .ToDictionary(e => GetPath(e), e=> e.Value);
            return dict;
        }
        public static Dictionary<string, List<string>> getNodesWithMulti(XDocument doc)
        {
            var list = doc
                .Descendants()
                //.Where(e => e.HasElements)
                .ToArray();
            Dictionary<string, List<string>> result = new Dictionary<string, List<string>>();
             foreach(XElement e in list)
             {
                 string key = GetPath(e);
                 if (result.ContainsKey(key))
                 {
                     List<string> tmp = result[key];
                     tmp.Add(e.Value);
                     result[key]=tmp;
                 }
                 else
                 {
                     List<string> tmp = new List<string>();
                     tmp.Add(e.Value);
                     result.Add(key,tmp);
                 }
                     
             }
             return result;
        }
        private static string GetPath(XElement element)
        {
            var nodes = new List<string>();
            var node = element;
            while (node != null)
            {
                nodes.Add(node.Name.ToString());
                node = node.Parent;
            }

            return string.Join("/", Enumerable.Reverse(nodes));
        }


        public static string GetRelativePathTo(string absPath, string relTo)
        {
            string[] absDirs = absPath.Split('\\');
            string[] relDirs = relTo.Split('\\');
            // Get the shortest of the two paths 
            int len = absDirs.Length < relDirs.Length ? absDirs.Length : relDirs.Length;
            // Use to determine where in the loop we exited 
            int lastCommonRoot = -1; int index;
            // Find common root 
            for (index = 0; index < len; index++)
            {
                if (absDirs[index] == relDirs[index])
                    lastCommonRoot = index;
                else break;
            }
            // If we didn't find a common prefix then throw 
            if (lastCommonRoot == -1)
            {
                return relTo;
            }
            // Build up the relative path 
            StringBuilder relativePath = new StringBuilder();
            // Add on the .. 
            for (index = lastCommonRoot + 1; index < absDirs.Length; index++)
            {
                if (absDirs[index].Length > 0) relativePath.Append("..\\");
            }
            // Add on the folders 
            if(absDirs.Contains(relDirs[lastCommonRoot])&&(len-1==lastCommonRoot))
            {
                relativePath.Append("..\\");
            }
            for (index = lastCommonRoot + 1; index < relDirs.Length - 1; index++)
            {
                relativePath.Append(relDirs[index] + "\\");
            }
            relativePath.Append(relDirs[relDirs.Length - 1]);
            relativePath.Replace('\\','/');
            return relativePath.ToString();
        }
        public static Boolean validate(string value, validTypes type)
        {
            Boolean result = false;
            if (value == "0")
                return true;
            Regex regex;
            switch (type)
            {
                case validTypes.FilePathValue:
                    if(File.Exists(value))
                        return true;
                    else
                        return false;
                case validTypes.DirPathValue:
                    if (Directory.Exists(value))
                        return true;
                    else
                        return false;
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
                        else if(node.Name.Equals("ENUMERATION"))
                        {
                            //foreach(XmlNode child in node.ChildNodes)
                            {
                                string key = node.Attributes["Name"].Value;
                                result.Add(key, node);
                                //result.Add(child.Attributes["Name"].Value, child);
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
                string tag = null;
                if (node.Attributes["XMLTag"]!=null)
                    tag = node.Attributes["XMLTag"].Value;
                if(cn is CBranch)
                    rootnode.AddBranch(new ViewModels.BranchViewModel(node.Attributes["Name"].Value, node.Attributes["Class"].Value,comment,isOptional,(rootnode as CApp).document,tag));
            }
            return rootnode;
        }
        public static XmlNode resolveTag(string _tag, XmlDocument _doc)
        {
            string[] tags = _tag.Split('/');
            if(tags.Count()>0)
            {
                XmlNode result = _doc.CreateElement(tags[0]);
                if(tags.Count()>1)
                {
                    XmlNode father = result;
                    for(int i=1;i<tags.Count();i++)
                    {
                        XmlNode nodo = _doc.CreateElement(tags[i]);
                        father.AppendChild(nodo);
                        father = nodo;
                    }

                }
                return result;
            }

            return null;
           
            
        }
        public static XmlNode getLastChild(XmlNode result)
        {
            if (result.HasChildNodes)
                return getLastChild(result.ChildNodes[0]);
            else
                return result;
        }
        public static List<string> getComboFromXML(string file, string action)
        {
            string xmlFile = null;
            if (CNode.XML.ContainsKey(file))
                xmlFile = CNode.XML[file];
            else
                return new List<string>();
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
        public static void cleanAll()
        {
            xmlDic.Clear();
        }
    }
}
