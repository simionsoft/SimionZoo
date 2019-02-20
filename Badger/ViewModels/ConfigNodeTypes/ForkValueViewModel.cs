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
