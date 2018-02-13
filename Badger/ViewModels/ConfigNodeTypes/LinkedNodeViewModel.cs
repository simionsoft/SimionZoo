using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.ViewModels
{
    class LinkedNodeViewModel : ConfigNodeViewModel
    {
        private string m_origin;

        public string Origin {
            get { return m_origin; }
            set {
                m_origin = value;
                NotifyOfPropertyChange(() => Origin);
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
            Origin = "Linked to " + originNode.name;
        }

        /// <summary>
        /// Constructor called when loading an experiment config file
        /// </summary>
        /// <param name="parentExperiment"></param>
        /// <param name="parentNode"></param>
        /// <param name="classDefinition"></param>
        /// <param name="parentXPath"></param>
        /// <param name="configNode"></param>
        public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parentNode,
            XmlNode classDefinition, string parentXPath, XmlNode configNode = null)
        {
            m_parentExperiment = parentExperiment;
            nodeDefinition = classDefinition;
            m_parent = parentNode;

            Console.WriteLine("Linked with " + nodeDefinition.Name);
        }

        //constructor used in clone()
        public LinkedNodeViewModel() { }


        public override ConfigNodeViewModel clone()
        {
            DoubleValueConfigViewModel newInstance =
                new DoubleValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
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
