/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using System.Xml;

using Badger.Data;

using Herd.Files;


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
                //content = definitionNode.Attributes[XMLTags.defaultAttribute].Value;
                textColor = XMLTags.colorInvalidValue;
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
