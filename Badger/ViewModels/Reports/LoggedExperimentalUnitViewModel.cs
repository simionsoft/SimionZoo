using System.Xml;
using System.Collections.Generic;
using System;

using Caliburn.Micro;

using Badger.Data;

using Herd.Files;

namespace Badger.ViewModels
{
    public class LoggedExperimentalUnitViewModel : SelectableTreeItem
    {
        LoggedExperimentalUnit m_model;
        public LoggedExperimentalUnit Model { get { return m_model; } }

        public string Name
        {
            get { return m_model.Name; }
        }

        public string ExperimentFileName { get { return m_model.ExperimentFileName; } }
        public string LogDescriptorFileName { get { return m_model.LogDescriptorFileName; } }
        public string LogFileName { get { return m_model.LogFileName; } }


        //private Dictionary<string, string> m_forkValues = new Dictionary<string, string>();
        public Dictionary<string, string> ForkValues
        {
            get { return m_model.ForkValues; }
            //set { m_forkValues = value; NotifyOfPropertyChange(() => ForkValues); }
        }
        public string ForkValuesAsString
        {
            get { return Herd.Utils.DictionaryAsString(ForkValues); }
        }

        public bool ContainsForks(BindableCollection<string> forks)
        {
            foreach (string fork in forks)
            {
                if (fork!=ReportsWindowViewModel.GroupByExperimentId && !ForkValues.ContainsKey(fork))
                    return false;
            }
            return true;
        }

        public List<string> VariablesInLog { get { return m_model.Variables; } }


        /// <summary>
        /// Fake constructor for testing purposes
        /// </summary>
        /// <param name="filename"></param>
        public LoggedExperimentalUnitViewModel(string filename)
        {
            //ExperimentFileName = filename;
            //Name = Herd.Utils.GetFilename(filename);
        }

        /// <summary>
        /// Main constructor
        /// </summary>
        /// <param name="configNode"></param>
        /// <param name="baseDirectory"></param>
        /// <param name="updateFunction"></param>

        public LoggedExperimentalUnitViewModel(LoggedExperimentalUnit model)
        {
            m_model = model;
        }

        public override void TraverseAction(bool doActionLocally, System.Action<SelectableTreeItem> action)
        {
            //In the fork hierarchy, experimental units are leave nodes, no nothing to be done here
        }

        public int GetVariableIndex(string variableName)
        {
            int index = 0;
            foreach (string variable in VariablesInLog)
            {
                if (variable == variableName)
                    return index;
                index++;
            }
            return -1;
        }
        
    }
}
