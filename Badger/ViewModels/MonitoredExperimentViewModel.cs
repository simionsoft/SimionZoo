
using System.Xml;
using Caliburn.Micro;
using Herd;



namespace Badger.ViewModels
{
    public class MonitoredExperimentViewModel : PropertyChangedBase
    {
        public string pipeName { get { return experiment.name; } }
        public string name { get { return experiment.name; } }
        public string filePath { get { return experiment.fileName; } }

        private AppViewModel m_experiment;
        public AppViewModel experiment
        {
            get { return m_experiment; }
            set { m_experiment = value; NotifyOfPropertyChange(() => experiment); }
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

        private double m_progress;
        public double progress
        {
            get { return m_progress; }
            set
            {
                m_progress = value; NotifyOfPropertyChange(() => progress);
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

        public MonitoredExperimentViewModel(AppViewModel appViewModel, PlotViewModel evaluationMonitorVM)
        {
            experiment = appViewModel;
            evaluationMonitor = evaluationMonitorVM;
        }

        //evaluation plot stuff
        private int evaluationSeriesId = -1;
        private PlotViewModel evaluationMonitor= null;
        public void addEvaluationValue(double xNorm, double y)
        {
            if (evaluationSeriesId == -1) //series not yet added
                evaluationSeriesId = evaluationMonitor.addLineSeries(name);
            evaluationMonitor.addLineSeriesValue(evaluationSeriesId, xNorm, y);
        }
    }
 }
