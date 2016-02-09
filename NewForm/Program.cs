using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

namespace NewForm
{
    public static class Program
    {
        public static Dictionary<string, XmlNode> definitions = new Dictionary<string, XmlNode>();
        /// <summary>
        /// Punto de entrada principal para la aplicación.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            List<Branch> branches = loadXml();
            //Application.Run(new Form1());
        }
        private static List<Branch> loadXml()
        {
            XmlDocument doc = new XmlDocument();
            doc.Load("../config/RLSimion.xml");
            if(doc.HasChildNodes)
            {
                XmlNode root = doc.ChildNodes[1];
                if(root.HasChildNodes)
                {
                    List<Branch> branches = new List<Branch>();
                    foreach(XmlNode node in root.ChildNodes)
                    {
                        if(node.Name.Equals("INCLUDE"))
                        {
                            loadDefinitions(node.InnerText);
                        }
                        else if(node.Name.Equals("BRANCH"))
                        {
                            string name =node.Attributes["Name"].Value;
                            string type =node.Attributes["Class"].Value;
                            Branch branch = new Branch(name,type);
                            branches.Add(branch);
                        }
                    }
                    return branches;
                }
            }
            return null;
        }
        private static void loadDefinitions(string fileName)
        {
            XmlDocument doc = new XmlDocument();
            doc.Load("../config/defintions.xml");
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
    }
}
