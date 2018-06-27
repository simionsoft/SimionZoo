using Badger.Simion;
using System.Xml;
using System.IO;
using System.Windows.Forms;
using Badger.Data;

namespace Badger.ViewModels
{
    class NeuralNetworkProblemDescriptionConfigViewModel : ConfigNodeViewModel
    {
        public NeuralNetworkProblemDescriptionConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            if (configNode == null || configNode[name] == null)
            {
                //default init

                //Using default attribute makes no sense here I think
                //content = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
                textColor = XMLConfig.colorInvalidValue;
            }
            else
            {
                //init from config file
                if (configNode[name].InnerXml != "")
                {
                    var mwvm = new NeuralNetwork.Windows.MainWindowViewModel();
                    mwvm.Import(configNode[name].InnerXml);
                    m_problemViewModel = mwvm.Problem;
                    content = configNode[name].InnerXml;
                }
            }
        }
        
        private NeuralNetwork.ProblemViewModel m_problemViewModel = null;

        public override ConfigNodeViewModel clone()
        {
            NeuralNetworkProblemDescriptionConfigViewModel newInstance =
                new NeuralNetworkProblemDescriptionConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }


        public override bool Validate()
        {
            if (content == "" || content == null || m_problemViewModel==null)
                return false;

            foreach (var chain in m_problemViewModel.NetworkArchitecture.Chains)
                foreach (var link in chain.ChainLinks)
                    if (!link.IsInputCompatible)
                        return false;

            return true;
        }


        public void EditModel()
        {
            var mwvm = new NeuralNetwork.Windows.MainWindowViewModel();
            mwvm.Import(content);

            CaliburnUtility.ShowPopupWindow(mwvm, "Neural Network Editor");
            m_problemViewModel = mwvm.Problem;
            content = mwvm.ExportToString();
        }
    }
}
