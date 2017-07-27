
using Caliburn.Micro;
using Herd;
using System.Collections.Generic;
using System.Linq;

namespace Badger.ViewModels
{
    public class MonitoredExperimentViewModel : PropertyChangedBase
    {
        private LoggedExperimentalUnitViewModel m_loggedExperimentalUnit;

        public string PipeName => m_loggedExperimentalUnit.name;
        public string Name => m_loggedExperimentalUnit.name;
        public string FilePath => m_loggedExperimentalUnit.experimentFilePath;

        public string ExeFile { get; set; }

        public List<string> Prerequisites { get; set; }
        public Dictionary<string, string> RenameRules;

        public List<string> Forks { get; set; }

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

        public bool isRunning { get { return m_state == ExperimentState.RUNNING; } }

        public bool isEnqueued { get { return m_state == ExperimentState.ENQUEUED; } }

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

        private double m_progress;
        public double Progress
        {
            get { return m_progress; }
            set
            {
                m_progress = value;
                NotifyOfPropertyChange(() => Progress);
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


        public MonitoredExperimentViewModel(LoggedExperimentalUnitViewModel expUnit, string exeFile,
           List<string> prerequisites, Dictionary<string, string> renameRules, PlotViewModel plot)
        {
            m_loggedExperimentalUnit = expUnit;
            ExeFile = exeFile;
            Prerequisites = prerequisites;
            RenameRules = renameRules;
            Forks = expUnit.forkValues.Select(k => k.Key + ": " + k.Value).ToList();
            m_plotEvaluationMonitor = plot;
        }

        //evaluation plot stuff
        private int evaluationSeriesId = -1;
        public PlotViewModel m_plotEvaluationMonitor;

        public void addEvaluationValue(double xNorm, double y)
        {
            if (evaluationSeriesId == -1) //series not yet added
                evaluationSeriesId = m_plotEvaluationMonitor.addLineSeries(Name);
            m_plotEvaluationMonitor.addLineSeriesValue(evaluationSeriesId, xNorm, y);
        }

        /// <summary>
        ///     Get MouseEnter event from View and process it.
        /// </summary>
        /// <param name="name">The name of the component which trigger the event</param>
        public void HandleMouseEnter(string name)
        {
            m_plotEvaluationMonitor.highlightLineSeries(name);
        }

        /// <summary>
        ///     Get MouseLeave event from View and process it.
        /// </summary>
        public void HandleMouseLeave()
        {
            m_plotEvaluationMonitor.resetLineSeriesColors();
        }
    }
}
