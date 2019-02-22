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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.ViewModels
{
    class WireConnectionViewModel: ConfigNodeViewModel
    {
        private List<string> m_wireNames = new List<string>();
        public List<string> WireNames
        {
            get { return m_wireNames; }
            set { m_wireNames = value; NotifyOfPropertyChange(() => WireNames); }
        }

        public string SelectedWire
        {
            get { return content; }
            set
            {
                content = value;
                NotifyOfPropertyChange(() => SelectedWire);
            }
        }

        public WireConnectionViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            //the possible values taken by this world variable
            m_parentExperiment.GetWireNames(ref m_wireNames);
            NotifyOfPropertyChange(() => WireNames);

            if (configNode != null)
            {
                configNode = configNode[name];
                SelectedWire = configNode.InnerText;
                parentExperiment.AddWire(SelectedWire);
            }

            m_parentExperiment.RegisterDeferredLoadStep(Update);
            m_parentExperiment.RegisterWireConnection(Update);
        }

        public override ConfigNodeViewModel clone()
        {
            WireConnectionViewModel newInstance =
                new WireConnectionViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);
            m_parentExperiment.RegisterWireConnection(newInstance.Update);
            newInstance.WireNames = WireNames;
            newInstance.SelectedWire = SelectedWire;
            return newInstance;
        }

        public void Update()
        {
            string oldSelectedWire = SelectedWire; //need to save it before it gets invalidated on updating the list

            m_parentExperiment.GetWireNames(ref m_wireNames);
            NotifyOfPropertyChange(() => WireNames);

            SelectedWire = oldSelectedWire;
        }

        public override bool Validate()
        {
            return WireNames.Exists(id => (id == content));
        }
    }

}
