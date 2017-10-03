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
            commonInit(parentExperiment, parent, definitionNode, parentXPath);

            if (configNode == null || configNode[name] == null)
            {
                //default init
                content = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
                textColor = XMLConfig.colorDefaultValue;
            }
            else
            {
                //init from config file
                content = configNode[name].InnerXml;
            }
        }

        private Badger.ViewModels.NeuralNetwork.ProblemViewModel m_problemViewModel = null;

        public override ConfigNodeViewModel clone()
        {
            NeuralNetworkProblemDescriptionConfigViewModel newInstance =
                new NeuralNetworkProblemDescriptionConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }


        public override bool validate()
        {
            if (m_problemViewModel == null || m_problemViewModel.NetworkArchitecture == null)
            {
                var mwvm = new NeuralNetwork.Windows.MainWindowViewModel();
                mwvm.Import(content);
                m_problemViewModel = mwvm.Problem;
            }

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
