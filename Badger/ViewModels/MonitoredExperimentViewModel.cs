
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
        private ExperimentalUnit m_experimentalUnit;
        public string PipeName { get { return m_experimentalUnit.pipeName; } }
        public string Name { get { return m_experimentalUnit.name; } }
        public string FilePath { get { return m_experimentalUnit.configFilePath; } }
        public string ExeFile { get { return m_experimentalUnit.exeFile; } }
        public List<string> Prerequisites { get { return m_experimentalUnit.prerrequisites; } }

        /*private Dictionary<string, string> m_forks;

        public Dictionary<string, string> Forks { get { return m_experimentalUnit.forkValues; } }*/

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
                    default:
                        return "";
                }
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
        private const double m_globalProgressUpdateRate = 5.0;
        private double m_lastProgressUpdate = -m_globalProgressUpdateRate;
        private double m_progress;
        public double progress
        {
            get { return m_progress; }
            set
            {
                m_progress = value;
                NotifyOfPropertyChange(() => progress);

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

        private ExperimentQueueMonitorViewModel m_parent;

        public MonitoredExperimentViewModel(ExperimentalUnit expUnit,
            PlotViewModel plot, ExperimentQueueMonitorViewModel parent)
        {
            evaluationMonitor = plot;
            m_experimentalUnit = expUnit;
            m_parent = parent;
        }

        //evaluation plot stuff
        private int evaluationSeriesId = -1;
        public PlotViewModel evaluationMonitor;

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
