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
using System.Collections.Generic;

using Herd.Files;

namespace Badger.ViewModels
{
    class EnumeratedValueConfigViewModel: ConfigNodeViewModel
    {
        private List<string> m_enumeratedNames;

        public List<string> enumeratedNames
        {
            get { return m_enumeratedNames; }
            set { m_enumeratedNames = value; NotifyOfPropertyChange(() => enumeratedNames); }
        }

        public string selectedEnumeratedName
        {
            get { return content; }
            set { content = value; }
        }

        string m_class;

        public EnumeratedValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            m_class = definitionNode.Attributes[XMLTags.classAttribute].Value;
            enumeratedNames = m_parentExperiment.GetEnumeratedType(m_class);

            if (configNode == null || configNode[name] == null)
            {
                content = m_default;
                textColor = XMLTags.colorDefaultValue;
            }
            else
            {
                //init from config file
                content = configNode[name].InnerText;
            }

        }

        public override ConfigNodeViewModel clone()
        {
            EnumeratedValueConfigViewModel newInstance =
                new EnumeratedValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);
            
            newInstance.m_class = m_class;
            newInstance.enumeratedNames = enumeratedNames;
            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }


        public override bool Validate()
        {
            List<string> enumeration = m_parentExperiment.GetEnumeratedType(m_class);
            return enumeration.Exists(id => (id==content));
        }
    }
}
