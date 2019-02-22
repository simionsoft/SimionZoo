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
using System.Collections.ObjectModel;
using System.IO;

using Herd.Files;


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
                if (wireConfigNode != null && wireConfigNode.Name == XMLTags.WireTag)
                {
                    content = wireConfigNode.InnerText;

                    if (wireConfigNode.Attributes[XMLTags.minValueAttribute] != null
                        && wireConfigNode.Attributes[XMLTags.maxValueAttribute] != null)
                    {
                        double min = double.Parse(wireConfigNode.Attributes[XMLTags.minValueAttribute].Value);
                        double max = double.Parse(wireConfigNode.Attributes[XMLTags.maxValueAttribute].Value);
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
                        writer.Write(leftSpace + "<" + name + "><" + XMLTags.WireTag + ">"
                            + content + "</" + XMLTags.WireTag + "></" + name + ">\n");
                    else
                        writer.WriteLine(leftSpace + "<" + name + "><" + XMLTags.WireTag + " " + XMLTags.minValueAttribute + "=\"" + wire.Minimum
                            + "\" " + XMLTags.maxValueAttribute + "=\"" + wire.Maximum + "\">" + content + "</" + XMLTags.WireTag + "></" + name + ">");
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
