using Herd.Network;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Xml;

namespace Herd.Files
{
    public class ExperimentalUnit
    {
        public string Name;
        public string ExperimentFileName;
        public string RelativeExperimentFileName
        {
            get
            {
                return Folders.experimentRelativeDir + "/" + Utils.RemoveDirectories(ExperimentFileName, 2);
            }
        }
        public string LogDescriptorFileName;
        public string LogFileName;

        public Dictionary<string, string> ForkValues = new Dictionary<string, string>();
        public List<string> Variables { get; set; } = new List<string>();

        /// <summary>
        /// Minimal constructor for testing
        /// </summary>
        /// <param name="name"></param>
        public ExperimentalUnit(string name, List<AppVersion> appVersions, RunTimeRequirements runTimeRequirements)
        {
            Name = name;
            AppVersions = appVersions;
            RunTimeReqs = runTimeRequirements;
        }

        /// <summary>
        /// Constructor used to load the experimental unit from a file
        /// </summary>
        /// <param name="configNode"></param>
        /// <param name="baseDirectory"></param>
        /// <param name="loadOptions"></param>
        public ExperimentalUnit(XmlNode configNode, string baseDirectory, LoadOptions loadOptions)
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

        public List<AppVersion> AppVersions { get; set; }
        public AppVersion SelectedVersion { get; set; }
        public RunTimeRequirements RunTimeReqs { get; set; }

        public void GetRuntimeRequirements(AppVersion selectedVersion, List<AppVersion> appVersions)
        {
            //Added for testing purposes: this avoids using real experiments
            if (RunTimeReqs == null)
            {
                //We can only get the runtime requirements of an app compatible with the current architecture
                if (IsHostArchitectureCompatible(selectedVersion))
                    RunTimeReqs = GetRunTimeRequirements(selectedVersion.ExeFile, ExperimentFileName);
                else
                {
                    AppVersion compatibleVersion = BestHostArchitectureMatch(appVersions);
                    RunTimeReqs = GetRunTimeRequirements(compatibleVersion.ExeFile, ExperimentFileName);
                }
            }
        }

        public bool IsHostArchitectureCompatible(AppVersion version)
        {
            string hostArchitecture = HerdAgent.ArchitectureId();
            if (hostArchitecture == version.Requirements.Architecture
                || (hostArchitecture == PropValues.Win64 && version.Requirements.Architecture==PropValues.Win32) ) //hard-coded for now
                return true;
            else
                return false;
        }

        public AppVersion BestHostArchitectureMatch(List<AppVersion> appVersions)
        {
            string hostArchitecture = HerdAgent.ArchitectureId();
            foreach (AppVersion version in appVersions)
            {
                if (version.Requirements.Architecture == hostArchitecture)
                    return version;
            }
            return null;
        }

        //this function is called from several tasks and needs to be synchronized
        [MethodImpl(MethodImplOptions.Synchronized)]
        public static RunTimeRequirements GetRunTimeRequirements(string exe, string args)
        {
            object o = new object();
            Monitor.Enter(o);
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = exe,
                    Arguments = args + " -requirements",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };

            string processOutput = "";
            process.Start();
            while (!process.StandardOutput.EndOfStream)
            {
                processOutput += process.StandardOutput.ReadLine();
            }

            //Parse the output
            RunTimeRequirements runTimeRequirements = null;
            int startPos = processOutput.IndexOf("<" + Herd.Network.XmlTags.Requirements + ">");
            int endPos = processOutput.IndexOf("</" + Herd.Network.XmlTags.Requirements + ">");
            if (startPos >= 0 && endPos > 0)
            {
                string xml = processOutput.Substring(startPos, endPos - startPos + ("</" + Herd.Network.XmlTags.Requirements + ">").Length);

                XmlDocument doc = new XmlDocument();
                doc.LoadXml(xml);

                runTimeRequirements = new RunTimeRequirements(doc.FirstChild);

            }
            Monitor.Exit(o);
            return runTimeRequirements;
        }
    }
}
