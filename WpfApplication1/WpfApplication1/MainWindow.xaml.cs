using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.Xml;

namespace WpfApplication1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public class CNode
    {
        private readonly ObservableCollection<CNode> _children= new ObservableCollection<CNode>();
        public ObservableCollection<CNode> children { get { return _children; } }
        public string name { get; set; }
        public XmlNode node { get; set; }
        public static CNode getInstance(XmlNode node)
        {
            if (node.Name == "xml")
                return null;
            if (node.Name == "INCLUDE")
            {
                MainWindow.loadDefinitions(node.InnerText);
                return null;
            }
            if (node.Name == "APP")
                return new CApp(node,node.Attributes.GetNamedItem("Name").Value);
            if (node.Name == "BRANCH")
                return new CBranch(node,node.Attributes.GetNamedItem("Name").Value, "commment");
            if (node.Name == "INTEGER-VALUE")
                return new CIntegerValue(node,node.Attributes.GetNamedItem("Name").Value, node.InnerText);
            return null;
        }
    }
    public class CIntegerValue : CNode
    {
        public string m_value { get; set; }
        public CIntegerValue(XmlNode node,string n, string value) { name = n; m_value = value; }
    }
    public class CMultiValued : CNode
    {
        public CMultiValued(XmlNode node, string n) { name = n; }
    }
    public class CBranch : CNode
    {
        public List<CIntegerValue> elements { get; set; }
        public string m_comment { get; set; }
        public CBranch(XmlNode node, string n, string c) { name = n; m_comment = c; }
    }
    
    public class CApp : CNode
    {
        public CApp(XmlNode node, string n) { name = n; }
    }

    public partial class MainWindow : Window
    {
        static Dictionary<string, XmlNode> definitions = new Dictionary<string, XmlNode>();
        private XmlDocument xmldoc;
        private CNode rootnode;
        public MainWindow()
        {
            InitializeComponent();
            xmldoc= new XmlDocument();
            xmldoc.Load("../config/RLSimion.xml");

            XmlNode element = xmldoc.DocumentElement;

            rootnode = CNode.getInstance(element);

            foreach (XmlNode node in element.ChildNodes)
                rootnode.children.Add ( CNode.getInstance(node));


            Tree.DataContext= rootnode;

        }
        public static void loadDefinitions(String includePath)
        {
            XmlDocument doc = new XmlDocument();
            doc.Load(includePath);
            if(doc.HasChildNodes)
            {
                XmlNode root = doc.ChildNodes[0];
                if (root.HasChildNodes)
                {
                    
                    foreach (XmlNode node in root.ChildNodes)
                    {
                        if(node.Name.Equals("CLASS") || node.Name.Equals("ENUMERATED"))
                        {
                            string key = node.Attributes["Name"].Value;
                            definitions.Add(key, node);
                        }
                           
                    }
                    
                }
            }
            
        }
        private void createControl(CBranch branch)
        {

        }


    }


}
