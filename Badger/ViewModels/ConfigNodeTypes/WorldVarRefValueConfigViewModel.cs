using System.Xml;
using System.Collections.ObjectModel;
using Badger.Simion;
using System.IO;
using System.Collections.Generic;

namespace Badger.ViewModels
{
    class WorldVarRefValueConfigViewModel: ConfigNodeViewModel
    {
        private ObservableCollection<string> m_variables= new ObservableCollection<string>();
        public ObservableCollection<string> Variables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => Variables); }
        }

        public string SelectedVariable
        {
            get { return content; }
            set {
                content = value;
                NotifyOfPropertyChange(() => SelectedVariable);
            }
        }
        
        private WorldVarType m_varType;

        public WorldVarRefValueConfigViewModel(ExperimentViewModel parentExperiment, WorldVarType varType, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            m_varType = varType;

            //if the list is already loaded, get it
            m_variables.Clear();
            foreach (string var in m_parentExperiment.GetWorldVarNameList(m_varType))
                m_variables.Add(var);
            NotifyOfPropertyChange(() => Variables);

            if (configNode != null)
            {
                configNode = configNode[name];
                //is the world variable wired?
                XmlNode wireConfigNode = configNode.FirstChild;
                if (wireConfigNode != null && wireConfigNode.Name == XMLConfig.WireTag)
                {
                    content = wireConfigNode.InnerText;

                    if (wireConfigNode.Attributes[XMLConfig.minValueAttribute] != null
                        && wireConfigNode.Attributes[XMLConfig.maxValueAttribute] != null)
                    {
                        double min = double.Parse(wireConfigNode.Attributes[XMLConfig.minValueAttribute].Value);
                        double max = double.Parse(wireConfigNode.Attributes[XMLConfig.maxValueAttribute].Value);
                        m_parentExperiment.AddWire(content, min, max);
                    }
                    else
                        m_parentExperiment.AddWire(content);
                }
                else
                    content = configNode.InnerText;
            }

            m_parentExperiment.RegisterWorldVarRef(Update);
            m_parentExperiment.RegisterDeferredLoadStep(Update);
        }

        public override ConfigNodeViewModel clone()
        {
            WorldVarRefValueConfigViewModel newInstance=
                new WorldVarRefValueConfigViewModel(m_parentExperiment, m_varType,m_parent, nodeDefinition, m_parent.xPath);
            m_parentExperiment.RegisterWorldVarRef(newInstance.Update);
            newInstance.m_varType = m_varType;
            newInstance.Variables = Variables;
            newInstance.SelectedVariable = SelectedVariable;
            return newInstance;
        }

        public void Update()
        {
            string oldSelectedVariable = SelectedVariable; //need to save it before it is invalidated on updating the list

            m_variables.Clear();
            foreach (string var in m_parentExperiment.GetWorldVarNameList(m_varType))
                m_variables.Add(var);

            NotifyOfPropertyChange(() => Variables);

            SelectedVariable = oldSelectedVariable;
        }

        public override bool Validate()
        {
            return Variables.Contains(content);
        }


        //We need to override this method. If the action/state is wired, the output should include the appropriate tag
        public override void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit || mode==SaveMode.AsProject)
            {
                WireViewModel wire = m_parentExperiment.GetWire(content);
                if (wire!=null)
                {
                    //wired
                    if (!wire.Limit)
                        writer.Write(leftSpace + "<" + name + "><" + XMLConfig.WireTag + ">"
                            + content + "</" + XMLConfig.WireTag + "></" + name + ">\n");
                    else
                        writer.WriteLine(leftSpace + "<" + name + "><" + XMLConfig.WireTag + " " + XMLConfig.minValueAttribute + "=\"" + wire.Minimum
                            + "\" " + XMLConfig.maxValueAttribute + "=\"" + wire.Maximum + "\">" + content + "</" + XMLConfig.WireTag + "></" + name + ">");
                }
                else
                {
                    //unwired
                    writer.WriteLine(leftSpace + "<" + name + ">" + content + "</" + name + ">");
                }
            }
        }
    }
}
