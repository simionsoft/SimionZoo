using AppXML.Models;
using AppXML.Data;
using System.Collections.ObjectModel;
using System.Xml;
using System.Windows;
using Caliburn.Micro;
using System.Windows.Forms;
using System.IO;
using System.Dynamic;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;
using System.IO.Pipes;
using System.Xml.Linq;
using System.Linq;
using System.Xml.XPath;
using System.Windows.Media;
using System.Threading;
using System.Globalization;
using System.Collections.Concurrent;
using Herd;
using AppXML.ViewModels;


namespace AppXML.ViewModels
{
    public class MonitoredExperimentViewModel : PropertyChangedBase
    {
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; NotifyOfPropertyChange(() => name); } }

        public string pipeName { get { return m_name; } }

        private string m_filePath;
        public string filePath { get { return m_filePath; } set { m_filePath = value; } }

        private XmlDocument m_experimentXML;
        public XmlDocument experimentXML { get { return m_experimentXML; } set { m_experimentXML = value; } }
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
                NotifyOfPropertyChange(() => isStatusInfo);
            }
        }
        public void addStatusInfoLine(string line)
        { statusInfo += line + "\n"; }
        public void showStatusInfoInNewWindow()
        {
            DialogViewModel dvm = new DialogViewModel("Experiment status info", this.m_statusInfo, DialogViewModel.DialogType.Info);
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = "Experiment status info";
            settings.WindowState = WindowState.Normal;
            settings.WindowResize = ResizeMode.CanMinimize;
            new WindowManager().ShowWindow(dvm, null, settings);
        }
        public bool isStatusInfo { get { return m_statusInfo != ""; } set { } }

        //log stuff
        private ExperimentQueueMonitorViewModel.LogFunction m_logFunction = null;
        private void logMessage(string message)
        {
            if (m_logFunction != null)
                m_logFunction(message);
        }

        public MonitoredExperimentViewModel(ExperimentViewModel experiment)
        {
            name = experiment.name;
            filePath = experiment.filePath;
            experimentXML= experiment.experimentXML;
        }
        //public MonitoredExperimentViewModel(HerdAgentViewModel agent, List<ExperimentViewModel> experiments
        //    , CancellationToken cancelToken, ExperimentQueueMonitorViewModel.LogFunction logFunction = null)
        //{
        //    m_herdAgent = agent;
        //    m_experiments = experiments;
        //    m_cancelToken = cancelToken;
        //    m_shepherd = new Shepherd();
        //    m_failedExperiments = new List<ExperimentViewModel>();
        //    m_logFunction = logFunction;
        //}
        //public override string ToString()
        //{
        //    return "IP= " + m_herdAgent.ipAddressString + ", " + m_experiments.Count + " task(s)";
        //}

    }
 }
