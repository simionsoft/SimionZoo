using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace Herd.Files
{
    public class LoggedExperimentalUnit
    {
        public string Name;
        public string ExperimentFileName;
        public string LogDescriptorFileName;
        public string LogFileName;

        public Dictionary<string, string> ForkValues = new Dictionary<string, string>();
        public List<string> Variables { get; set; } //this should be read from the log descriptor

        public LoggedExperimentalUnit(XmlNode configNode, string baseDirectory, LoadOptions loadOptions)
        {
            //Experiment Name
            if (configNode.Attributes != null)
            {
                if (configNode.Attributes.GetNamedItem(XMLTags.nameAttribute) != null)
                    Name = configNode.Attributes[XMLTags.nameAttribute].Value;
            }

            //Initalize the paths to the log files
            if (configNode.Attributes.GetNamedItem(XMLTags.pathAttribute) == null)
                throw new Exception("Malformed experiment batch file: cannot get the path to an experimental unit");

            ExperimentFileName = baseDirectory + configNode.Attributes[XMLTags.pathAttribute].Value;
            LogDescriptorFileName = Utils.GetLogFilePath(ExperimentFileName, true);
            LogFileName = Utils.GetLogFilePath(ExperimentFileName, false);

            //FORKS
            //load the value of each fork used in this experimental unit
            foreach (XmlNode fork in configNode.ChildNodes)
            {
                string forkName = fork.Attributes[XMLTags.aliasAttribute].Value;
                foreach (XmlNode value in fork.ChildNodes)
                {
                    string forkValue = value.Attributes.GetNamedItem("Value").InnerText; // The value is in the attribute named "Value"
                    ForkValues[forkName] = forkValue;
                }
            }
            //update progress
            loadOptions.OnExpUnitLoaded?.Invoke();
        }

        public bool LogFileExists()
        {
            if (File.Exists(LogFileName))
            {
                FileInfo fileInfo = new FileInfo(LogFileName);
                if (fileInfo.Length > 0)
                    return true;
            }
            return false;
        }

    }
}
