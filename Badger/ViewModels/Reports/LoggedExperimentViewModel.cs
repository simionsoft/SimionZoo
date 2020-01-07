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

using System;
using System.Collections.Generic;
using System.Xml;

using Herd.Files;
using Herd.Network;

namespace Badger.ViewModels
{
    public class LoggedExperimentViewModel : SelectableTreeItem
    {
        private Experiment m_model = null;
        public Experiment Model { get { return m_model; } }

        public string ExperimentName { get { return m_model.Name; } set { m_model.Name = value; } }

        public List<AppVersion> AppVersions { get { return m_model.AppVersions; } }

        public List<LoggedForkViewModel> Forks { get; set; } = new List<LoggedForkViewModel>();

        public List<LoggedExperimentalUnitViewModel> ExperimentalUnits { get; set; } = new List<LoggedExperimentalUnitViewModel>();

        public override void TraverseAction(bool doActionLocally, Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in Forks) fork.TraverseAction(true, action);
        }

        public void ToggleIsExpanded()
        {
            TraverseAction(false, (SelectableTreeItem item)=> item.IsVisible = !item.IsVisible);
        }

        /// <summary>
        ///     Class constructor.
        /// </summary>
        /// <param name="experiment">The experiment with all the data used in the view model</param>

        public LoggedExperimentViewModel(Experiment experiment)
        {
            m_model = experiment;

            //Create the fork view models from the models
            foreach (Fork fork in experiment.Forks)
            {
                LoggedForkViewModel forkVM = new LoggedForkViewModel(fork);
                Forks.Add(forkVM);
            }
            //Create the experimental unit view models from the models
            foreach (ExperimentalUnit expUnit in experiment.ExperimentalUnits)
            {
                LoggedExperimentalUnitViewModel expUnitVM = new LoggedExperimentalUnitViewModel(expUnit);
                foreach (string variable in expUnitVM.VariablesInLog)
                    AddVariable(variable);
                ExperimentalUnits.Add(expUnitVM);
            }
            //Associate forks and experiment units
            foreach (LoggedExperimentalUnitViewModel expUnit in ExperimentalUnits)
            {
                TraverseAction(false, (node) =>
                {
                    if (node is LoggedForkValueViewModel forkValue)
                    {
                        foreach (string forkName in expUnit.ForkValues.Keys)
                        {
                            if (forkName == forkValue.Parent.ForkName && expUnit.ForkValues[forkName] == forkValue.Value)
                            {
                                forkValue.ExpUnits.Add(expUnit);
                            }
                        }
                    }
                });
            }
        }

        /// <summary>
        ///     Call after reading the log file descriptor of each experimetal unit
        /// </summary>
        /// <param name="variableName"></param>
        public void AddVariable(string variableName)
        {
            if (!VariablesInLog.Contains(variableName))
                VariablesInLog.Add(variableName);
        }

        //Variables
        public List<string> VariablesInLog { get; set; }
            = new List<string>();
    }
}