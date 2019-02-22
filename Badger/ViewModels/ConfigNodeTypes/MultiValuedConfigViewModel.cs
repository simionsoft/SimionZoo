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
    public class MultiValuedConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private bool m_bOptional;

        public MultiValuedConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent
            , XmlNode definitionNode, string parentXPath, XmlNode configNode= null, bool initChildren= true)
        {
            CommonInitialization(parentExperiment,parent, definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLTags.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLTags.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLTags.optionalAttribute].Value == "true";
            else m_bOptional = false;
            
            if (configNode!=null)
            {
                foreach(XmlNode configChild in configNode.ChildNodes)
                {
                    if (configChild.Name==name)
                        children.Add(new MultiValuedItemConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configChild));
                }
            }
            else //default initialization
            {
                if (initChildren && !m_bOptional)
                    children.Add(new MultiValuedItemConfigViewModel(parentExperiment, this,definitionNode, m_xPath));
            }
        }

        public override ConfigNodeViewModel clone()
        {
            MultiValuedConfigViewModel newNestedCopy = getInstance(m_parentExperiment, m_parent
                , nodeDefinition, m_parent.xPath) as MultiValuedConfigViewModel;
            foreach (ConfigNodeViewModel child in children)
            {
                newNestedCopy.children.Add(child.clone());
            }
            return newNestedCopy;
        }

        public void addChild()
        {
            children.Add(new MultiValuedItemConfigViewModel(m_parentExperiment,this, nodeDefinition, m_xPath));
            m_parentExperiment.DoDeferredLoadSteps();
        }
        public void removeChild(MultiValuedItemConfigViewModel child)
        {
            children.Remove(child);
        }

        public override bool Validate()
        {
            if (!m_bOptional && children.Count == 0) return false;
            return base.Validate();
        }

        //the base method is overriden because we don't want the MultiValuedConfigViewModel to add
        //any header or footer... those will be added by the MultiValuedItemConfigViewModel children,
        //only once per child
        public override void outputXML(StreamWriter writer,SaveMode mode,string leftSpace)
        {
            outputChildrenXML(writer,mode,leftSpace);
        }
    }
}
