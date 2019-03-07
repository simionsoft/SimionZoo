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

using Herd.Network;
using Herd;

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
        public static bool LogFileExists(string relExplUnitPath, string baseDirectory)
        {
            string experimentFileName = baseDirectory + relExplUnitPath;
            string logDescriptorFileName = Utils.GetLogFilePath(experimentFileName, true);
            string logFileName = Utils.GetLogFilePath(experimentFileName, false);

            return Utils.FileExistsAndNotEmpty(logDescriptorFileName) && Utils.FileExistsAndNotEmpty(logFileName);
        }
        public bool LogFileExists()
        {
            return Utils.FileExistsAndNotEmpty(LogFileName) && Utils.FileExistsAndNotEmpty(LogDescriptorFileName);
        }



        public List<AppVersion> AppVersions { get; set; }
        public AppVersion SelectedVersion { get; set; }
        public RunTimeRequirements RunTimeReqs { get; set; }

        /// <summary>
        /// Executes the app to retrive the runtime requirements
        /// </summary>
        public void RequestRuntimeRequirements()
        {
            //Added for testing purposes: this avoids using real experiments
            if (RunTimeReqs == null)
            {
                foreach(AppVersion appVersion in AppVersions)
                {
                    if (IsHostArchitectureCompatible(appVersion))
                    {
                        RunTimeReqs = GetRunTimeRequirements(appVersion.ExeFile, ExperimentFileName);
                        if (RunTimeReqs != null)
                        {
                            break;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Returns whether an app version is compatible with the host architecture
        /// </summary>
        /// <param name="version">The appversion.</param>
        public bool IsHostArchitectureCompatible(AppVersion version)
        {
            string hostArchitecture = HerdAgent.ArchitectureId();
            if (hostArchitecture == version.Requirements.Architecture
                || (hostArchitecture == PropValues.Win64 && version.Requirements.Architecture==PropValues.Win32) ) //hard-coded for now
                return true;
            else
                return false;
        }

        /// <summary>
        /// Returns the app version from the list that best matches the host architecture
        /// </summary>
        /// <param name="appVersions">The application versions.</param>
        /// <returns></returns>
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
            if (processOutput == "")
                return null; //something went wrong, possibly the experiment cannot be run with this AppVersion
            int startPos = processOutput.IndexOf("<" + XmlTags.Requirements + ">");
            int endPos = processOutput.IndexOf("</" + XmlTags.Requirements + ">");
            if (startPos >= 0 && endPos > 0)
            {
                string xml = processOutput.Substring(startPos, endPos - startPos + ("</" + XmlTags.Requirements + ">").Length);

                XmlDocument doc = new XmlDocument();
                doc.LoadXml(xml);

                runTimeRequirements = new RunTimeRequirements(doc.FirstChild);
            }
            Monitor.Exit(o);
            return runTimeRequirements;
        }
    }
}
