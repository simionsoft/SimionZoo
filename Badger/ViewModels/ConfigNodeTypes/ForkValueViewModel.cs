using System.IO;
using Badger.Simion;
using Caliburn.Micro;
using System.Xml;
using System;

namespace Badger.ViewModels
{
    public class ForkValueViewModel:ConfigNodeViewModel
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
        public ForkValueViewModel(ExperimentViewModel parentExperiment,XmlNode classDefinition
            , ConfigNodeViewModel parentNode, XmlNode configNode)
        {
            name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            //not sure how to do this in a more elegant way
            this.configNode = ConfigNodeViewModel.getInstance(parentExperiment, parentNode
                , classDefinition, parentNode.xPath, configNode);
            this.configNode.bCanBeForked = false; //already belongs to a fork
        }


        public override void outputXML(StreamWriter writer, SaveMode mode,string leftSpace)
        {
            //header
            if (mode == SaveMode.AsProject || mode==SaveMode.ForkHierarchy || mode==SaveMode.ForkValues)
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkValueTag + ">" + configNode.content
                    + "</" + XMLConfig.forkValueTag + ">");
            if (mode==SaveMode.AsExperiment)
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkValueTag + " " + XMLConfig.nameAttribute + "=\""
                    + name + "\">");
            //body: children
            configNode.outputXML(writer, mode,leftSpace + "  ");
            //footer
            if (mode==SaveMode.AsExperiment)
                writer.WriteLine(leftSpace + "</" + XMLConfig.forkValueTag + ">");
        }

        public override bool validate()
        {
            return configNode.validate();
        }

        public override ConfigNodeViewModel clone()
        {
            return new ForkValueViewModel(name,parent as ForkedNodeViewModel
                ,configNode.clone());
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
