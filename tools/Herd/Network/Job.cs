using System;
using System.Collections.Generic;


namespace Herd.Network
{
    public class HerdJob
    {
        //name
        string m_name = "";
        public string Name { get { return m_name; } set { m_name = value; } }

        //tasks
        List<HerdTask> m_tasks = new List<HerdTask>();
        public void AddTask(HerdTask task)
        {
            m_tasks.Add(task);
        }
        public List<HerdTask> Tasks { get { return m_tasks; } }

        //input files
        List<string> m_inputFiles = new List<string>();
        public void AddInputFiles(List<string> source) { foreach (string file in source) AddInputFile(file); }
        public bool AddInputFile(string file)
        {
            if (!m_inputFiles.Contains(file))
            {
                m_inputFiles.Add(file);
                return true;
            }
            else return false;
        }
        public List<string> InputFiles { get { return m_inputFiles; } }

        //output files
        List<string> m_outputFiles = new List<string>();
        public void AddOutputFiles(List<string> source) { foreach (string file in source) AddOutputFile(file); }
        public bool AddOutputFile(string file)
        {
            if (!m_outputFiles.Contains(file))
            {
                m_outputFiles.Add(file);
                return true;
            }
            else return false;
        }
        public List<string> OutputFiles { get { return m_outputFiles; } }

        //rename rules
        Dictionary<string, string> m_renameRules = new Dictionary<string, string>();
        public Dictionary<string, string> RenameRules { get { return m_renameRules; } }
        public void AddRenameRule(string original, string renamed) { m_renameRules[original] = renamed; }
        public void AddRenameRules(Dictionary<string, string> source) { foreach (string key in source.Keys) m_renameRules[key] = source[key]; }



        //constructor
        public HerdJob(string name) { m_name = name; }

        public override string ToString()
        {
            string ret = "Job: " + m_name;
            foreach (HerdTask task in m_tasks)
                ret += "||" + task.ToString();
            return ret;
        }

        public string RenamedFilename(string filename)
        {
            if (m_renameRules != null && m_renameRules.ContainsKey(filename))
                return m_renameRules[filename];
            return filename;
        }
        public string OriginalFilename(string filename)
        {
            if (m_renameRules != null)
            {
                foreach (string originalName in m_renameRules.Keys)
                {
                    if (m_renameRules[originalName] == filename)
                        return originalName;
                }
            }
            return filename;
        }
    }
}
