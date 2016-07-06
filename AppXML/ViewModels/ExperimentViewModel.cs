using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Dynamic;
using System.Windows;
using Caliburn.Micro;
using System.IO;

namespace AppXML.ViewModels
{
    public class ExperimentViewModel:Caliburn.Micro.PropertyChangedBase
    {
        private bool m_bDataAvailable = false;
        public bool bDataAvailable { get { return m_bDataAvailable; }
            set{ m_bDataAvailable= value; NotifyOfPropertyChange(()=>bDataAvailable);} }

        private string m_filePath= "";
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

        private string m_name;
        public string name { get { return m_name; }
            set { m_name = RemoveSpecialCharacters(value); NotifyOfPropertyChange(() => name); }
        }


        private XmlDocument m_experimentXML;
        public XmlDocument experimentXML{get{return m_experimentXML;} set{m_experimentXML=value;}}

        public ExperimentViewModel(string name, XmlDocument experimentXML, string path="")
        {
            m_name = name;
            m_experimentXML = experimentXML;
            if (path != "")
                m_filePath = path;
        }
        public string getLogDescriptorsFilePath()
        {
            if (m_filePath != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                lastPos1 = m_filePath.LastIndexOf("/");
                lastPos2 = m_filePath.LastIndexOf("\\");

                lastPos = Math.Max(lastPos1, lastPos2);
                if (lastPos > 0)
                {
                    string directory = m_filePath.Substring(0, lastPos + 1);

                    return directory + "experiment-log.xml";
                }
            }
            return "";
        }
        public string getLogFilePath()
        {
            if (m_filePath != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                lastPos1 = m_filePath.LastIndexOf("/");
                lastPos2 = m_filePath.LastIndexOf("\\");

                lastPos = Math.Max(lastPos1, lastPos2);
                if (lastPos > 0)
                {
                    string directory = m_filePath.Substring(0, lastPos + 1);

                    return directory + "experiment-log.xml";
                }
            }
            return "";
        }
        public bool checkLogFilesAlreadyExist()
        {
            //for now, i'd rather hardcode the log filenames than go through the dll... doesn't seem worth the effort
            if (m_filePath != "")
            {
                if (File.Exists(getLogDescriptorsFilePath()) && File.Exists(getLogFilePath()))
                    return true;
            }
            return false;
        }
    }
 
}