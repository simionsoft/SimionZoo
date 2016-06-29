using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Dynamic;
using System.Windows;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class ExperimentViewModel:Caliburn.Micro.PropertyChangedBase
    {
        private string m_filePath;
        public string filePath { get { return m_filePath; } set { m_filePath = value; } }

        private static string RemoveSpecialCharacters(string str)
        {
            StringBuilder sb = new StringBuilder();
            foreach (char c in str)
            {
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '-' || c == '_')
                {
                    sb.Append(c);
                }
            }
            return sb.ToString();
        }

        //private string m_tags;
        //public string tags { get { return m_tags; } set { m_tags = value; } }

        private string m_name;
        public string name { get { return m_name; }
            set { m_name = RemoveSpecialCharacters(value); NotifyOfPropertyChange(() => name); }
        }

        public string pipeName { get { return m_name; } }


        private XmlDocument m_experimentXML;
        public XmlDocument experimentXML{get{return m_experimentXML;} set{m_experimentXML=value;}}

        public ExperimentViewModel(string name, XmlDocument experimentXML)
        {
            m_name = name;
            m_experimentXML = experimentXML;
        }
    }
 
}