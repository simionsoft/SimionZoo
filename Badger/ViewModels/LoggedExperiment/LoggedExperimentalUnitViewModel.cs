using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Simion;
using Badger.Data;

namespace Badger.ViewModels
{
    public class LoggedExperimentalUnitViewModel: PropertyChangedBase
    {
        private string m_logFilePath = "";
        public string logFilePath { get { return m_logFilePath; } set { m_logFilePath = value; } }

        private string m_name;
        public string name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }

        private XmlDocument m_logDescriptor;
        public XmlDocument logDescriptor
        {
            get { return m_logDescriptor; }
            set { m_logDescriptor = value; }
        }

        private Dictionary<string, string> forkTags = new Dictionary<string, string>();
        public void addForkTag(string forkName, string value)
        {
            forkTags.Add(forkName, value);
        }
        public List<string> getForks()
        {
            List<string> forks = new List<string>();
            foreach (string forkname in forkTags.Keys)
                forks.Add(forkname);
            return forks;
        }
        public string getForkValue(string forkName)
        {
            if (forkTags.ContainsKey(forkName)) return forkTags[forkName];
            return "";
        }

        private List<string> m_variablesInLog = new List<string>();

        public LoggedExperimentalUnitViewModel(XmlNode configNode)
        {
            string experimentFilePath = configNode.Attributes[XMLConfig.pathAttribute].Value;
            string logDescFile = SimionFileData.getLogDescriptorsFilePath(experimentFilePath);
            string logFile = SimionFileData.getLogFilePath(experimentFilePath);

            m_name = name;
            m_logDescriptor = new XmlDocument();
            //logDescriptor.Load(logDescriptorFilePath);
            //processDescriptor();

            m_logFilePath = logFilePath;
        }
    }
}
