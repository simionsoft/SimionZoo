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
        ExperimentalUnit m_model;
        public ExperimentalUnit Model { get { return m_model; } }

        public string ExpUnitName
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
        /// <param name="filename">path to the experimental unit</param>
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
        public LoggedExperimentalUnitViewModel(ExperimentalUnit model)
        {
            m_model = model;
        }

        public override void TraverseAction(bool doActionLocally, System.Action<SelectableTreeItem> action)
        {
            //In the fork hierarchy, experimental units are leave nodes, no nothing to be done here
        }

        /// <summary>
        /// Gets the index of a variable
        /// </summary>
        /// <param name="variableName">Name of the variable</param>
        /// <returns>Its index in the log file</returns>
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
