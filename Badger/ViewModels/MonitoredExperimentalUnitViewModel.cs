
using Caliburn.Micro;
using Herd;
using System.Collections.Generic;
using System.Linq;

namespace Badger.ViewModels
{
    public class MonitoredExperimentalUnitViewModel : PropertyChangedBase
    {
        private LoggedExperimentalUnitViewModel m_loggedExperimentalUnit;

        public string PipeName => m_loggedExperimentalUnit.Name;
        public string Name => m_loggedExperimentalUnit.Name;
        public string FilePath => m_loggedExperimentalUnit.ExperimentFileName;

        string m_taskName;
        public string TaskName
        {
            get { return m_taskName; }
            set { m_taskName = value; NotifyOfPropertyChange(() => TaskName); }
        }

        public string ExeFile { get; set; }

        public List<string> Prerequisites { get; set; }
        public Dictionary<string, string> RenameRules;

        public List<string> Forks { get; set; }

        public string ForkListAsString
        {
            get
            {
                if (Forks.Count > 0) return Data.Utility.ListAsString(Forks);
                return Name;
            }
        }

        //STATE
        public enum ExperimentState { RUNNING, FINISHED, ERROR, ENQUEUED, SENDING, RECEIVING, WAITING_EXECUTION, WAITING_RESULT };
        private ExperimentState m_state = ExperimentState.ENQUEUED;
        public ExperimentState State
        {
            get { return m_state; }
            set
            {
                //if a task within a job fails, we don't want to overwrite it's state when the job finishes
                //we don't update state in case new state is not RUNNING or SENDING
                if (m_state != ExperimentState.ERROR || value == ExperimentState.WAITING_EXECUTION)
                    m_state = value;
                NotifyOfPropertyChange(() => State);
                NotifyOfPropertyChange(() => IsRunning);
                NotifyOfPropertyChange(() => StateString);
                NotifyOfPropertyChange(() => StateColor);
            }
        }

        public void resetState()
        {
            State = ExperimentState.ENQUEUED;
            NotifyOfPropertyChange(() => State);
        }

        public bool IsRunning { get { return m_state == ExperimentState.RUNNING; } }

        public bool IsEnqueued { get { return m_state == ExperimentState.ENQUEUED; } }

        public string StateString
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

        public string StateColor
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

        private double m_progress = 0.0;
        public double Progress
        {
            get
            {
                return m_progress;
            }
            set
            {
                m_progress = value;
                NotifyOfPropertyChange(() => Progress);
            }
        }

        private string m_statusInfo;
        public string StatusInfo
        {
            get { return m_statusInfo; }
            set
            {
                m_statusInfo = value;
                NotifyOfPropertyChange(() => StatusInfo);
            }
        }

        public void ShowLog()
        {
            Data.CaliburnUtility.ShowWarningDialog(StatusInfo, "Experiment log");
        }

        public void AddStatusInfoLine(string line)
        { StatusInfo += line + "\n"; }


        //log stuff
        private Logger.LogFunction m_logFunction = null;
        private void LogMessage(string message)
        {
            m_logFunction?.Invoke(message);
        }


        public MonitoredExperimentalUnitViewModel(LoggedExperimentalUnitViewModel expUnit, string exeFile,
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
        private int m_seriesId = -1;
        public PlotViewModel m_plotEvaluationMonitor;

        public void AddEvaluationValue(double xNorm, double y)
        {
            if (m_seriesId == -1) //series not yet added
                m_seriesId = m_plotEvaluationMonitor.AddLineSeries(Name);
            m_plotEvaluationMonitor.AddLineSeriesValue(m_seriesId, xNorm, y);
        }

        /// <summary>
        ///     Get MouseEnter event from View and process it.
        /// </summary>
        /// <param name="name">The name of the component which trigger the event</param>
        public void HandleMouseEnter()
        {
            m_plotEvaluationMonitor.HighlightLineSeries(m_seriesId);
        }

        /// <summary>
        ///     Get MouseLeave event from View and process it.
        /// </summary>
        public void HandleMouseLeave()
        {
            m_plotEvaluationMonitor.ResetLineSeriesColors();
        }
    }
}
