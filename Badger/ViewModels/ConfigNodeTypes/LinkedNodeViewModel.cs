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


        public LinkedNodeViewModel(ConfigNodeViewModel linkOriginNode)
        {
            Console.WriteLine("Linked with " + linkOriginNode.name);
        }

        //Constructor used from the experiment editor
        public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel originNode,
            ConfigNodeViewModel targetNode)
        {
            m_parent = originNode.parent;
            m_parentExperiment = parentExperiment;
            name = targetNode.name;
            comment = targetNode.comment;
            content = originNode.content;
            Origin = originNode;
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
            //m_parent = Origin.parent;
        }

        //constructor used in clone()
        public LinkedNodeViewModel() { }


        public override ConfigNodeViewModel clone()
        {
            LinkedNodeViewModel newInstance =
                new LinkedNodeViewModel(m_parentExperiment, m_parent, nodeDefinition);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
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
        public void UnlinkNode()
        {
        }
    }
}
