using System;
using System.Collections.Generic;
using System.Xml;

using Herd.Files;
using Herd.Network;

namespace Badger.ViewModels
{
    public class LoggedExperimentViewModel : SelectableTreeItem
    {
        private LoggedExperiment m_model = null;
        public LoggedExperiment Model { get { return m_model; } }

        public string Name { get { return m_model.Name; } set { m_model.Name = value; } }

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

        public LoggedExperimentViewModel(LoggedExperiment experiment)
        {
            m_model = experiment;

            //Create the fork view models from the models
            foreach (LoggedFork fork in experiment.Forks)
            {
                LoggedForkViewModel forkVM = new LoggedForkViewModel(fork);
                Forks.Add(forkVM);
            }
            //Create the experimental unit view models from the models
            foreach (LoggedExperimentalUnit expUnit in experiment.ExperimentalUnits)
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
                            if (forkName == forkValue.Parent.Name && expUnit.ForkValues[forkName] == forkValue.Value)
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