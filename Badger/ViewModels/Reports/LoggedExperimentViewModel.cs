using System;
using System.Collections.Generic;
using System.Xml;
using Badger.Simion;
using Badger.ViewModels.Reports;
using Caliburn.Micro;
using System.IO;

namespace Badger.ViewModels
{
    public class LoggedExperimentViewModel : SelectableTreeItem
    {
        public string Name { get; set; } = "";

        public string ExeFile { get; set; }

        public List<LoggedPrerequisiteViewModel> Prerequisites { get; set; } = new List<LoggedPrerequisiteViewModel>();

        public List<LoggedForkViewModel> Forks { get; set; } = new List<LoggedForkViewModel>();

        public List<LoggedExperimentalUnitViewModel> ExperimentalUnits { get; set; } = new List<LoggedExperimentalUnitViewModel>();

        public override void TraverseAction(bool doActionLocally, Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in Forks) fork.TraverseAction(true, action);
        }

        /// <summary>
        ///     Class constructor.
        /// </summary>
        /// <param name="configNode">The XML node from which the experiment's parameters hang.</param>
        /// <param name="baseDirectory">The directory of the parent batch file, if there is one.</param>
        /// <param name="isForReport">True if we are reading the experiment to make a report.</param>
        public LoggedExperimentViewModel(XmlNode configNode, string baseDirectory, bool isForReport)
        {
            XmlAttributeCollection attrs = configNode.Attributes;

            if (attrs != null)
            {
                if (attrs.GetNamedItem(XMLConfig.nameAttribute) != null)
                    Name = attrs[XMLConfig.nameAttribute].Value;

                if (attrs.GetNamedItem(XMLConfig.ExeFileNameAttr) != null)
                    ExeFile = attrs[XMLConfig.ExeFileNameAttr].Value;
            }

            foreach (XmlNode child in configNode.ChildNodes)
            {
                switch (child.Name)
                {
                    case XMLConfig.forkTag:
                        LoggedForkViewModel newFork = new LoggedForkViewModel(child);
                        Forks.Add(newFork);
                        break;

                    case XMLConfig.PrerequisiteTag:
                        LoggedPrerequisiteViewModel newPrerequisite = new LoggedPrerequisiteViewModel(child);
                        Prerequisites.Add(newPrerequisite);
                        break;

                    case XMLConfig.experimentalUnitNodeTag:
                        LoggedExperimentalUnitViewModel newExpUnit = new LoggedExperimentalUnitViewModel(child, baseDirectory);

                        if (isForReport)
                        {
                            newExpUnit.logDescriptor = new XmlDocument();
                            if (File.Exists(newExpUnit.logDescriptorFilePath))
                            {
                                newExpUnit.logDescriptor.Load(newExpUnit.logDescriptorFilePath);
                                List<string> variableNames = newExpUnit.processDescriptor();
                                foreach (var name in variableNames) AddVariable(name);
                            }
                        }

                        ExperimentalUnits.Add(newExpUnit);
                        break;
                }
            }
            //Associate forks and experiment units
            foreach(LoggedExperimentalUnitViewModel expUnit in ExperimentalUnits)
            {
                TraverseAction(false, (node) => 
                {
                    LoggedForkValueViewModel forkValue = node as LoggedForkValueViewModel;
                    if (forkValue!=null)
                    {
                        foreach (string forkName in expUnit.forkValues.Keys)
                        {
                            if (forkName==forkValue.parent.name && expUnit.forkValues[forkName]==forkValue.value)
                            {
                                forkValue.expUnits.Add(expUnit);
                            }
                        }
                    }
                });
            }
        }

        /// <summary>
        ///     Call after reading the log file descriptor of each experimetal unit
        /// </summary>
        /// <param name="variableName"></param>
        public void AddVariable(string variableName)
        {
            bool bVarExists = false;
            int i = 0, len = variables.Count;

            while (!bVarExists && i < len)
            {
                if (variables[i].name == variableName)
                    bVarExists = true;
                i++;
            }

            if (!bVarExists)
                variables.Add(new LoggedVariableViewModel(variableName));
        }

        //Variables
        private BindableCollection<LoggedVariableViewModel> m_variables
            = new BindableCollection<LoggedVariableViewModel>();
        public BindableCollection<LoggedVariableViewModel> variables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => variables); }
        }
    }
}