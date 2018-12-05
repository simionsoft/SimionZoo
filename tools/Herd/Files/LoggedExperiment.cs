
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
                        if (loadOptions.Selection == LoadOptions.ExpUnitSelection.All
                            || (LoggedExperimentalUnit.LogFileExists(child.Attributes[XMLTags.nameAttribute].Value,baseDirectory) 
                            == (loadOptions.Selection == LoadOptions.ExpUnitSelection.OnlyFinished)))
                        {
                            LoggedExperimentalUnit newExpUnit = new LoggedExperimentalUnit(child, baseDirectory, loadOptions);
                            if (loadOptions.LoadVariablesInLog)
                            {
                                //We load the list of variables from the log descriptor and add them to the global list
                                newExpUnit.Variables = Log.LoadLogDescriptor(newExpUnit.LogDescriptorFileName);
                                foreach (string variable in newExpUnit.Variables) AddVariable(variable);
                            }
                            ExperimentalUnits.Add(newExpUnit);
                        }
                        break;
                }
            }

            //update progress
            loadOptions.OnExpLoaded?.Invoke(this);
        }
    }
}
