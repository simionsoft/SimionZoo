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
    public class ForkValueViewModel : ConfigNodeViewModel
    {

        private ConfigNodeViewModel m_configNode;

        public ConfigNodeViewModel configNode
        {
            get { return m_configNode; }
            set { m_configNode = value; NotifyOfPropertyChange(() => configNode); }
        }

        //constructor called from the editor on forking a node
        public ForkValueViewModel(string valueName, ForkedNodeViewModel parent, ConfigNodeViewModel forkedNode)
        {
            name = valueName;
            m_parent = parent;
            configNode = forkedNode;
            //the config node now hangs from this fork value
            configNode.parent = this;
        }

        //constructor called when loading a fork from a .badger file
        public ForkValueViewModel(ExperimentViewModel parentExperiment, XmlNode classDefinition,
            ConfigNodeViewModel parentNode, XmlNode configNode)
        {
            name = configNode.Attributes[XMLTags.nameAttribute].Value;
            //not sure how to do this in a more elegant way
            this.configNode = getInstance(parentExperiment, parentNode,
                classDefinition, parentNode.xPath, configNode);
            this.configNode.bCanBeForked = false; //already belongs to a fork
        }


        public override void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            //header
            if (mode == SaveMode.AsProject)
                writer.WriteLine(leftSpace + "<" + XMLTags.forkValueTag + ">" + configNode.content + ">");

            if (mode == SaveMode.ForkHierarchy)
                writer.WriteLine(leftSpace + "<" + XMLTags.forkValueTag
                    + " " + XMLTags.valueAttribute + "=\"" + configNode.content + "\">");

            if (mode == SaveMode.AsExperiment)
                writer.WriteLine(leftSpace + "<" + XMLTags.forkValueTag + " "
                    + XMLTags.nameAttribute + "=\"" + name + "\">");
            //body: children
            configNode.outputXML(writer, mode, leftSpace + "\t");
            
            //footer
            if (mode == SaveMode.AsExperiment || mode == SaveMode.ForkHierarchy)
                writer.WriteLine(leftSpace + "</" + XMLTags.forkValueTag + ">");
        }

        public override bool Validate()
        {
            return configNode.Validate();
        }

        public override ConfigNodeViewModel clone()
        {
            return new ForkValueViewModel(name, parent as ForkedNodeViewModel, configNode.clone());
        }

        public override int getNumForkCombinations()
        {
            return configNode.getNumForkCombinations();
        }

        public override void setForkCombination(ref int id, ref string combinationName)
        {
            configNode.setForkCombination(ref id, ref combinationName);
        }
    }
}
