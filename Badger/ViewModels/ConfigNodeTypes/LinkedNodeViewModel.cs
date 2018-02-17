using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Badger.Simion;

namespace Badger.ViewModels
{
    class LinkedNodeViewModel : ConfigNodeViewModel
    {
        private ConfigNodeViewModel m_origin;

        public ConfigNodeViewModel Origin
        {
            get { return m_origin; }
            set
            {
                m_origin = value;
                NotifyOfPropertyChange(() => Origin);
            }
        }

        private string m_originName;

        public string OriginName
        {
            get { return m_originName; }
            set
            {
                m_originName = value;
                NotifyOfPropertyChange(() => OriginName);
            }
        }

        private ConfigNodeViewModel m_linkedNode;

        public ConfigNodeViewModel LinkedNode
        {
            get { return m_linkedNode; }
            set
            {
                m_linkedNode = value;
                NotifyOfPropertyChange(() => LinkedNode);
            }
        }

        public LinkedNodeViewModel(ConfigNodeViewModel linkOriginNode)
        {
            Console.WriteLine("Linked with " + linkOriginNode.name);
        }

        //Constructor used from the experiment editor
        public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel originNode,
            ConfigNodeViewModel targetNode)
        {
            m_parent = targetNode.parent;
            m_parentExperiment = parentExperiment;
            m_origin = originNode;
            // We need the linked node to show exactly as the origin node
            LinkedNode = originNode.clone();
            // But we need to protects its identity, so lets put a few things back to normal
            LinkedNode.nodeDefinition = targetNode.nodeDefinition;
            LinkedNode.LinkedNodes = targetNode.LinkedNodes;
            LinkedNode.name = targetNode.name;
            LinkedNode.comment = targetNode.comment;

            nodeDefinition = targetNode.nodeDefinition;
            name = targetNode.name;
            comment = targetNode.comment;
            content = originNode.content;
        }

        /// <summary>
        /// Constructor called when loading an experiment config file
        /// </summary>
        /// <param name="parentExperiment"></param>
        /// <param name="parentNode"></param>
        /// <param name="classDefinition">Class of the node in app definitions</param>
        /// <param name="parentXPath"></param>
        /// <param name="configNode">Node in simion.exp file with the configuration for a node class</param>
        public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parentNode,
            XmlNode classDefinition, XmlNode configNode = null)
        {
            m_parentExperiment = parentExperiment;
            nodeDefinition = classDefinition;

            foreach (XmlNode configChildNode in configNode)
            {
                if (configChildNode.Name.Equals(XMLConfig.linkedNodeTag)
                    && configChildNode.Attributes[XMLConfig.nameAttribute].Value
                    .Equals(classDefinition.Attributes[XMLConfig.nameAttribute].Value))
                    OriginName = configChildNode.Attributes[XMLConfig.originNodeAttribute].Value;
            }

            name = nodeDefinition.Attributes[XMLConfig.nameAttribute].Value;
            LinkedNode = getInstance(parentExperiment, parentNode, classDefinition, "",
                configNode);
            //LinkedNode.nodeDefinition = classDefinition;
        }

        //constructor used in clone()
        public LinkedNodeViewModel() { }


        public override ConfigNodeViewModel clone()
        {
            return null;
        }


        public override void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            if (mode == SaveMode.AsProject || mode == SaveMode.AsExperiment)
            {
                writer.WriteLine(leftSpace + "<" + XMLConfig.linkedNodeTag + " "
                    + XMLConfig.nameAttribute + "=\"" + name.TrimEnd(' ') + "\" " + XMLConfig.originNodeAttribute
                    + "=\"" + m_origin.name + "\"/>");
            }
        }

        public override bool validate()
        {
            return true;
        }

        /// <summary>
        /// 
        /// </summary>
        //public void UnlinkNode()
//        {
  //          var node = m_parentExperiment.DepthFirstSearch(this);
      //      Console.WriteLine("Unlink this node!!");
    //    }
    }
}
