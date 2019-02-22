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

using System.IO;
using System.Xml;

using Herd.Files;

namespace Badger.ViewModels
{
    public class BranchConfigViewModel : NestedConfigNode
    {
        private string m_className = "";

        public string ClassName
        {
            get { return m_className; }
            set
            {
                m_className = value;
                NotifyOfPropertyChange(() => ClassName);
            }
        }

        private string m_window = "";

        private bool m_bIsOptional = false;
        public bool bIsOptional
        {
            get { return m_bIsOptional; }
            set { m_bIsOptional = value; NotifyOfPropertyChange(() => bIsOptional); }
        }

        private bool m_bIsUsed = false;
        public bool bIsUsed
        {
            get { return !bIsOptional || m_bIsUsed; }
            set { m_bIsUsed = value; NotifyOfPropertyChange(() => bIsUsed); }
        }

        public BranchConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent
            , XmlNode definitionNode, string parentXPath, XmlNode configNode = null, bool initChildren = true)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            m_className = definitionNode.Attributes[XMLTags.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLTags.windowAttribute) != null)
                m_window = definitionNode.Attributes[XMLTags.windowAttribute].Value;
            else m_window = "";
            if (definitionNode.Attributes.GetNamedItem(XMLTags.optionalAttribute) != null)
                bIsOptional = bool.Parse(definitionNode.Attributes[XMLTags.optionalAttribute].Value);
            else bIsOptional = false;

            if (configNode != null)
            {
                configNode = configNode[name];
                if (configNode != null && bIsOptional)
                    bIsUsed = true;
            }
            else if (bIsOptional)
                bIsUsed = true;

            if (initChildren)
                childrenInit(parentExperiment, parentExperiment.GetClassDefinition(m_className), m_xPath, configNode);
        }

        public override bool Validate()
        {
            if (bIsOptional && !bIsUsed) return true; // don't need to validate children if its optional and not used

            foreach (ConfigNodeViewModel child in children)
                if (!child.Validate())
                {
                    //Console.WriteLine(child.name);
                    return false;
                }
            return true;
        }

        public override void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            if (bIsUsed)
                base.outputXML(writer, mode, leftSpace);
        }
        public override ConfigNodeViewModel clone()
        {
            BranchConfigViewModel newBranch = new BranchConfigViewModel(m_parentExperiment, parent,
                nodeDefinition, parent.xPath, null, false);
            foreach (ConfigNodeViewModel child in children)
            {
                ConfigNodeViewModel clonedChild = child.clone();
                clonedChild.parent = newBranch;
                newBranch.children.Add(clonedChild);
            }
            return newBranch;
        }
    }
}
