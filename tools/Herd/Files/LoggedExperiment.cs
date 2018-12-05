
using System.Collections.Generic;
using System.Xml;
using Herd.Network;

namespace Herd.Files
{
    public class LoggedExperiment
    {
        public string Name;

        public List<LoggedExperimentalUnit> ExperimentalUnits = new List<LoggedExperimentalUnit>();
        public List<LoggedFork> Forks = new List<LoggedFork>();
        public List<AppVersion> AppVersions = new List<AppVersion>();

        List<string> m_variables = new List<string>();

        void AddVariable(string variable)
        {
            if (!m_variables.Contains(variable))
                m_variables.Add(variable);
        }

        public LoggedExperiment(XmlNode configNode, string baseDirectory, LoadOptions loadOptions)
        {
            XmlAttributeCollection attrs = configNode.Attributes;

            if (attrs != null)
            {
                if (attrs.GetNamedItem(XMLTags.nameAttribute) != null)
                    Name = attrs[XMLTags.nameAttribute].Value;
            }

            foreach (XmlNode child in configNode.ChildNodes)
            {
                switch (child.Name)
                {
                    case XMLTags.forkTag:
                        LoggedFork newFork = new LoggedFork(child);
                        Forks.Add(newFork);
                        break;

                    case XmlTags.Version:
                        AppVersion appVersion = new AppVersion(child);
                        AppVersions.Add(appVersion);
                        break;

                    case XMLTags.ExperimentalUnitNodeTag:
                        LoggedExperimentalUnit newExpUnit = new LoggedExperimentalUnit(child, baseDirectory, loadOptions);
                        if (loadOptions.LoadVariablesInLog)
                        {
                            //We load the list of variables from the log descriptor and add them to the global list
                            newExpUnit.Variables = Log.LoadLogDescriptor(newExpUnit.LogDescriptorFileName);
                            foreach (string variable in newExpUnit.Variables) AddVariable(variable);
                        }
                        if ((loadOptions.LoadUnfinishedExpUnits && loadOptions.LoadFinishedExpUnits) || !newExpUnit.LogFileExists())
                            ExperimentalUnits.Add(newExpUnit);
                        break;
                }
            }
            //Associate forks and experiment units
            //foreach (ExperimentalUnit expUnit in ExperimentalUnits)
            //{
            //    TraverseAction(false, (node) =>
            //    {
            //        if (node is LoggedForkValueViewModel forkValue)
            //        {
            //            foreach (string forkName in expUnit.forkValues.Keys)
            //            {
            //                if (forkName == forkValue.parent.Name && expUnit.forkValues[forkName] == forkValue.Value)
            //                {
            //                    forkValue.expUnits.Add(expUnit);
            //                }
            //            }
            //        }
            //    });
            //}
        }
    }
}
