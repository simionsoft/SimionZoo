using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class PlotEditorWindowViewModel : Screen
    {        

        private const string m_descriptorRootNodeName = "ExperimentLogDescriptor";
        private Dictionary<ExperimentViewModel, XmlDocument> m_logDescriptors = new Dictionary<ExperimentViewModel, XmlDocument>();

        private List<string> m_variableList = new List<string>();
        private List<string> m_episodeTypeList = new List<string>();

        private List<PlotTabViewModel> m_plots = new List<PlotTabViewModel>();
        public List<PlotTabViewModel> plots { get { return m_plots; } set { } }

        public PlotEditorWindowViewModel(List<ExperimentViewModel> experimentList)
        {
            //maybe this initialization should be run as a background task?
            foreach(ExperimentViewModel exp in experimentList)
            {
                string logDescriptorFilePath = exp.getLogDescriptorsFilePath();
                XmlDocument logDescriptor = new XmlDocument();
                logDescriptor.LoadXml(logDescriptorFilePath);

                m_logDescriptors.Add(exp, logDescriptor);
                processDescriptor(logDescriptor);
            }
            //we add the initial plot
            m_plots.Add(new PlotTabViewModel(this));
            NotifyOfPropertyChange(() => plots);
        }
        private void processDescriptor(XmlDocument descriptor)
        {
            XmlNode node = descriptor.FirstChild;
            if (node.Name==m_descriptorRootNodeName)
            {
                foreach(XmlNode child in node.ChildNodes)
                {
                    if (child.Name=="State-variable" || child.Name=="Action-variable" || child.Name=="Reward-variable"
                        || child.Name=="Stat-variable")
                    {
                        string varName = child.InnerText;
                        if (!m_variableList.Contains(varName)) m_variableList.Add(varName);
                    }
                }
            }
        }
        private bool descriptorIncludesVariable(XmlDocument descriptor, string variable)
        {
            //this method returns whether a log contains info about the given variable
            //it does not check the type of variable: state, action, reward, ...
            XmlNode node = descriptor.FirstChild;
            if (node != null && node.Name==m_descriptorRootNodeName)
            {
                foreach(XmlNode child in node.ChildNodes)
                {
                    if (child.InnerText == variable)
                        return true;
                }
            }
            return false;
        }
        public void getExperimentList(string variable, ref List<ExperimentViewModel> experimentList)
        {
            foreach(KeyValuePair<ExperimentViewModel,XmlDocument> descriptor in m_logDescriptors)
            {
                if (descriptorIncludesVariable(descriptor.Value, variable))
                    experimentList.Add(descriptor.Key);
            }
        }
        public int getVariableIndex(ExperimentViewModel experiment, string variable, ref uint numVariables)
        {
            XmlDocument descriptor= m_logDescriptors[experiment];
            if (descriptor == null) return -1;
            int index = 0;
            bool bFound = false;
            numVariables = 0;
            XmlNode rootNode = descriptor.FirstChild;
            if (rootNode == null) return -1;
            foreach(XmlNode child in rootNode.ChildNodes)
            {
                if (child.InnerText == variable) bFound = true;
                if (child.Name.Contains("-variable"))
                {
                    if (!bFound) ++index;
                    ++numVariables;
                }
            }
            return -1;
        }
    }
}
