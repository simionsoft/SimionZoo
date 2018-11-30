using Caliburn.Micro;
using Herd;
using System.Collections.Generic;
using System.Linq;
using Badger.Data;

namespace Badger.ViewModels
{
    public class MonitoredExperimentalUnitViewModel : PropertyChangedBase
    {
        private LoggedExperimentalUnitViewModel m_loggedExperimentalUnit;

        string m_pipeName;
        public string PipeName => m_pipeName;
        string m_name;
        public string Name => m_name;
        string m_filePath;
        public string FilePath => m_filePath;

        string m_taskName;
        public string TaskName
        {
            get { return m_taskName; }
            set { m_taskName = value; NotifyOfPropertyChange(() => TaskName); }
        }

        List<AppVersion> m_appVersions;
        public List<AppVersion> AppVersions { get { return m_appVersions; } }

        AppVersion m_selectedVersion;
        public AppVersion SelectedVersion{ get { return m_selectedVersion; } set { m_selectedVersion = value; } }

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
            CaliburnUtility.ShowWarningDialog(StatusInfo, "Experiment log");
        }

        public void AddStatusInfoLine(string line)
        { StatusInfo += line + "\n"; }


        //log stuff
        private Logger.LogFunction m_logFunction = null;
        private void LogMessage(string message)
        {
            m_logFunction?.Invoke(message);
        }

        /// <summary>
        /// Dumb constructor used only for testing purposes
        /// </summary>
        /// <param name="name">Name given to the experimental unit</param>
        /// <param name="appVersions">Versions of the app being executed</param>
        /// <param name="runTimeRequirements">Run-time requirements</param>
        public MonitoredExperimentalUnitViewModel(string name, List<AppVersion> appVersions, RunTimeRequirements runTimeRequirements)
        {
            m_appVersions = appVersions;
            m_runTimeRequirements = runTimeRequirements;
            m_pipeName = name;
            m_name = name;
            m_filePath = name;
        }

        /// <summary>
        /// Main constructor
        /// </summary>
        /// <param name="expUnit"></param>
        /// <param name="plot"></param>
        /// <param name="appVersions"></param>
        public MonitoredExperimentalUnitViewModel(LoggedExperimentalUnitViewModel expUnit, PlotViewModel plot, List<AppVersion> appVersions)
        {
            m_appVersions = appVersions;
            m_loggedExperimentalUnit = expUnit;
            m_pipeName = m_loggedExperimentalUnit.Name;
            m_name = m_loggedExperimentalUnit.Name;
            m_filePath = m_loggedExperimentalUnit.ExperimentFileName;

            Forks = expUnit.forkValues.Select(k => k.Key + ": " + k.Value).ToList();
            m_plotEvaluationMonitor = plot;
        }

        //run-time app requirements
        RunTimeRequirements m_runTimeRequirements = null;
        public RunTimeRequirements RunTimeReqs
        {
            get { return m_runTimeRequirements; }
        }

        public void GetRuntimeRequirements(AppVersion selectedVersion, List<AppVersion> appVersions)
        {
            //Added for testing purposes: this avoids using real experiments
            if (m_runTimeRequirements == null)
            {
                //We can only get the runtime requirements of an app compatible with the current architecture
                if (IsHostArchitectureCompatible(selectedVersion))
                    m_runTimeRequirements = Utility.GetRunTimeRequirements(selectedVersion.ExeFile, FilePath);
                else
                {
                    AppVersion compatibleVersion = BestHostArchitectureMatch(appVersions);
                    m_runTimeRequirements = Utility.GetRunTimeRequirements(compatibleVersion.ExeFile, FilePath);
                }
            }
        }

        public bool IsHostArchitectureCompatible(AppVersion version)
        {
            if (HerdAgent.ArchitectureId() == version.Requirements.Architecture)
                return true;
            else
                return false;
        }

        public AppVersion BestHostArchitectureMatch(List<AppVersion> appVersions)
        {
            string hostArchitecture = HerdAgent.ArchitectureId();
            foreach (AppVersion version in appVersions)
            {
                if (version.Requirements.Architecture == hostArchitecture)
                    return version;
            }
            return null;
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
