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

using Herd.Files;


namespace Badger.ViewModels
{
    public class MultiValuedItemConfigViewModel: NestedConfigNode
    {
        private string m_className = "";

        public MultiValuedItemConfigViewModel(ExperimentViewModel parentExperiment,MultiValuedConfigViewModel parent
            ,XmlNode definitionNode, string parentXPath, XmlNode configNode=null)
        {
            CommonInitialization(parentExperiment,parent,definitionNode,parentXPath);

            m_parent = parent;
            m_className = definitionNode.Attributes[XMLTags.classAttribute].Value;

            switch (m_className)
            {
                case XMLTags.integerNodeTag: children.Add(new IntegerValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break;
                case XMLTags.boolNodeTag: children.Add(new BoolValueConfigViewModel(parentExperiment, this, definitionNode, m_xPath, configNode)); break;
                case XMLTags.doubleNodeTag: children.Add(new DoubleValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLTags.stringNodeTag: children.Add(new StringValueConfigViewModel(parentExperiment, this, definitionNode, m_xPath, configNode));break ;
                case XMLTags.filePathNodeTag: children.Add(new FilePathValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLTags.dirPathNodeTag: children.Add(new DirPathValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLTags.stateVarRefTag: children.Add(new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.StateVar, parent, definitionNode, parentXPath, configNode)); break;
                case XMLTags.actionVarRefTag: children.Add(new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.ActionVar, parent, definitionNode, parentXPath, configNode)); break;
                default:
                    XmlNode classDefinition = parentExperiment.GetClassDefinition(m_className);
                    if (classDefinition != null)
                        childrenInit(parentExperiment, parentExperiment.GetClassDefinition(m_className), m_xPath, configNode);
                    break;
            }
            
        }
        public void remove()
        {
            MultiValuedConfigViewModel parent = m_parent as MultiValuedConfigViewModel;
            if (parent != null)
                parent.removeChild(this);
        }

        public override ConfigNodeViewModel clone()
        {
            MultiValuedItemConfigViewModel newItem = new MultiValuedItemConfigViewModel(m_parentExperiment
                , parent as MultiValuedConfigViewModel
                , nodeDefinition, parent.xPath);
            foreach (ConfigNodeViewModel child in children)
            {
                ConfigNodeViewModel clonedChild = child.clone();
                clonedChild.parent = newItem;
                newItem.children.Add(clonedChild);
            }
            return newItem;
        }

    }
}
