using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Xml;
using Herd.Files;

namespace Herd.Network
{
    public class MonitoredExperimentalUnit
    {
        public List<AppVersion> AppVersions { get; set; }
        public AppVersion SelectedVersion { get; set; }
        public RunTimeRequirements RunTimeReqs { get; set; }
        string FilePath;
        public string Name;

        /// <summary>
        /// Main constructor
        /// </summary>
        /// <param name="expUnitFilePath"></param>
        /// <param name="appVersions"></param>
        public MonitoredExperimentalUnit(string expUnitFilePath, List<AppVersion> appVersions)
        {
            FilePath = expUnitFilePath;
            AppVersions = appVersions;
        }

        /// <summary>
        /// Alternative constructor used only for testing purposes
        /// </summary>
        /// <param name="expUnitFilePath"></param>
        /// <param name="appVersions"></param>
        public MonitoredExperimentalUnit(string name, List<AppVersion> appVersions, RunTimeRequirements runTimeRequirements)
        {
            Name = name;
            AppVersions = appVersions;
            RunTimeReqs = runTimeRequirements;
        }

        public void GetRuntimeRequirements(AppVersion selectedVersion, List<AppVersion> appVersions)
        {
            //Added for testing purposes: this avoids using real experiments
            if (RunTimeReqs == null)
            {
                //We can only get the runtime requirements of an app compatible with the current architecture
                if (IsHostArchitectureCompatible(selectedVersion))
                    RunTimeReqs = GetRunTimeRequirements(selectedVersion.ExeFile, FilePath);
                else
                {
                    AppVersion compatibleVersion = BestHostArchitectureMatch(appVersions);
                    RunTimeReqs = GetRunTimeRequirements(compatibleVersion.ExeFile, FilePath);
                }
            }
        }

        public bool IsHostArchitectureCompatible(AppVersion version)
        {
            if (HerdAgent.ArchitectureId() == version.Requirements.Architecture)
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
