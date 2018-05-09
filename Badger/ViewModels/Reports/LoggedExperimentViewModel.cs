using System;
using System.Collections.Generic;
using System.Xml;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class LoggedExperimentViewModel : SelectableTreeItem
    {
        public string Name { get; set; } = "";

        List<AppVersion> m_appVersions = new List<AppVersion>();
        public List<AppVersion> AppVersions { get { return m_appVersions; } }

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
        /// <param name="loadVariablesInLog">True if we are reading the experiment to make a report.</param>
        /// <param name="loadOnlyUnfinishedExperimentalUnits">True if we only want to load unfinished experimental units</param>
        /// <param name="updateFunction">Callback function to be called after a load progress event</param>
        public LoggedExperimentViewModel(XmlNode configNode, string baseDirectory, bool loadVariablesInLog
            , bool loadOnlyUnfinishedExperimentalUnits, SimionFileData.LoadUpdateFunction updateFunction = null)
        {
            XmlAttributeCollection attrs = configNode.Attributes;

            if (attrs != null)
            {
                if (attrs.GetNamedItem(XMLConfig.nameAttribute) != null)
                    Name = attrs[XMLConfig.nameAttribute].Value;
            }

            foreach (XmlNode child in configNode.ChildNodes)
            {
                switch (child.Name)
                {
                    case XMLConfig.forkTag:
                        LoggedForkViewModel newFork = new LoggedForkViewModel(child);
                        Forks.Add(newFork);
                        break;

                    //case XmlTags.Version:
                    //    //No need to load this information once the experiment has been run
                    //    //AppVersion appVersion = new AppVersion(child);
                    //    //m_appVersions.Add(appVersion);
                    //    break;

                    case XMLConfig.experimentalUnitNodeTag:
                        LoggedExperimentalUnitViewModel newExpUnit = new LoggedExperimentalUnitViewModel(child, baseDirectory, updateFunction);
                        if (loadVariablesInLog)
                        {
                            //We load the list of variables from the log descriptor and add them to the global list
                            newExpUnit.LoadLogDescriptor();
                            foreach (string variable in newExpUnit.VariablesInLog) AddVariable(variable);
                        }
                        if (!loadOnlyUnfinishedExperimentalUnits || !newExpUnit.PreviousLogExists())
                            ExperimentalUnits.Add(newExpUnit);
                        break;
                }
            }
            //Associate forks and experiment units
            foreach(LoggedExperimentalUnitViewModel expUnit in ExperimentalUnits)
            {
                TraverseAction(false, (node) => 
                {
                    if (node is LoggedForkValueViewModel forkValue)
                    {
                        foreach (string forkName in expUnit.forkValues.Keys)
                        {
                            if (forkName==forkValue.parent.Name && expUnit.forkValues[forkName]==forkValue.Value)
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
            if (!VariablesInLog.Contains(variableName))
                VariablesInLog.Add(variableName);
        }

        //Variables
        public List<string> VariablesInLog { get; set; }
            = new List<string>();
    }
}