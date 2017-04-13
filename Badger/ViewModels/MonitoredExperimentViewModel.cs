
using System;
using System.Xml;
using Caliburn.Micro;
using Herd;
using System.Collections.Generic;

using Badger.Data;

namespace Badger.ViewModels
{
    public class MonitoredExperimentViewModel : PropertyChangedBase
    {
        private Experiment m_experiment;
        public string PipeName { get { return m_experiment.pipeName; } }
        public string Name { get { return m_experiment.name; } }
        public string FilePath { get { return m_experiment.configFilePath; } }
        public string exeFile { get { return m_experiment.exeFile; } }
        public List<string> prerrequisites { get { return m_experiment.prerrequisites; } }

        private string m_forks;

        public string Forks
        {
            get
            {
                /*foreach (ForkedNodeViewModel v in m_experiment.Forks)
                    m_forks += v.alias + ": " + v.selectedForkValue.configNode.content + "\n";
                    */
                return m_experiment.Forks;
            }
        }

        //STATE
        public enum ExperimentState { RUNNING, FINISHED, ERROR, ENQUEUED, SENDING, RECEIVING, WAITING_EXECUTION, WAITING_RESULT };
        private ExperimentState m_state = ExperimentState.ENQUEUED;
        public ExperimentState state
        {
            get { return m_state; }
            set
            {
                //if a task within a job fails, we don't want to overwrite it's state when the job finishes
                //we don't update state in case new state is not RUNNING or SENDING
                if (m_state != ExperimentState.ERROR || value == ExperimentState.WAITING_EXECUTION)
                    m_state = value;
                NotifyOfPropertyChange(() => state);
                NotifyOfPropertyChange(() => isRunning);
                NotifyOfPropertyChange(() => stateString);
                NotifyOfPropertyChange(() => stateColor);
            }
        }

        public void resetState()
        {
            state = ExperimentState.ENQUEUED;
            NotifyOfPropertyChange(() => state);
        }
        public bool isRunning
        {
            get { return m_state == ExperimentState.RUNNING; }
            set { }
        }
        public bool isEnqueued
        {
            get { return m_state == ExperimentState.ENQUEUED; }
            set { }
        }

        public string stateString
        {
            get
            {
                switch (m_state)
                {
                    case ExperimentState.RUNNING: return "Running";
                    case ExperimentState.FINISHED: return "Finished";
                    case ExperimentState.ERROR: return "Error";
                    case ExperimentState.SENDING: return "Sending";
                    case ExperimentState.RECEIVING: return "Receiving";
                    case ExperimentState.WAITING_EXECUTION: return "Awaiting";
                    case ExperimentState.WAITING_RESULT: return "Awaiting";
                }
                return "";
            }
        }

        public string stateColor
        {
            get
            {
                switch (m_state)
                {
                    case ExperimentState.ENQUEUED:
                    case ExperimentState.RUNNING:
                    case ExperimentState.SENDING:
                    case ExperimentState.RECEIVING:
                    case ExperimentState.WAITING_EXECUTION:
                    case ExperimentState.WAITING_RESULT:
                        return "Black";
                    case ExperimentState.FINISHED: return "DarkGreen";
                    case ExperimentState.ERROR: return "Red";
                }
                return "Black";
            }
        }

        //progress is expressed as a percentage
        private const double m_globalProgressUpdateRate = 10.0;
        private double m_lastProgressUpdate = -m_globalProgressUpdateRate;
        private double m_progress;
        public double progress
        {
            get { return m_progress; }
            set
            {
                m_progress = value; NotifyOfPropertyChange(() => progress);
                if (m_progress - m_lastProgressUpdate >= m_globalProgressUpdateRate)
                {
                    m_parent.updateGlobalProgress();
                    m_lastProgressUpdate = m_progress;
                }
            }
        }

        private string m_statusInfo;
        public string statusInfo
        {
            get { return m_statusInfo; }
            set
            {
                m_statusInfo = value;
                NotifyOfPropertyChange(() => statusInfo);
            }
        }
        public void addStatusInfoLine(string line)
        { statusInfo += line + "\n"; }


        //log stuff
        private Logger.LogFunction m_logFunction = null;
        private void logMessage(string message)
        {
            m_logFunction?.Invoke(message);
        }

        private ExperimentMonitorViewModel m_parent;

        public MonitoredExperimentViewModel(Experiment experiment, PlotViewModel plot, ExperimentMonitorViewModel parent)
        {
            evaluationMonitor = plot;
            m_experiment = experiment;
            m_parent = parent;
        }

        //evaluation plot stuff
        private int evaluationSeriesId = -1;
        public PlotViewModel evaluationMonitor = null;

        public void addEvaluationValue(double xNorm, double y)
        {
            if (evaluationSeriesId == -1) //series not yet added
                evaluationSeriesId = evaluationMonitor.addLineSeries(Name);
            evaluationMonitor.addLineSeriesValue(evaluationSeriesId, xNorm, y);
        }

        /// <summary>
        ///     Get MouseEnter event from View and process it.
        /// </summary>
        /// <param name="name">The name of the component which trigger the event</param>
        public void HandleMouseEnter(string name)
        {
            evaluationMonitor.highlightLineSeries(name);
        }

        /// <summary>
        ///     Get MouseLeave event from View and process it.
        /// </summary>
        public void HandleMouseLeave()
        {
            evaluationMonitor.resetLineSeriesColors();
        }
    }
}
