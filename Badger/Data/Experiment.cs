using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.ViewModels;

namespace Badger.Data
{
    public class Experiment
    {
        private string m_name;
        public string name { get { return m_name; } }

        private string m_configFilePath;
        public string configFilePath { get { return m_configFilePath; } }

        public string pipeName { get { return m_name; } }

        private List<string> m_prerrequisites;
        public List<string> prerrequisites { get { return m_prerrequisites; } }
        private string m_exeFile;
        public string exeFile { get { return m_exeFile; } }

        private string m_forks;

        public string Forks { get { return m_forks; } }

        public Experiment(string expName, string configFile, string exeFile,
            List<string> prerrequisites, string forks)
        {
            m_name = expName;
            m_configFilePath = configFile;
            m_exeFile = exeFile;
            m_prerrequisites = prerrequisites;
            m_forks = forks;
        }
    }
}
