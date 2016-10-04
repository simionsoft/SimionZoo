using Badger.Data;
using Badger.ViewModels;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.Models
{
    public class CNode
    {
        public static Dictionary<string, XmlNode> definitions;
        public static Dictionary<string,string> XML=new Dictionary<string,string>();
        private  readonly ObservableCollection<BranchViewModel> _children = new ObservableCollection<BranchViewModel>();
        public ObservableCollection<BranchViewModel> children { get { return _children; } set { } }

        

        public static void cleanAll()
        {
            XML.Clear();
            definitions.Clear();
        }

        public void AddBranch(BranchViewModel branch)
        {
            _children.Add(branch);
        }
        public string name { get; set; }
        public XmlNode node { get; set; }
        

        public static CNode getInstance(XmlNode node)
        {
            if (node.Name == "xml")
                return null;
            if (node.Name == "INCLUDE")
            {
                CNode.definitions=Utility.getDefinitions(node.InnerText);
                return null;
            }
            if(node.Name=="EXE")
            {
                CApp.EXE = node.InnerText;
            }
            if(node.Name=="PRE")
            {
                CApp.addPre(node.InnerText);
            }
            if (node.Name == "APP")
                return new CApp(node, node.Attributes.GetNamedItem("Name").Value);
            if (node.Name == "BRANCH")
                return new CBranch(node, node.Attributes.GetNamedItem("Name").Value, "commment",node.Attributes.GetNamedItem("Class").Value);
            if (node.Name == "MULTI-VALUED")
                return new CMultiValued(node.Attributes["Name"].Value, node.Attributes["Class"].Value);
            if (node.Name.EndsWith("VALUE"))
                return new CIntegerValue(node, node.Attributes.GetNamedItem("Name").Value, node.InnerText);
            
            return null;
        }
    }
}
