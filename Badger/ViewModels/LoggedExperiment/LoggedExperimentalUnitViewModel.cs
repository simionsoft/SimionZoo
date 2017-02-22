using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Simion;
using Badger.Data;

namespace Badger.ViewModels
{
    public class LoggedExperimentalUnitViewModel: SelectableTreeItem
    {
        private string m_logFilePath = "";
        public string logFilePath { get { return m_logFilePath; } set { m_logFilePath = value; } }

        private string m_logDescriptorFilePath = "";
        public string logDescriptorFilePath { get { return m_logDescriptorFilePath; } set { m_logDescriptorFilePath = value; } }

        private string m_experimentFilePath = "";
        public string experimentFilePath { get { return m_experimentFilePath; } set { m_experimentFilePath = value; } }

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

        List<LoggedForkViewModel> m_forkValues= new List<LoggedForkViewModel>();
        List<LoggedForkViewModel> forkValues
        {
            get { return m_forkValues; }
            set { m_forkValues = value; }
        }

        private List<string> m_variablesInLog = new List<string>();

        public LoggedExperimentalUnitViewModel(XmlNode configNode)
        {
            if (configNode.Attributes.GetNamedItem(XMLConfig.nameAttribute)!=null)
                name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            if (configNode.Attributes.GetNamedItem(XMLConfig.pathAttribute) != null)
                experimentFilePath = configNode.Attributes[XMLConfig.pathAttribute].Value;
            logDescriptorFilePath = SimionFileData.getLogDescriptorsFilePath(experimentFilePath);
            logFilePath = SimionFileData.getLogFilePath(experimentFilePath);

            logDescriptor = new XmlDocument();
            logDescriptor.Load(logDescriptorFilePath);
            processDescriptor();

            //load the value of each fork used in this experimental unit
            foreach(XmlNode child in configNode.ChildNodes)
            {
                LoggedForkViewModel newFork = new LoggedForkViewModel(child);
                forkValues.Add(newFork);
            }
        }

        private void processDescriptor()
        {
            XmlNode node = m_logDescriptor.FirstChild;
            if (node.Name == XMLConfig.descriptorRootNodeName)
            {
                foreach (XmlNode child in node.ChildNodes)
                {
                    if (child.Name == XMLConfig.descriptorStateVarNodeName 
                        || child.Name == XMLConfig.descriptorActionVarNodeName
                        || child.Name == XMLConfig.descriptorRewardVarNodeName
                        || child.Name == XMLConfig.descriptorStatVarNodeName)
                    {
                        string varName = child.InnerText;
                        m_variablesInLog.Add(varName);
                    }
                }
            }
        }
        //public void addVariablesToList(ref ObservableCollection<LoggedVariableViewModel> variableList)
        //{
        //    if (variableList.Count == 0)
        //    {
        //        //if the list is empty, we add all the variables available in this log
        //        foreach (string var in m_variablesInLog)
        //        {
        //            variableList.Add(new LoggedVariableViewModel(var, m_parent));
        //        }
        //    }
        //    else
        //    {
        //        //else, we intersect both sets: remove all variables not present in this log
        //        foreach (LoggedVariableViewModel variable in variableList)
        //        {
        //            if (!m_variablesInLog.Contains(variable.name))
        //                variableList.Remove(variable);
        //        }
        //    }
        //}

    }
}
