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
            loadOptions.OnExpUnitLoaded?.Invoke(this);
        }

        /// <summary>
        /// Use this if you want to load an experimental unit only depending on whether the log file exists or not
        /// </summary>
        /// <param name="logFileName"></param>
        /// <returns></returns>
        static public bool LogFileExists(string relExplUnitPath, string baseDirectory)
        {
            string experimentFileName = baseDirectory + relExplUnitPath;
            string logDescriptorFileName = Utils.GetLogFilePath(experimentFileName, true);
            string logFileName = Utils.GetLogFilePath(experimentFileName, false);

            return FileExistsAndNotEmpty(logDescriptorFileName) && FileExistsAndNotEmpty(logFileName);
        }
        public bool LogFileExists()
        {
            return FileExistsAndNotEmpty(LogFileName) && FileExistsAndNotEmpty(LogDescriptorFileName);
        }

        static bool FileExistsAndNotEmpty(string fileName)
        {
            if (File.Exists(fileName))
            {
                FileInfo fileInfo = new FileInfo(fileName);
                if (fileInfo.Length > 0)
                    return true;
            }
            return false;
        }
    }
}
