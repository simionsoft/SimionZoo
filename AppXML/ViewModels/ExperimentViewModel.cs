using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Dynamic;
using System.Windows;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class ExperimentViewModel:Caliburn.Micro.PropertyChangedBase
    {
        public enum ExperimentState { RUNNING, FINISHED, ERROR, ENQUEUED, SENDING,RECEIVING, WAITING_EXECUTION, WAITING_RESULT};
        private ExperimentState m_state = ExperimentState.ENQUEUED;
        public ExperimentState state { get { return m_state; }
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
            NotifyOfPropertyChange(()=>state);
        }
        private bool m_editionEnabled = true;
        public void enableEdition(bool bEnable)
        {
            m_editionEnabled = bEnable;
        }
        public bool isEditionEnabled { get { return m_editionEnabled; } set { } }

        public bool isRunning
        {
            get { return m_state==ExperimentState.RUNNING; }
            set { }
        }
        public bool isEnqueued
        {
            get { return m_state == ExperimentState.ENQUEUED; }
            set { }
        }

        public string stateString{
            get{switch (m_state){
                    case ExperimentState.RUNNING: return "Running";
                    case ExperimentState.FINISHED: return "Finished";
                    case ExperimentState.ERROR: return "Error";
                    case ExperimentState.SENDING: return "Sending";
                    case ExperimentState.RECEIVING: return "Receiving";
                    case ExperimentState.WAITING_EXECUTION: return "Awaiting";
                    case ExperimentState.WAITING_RESULT: return "Awaiting";
            }
                return "";}}

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

        private string m_filePath;
        public string filePath { get { return m_filePath; } set { m_filePath = value; } }

        private static string RemoveSpecialCharacters(string str)
        {
            StringBuilder sb = new StringBuilder();
            foreach (char c in str)
            {
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '-' || c == '_')
                {
                    sb.Append(c);
                }
            }
            return sb.ToString();
        }

        //private string m_tags;
        //public string tags { get { return m_tags; } set { m_tags = value; } }

        private string m_name;
        public string name { get { return m_name; }
            set { m_name = RemoveSpecialCharacters(value); NotifyOfPropertyChange(() => name); }
        }

        public string pipeName { get { return m_name; } }

        private double m_progress;
        public double progress { get { return m_progress; }
            set { m_progress = value; NotifyOfPropertyChange(() => progress); 
            //if (m_progress==100.0) state=ExperimentState.FINISHED; //no longer needed because herd agents send a per-task end message?
            }
        }

        private string m_statusInfo;
        public string statusInfo { get { return m_statusInfo; } set { m_statusInfo = value;
            NotifyOfPropertyChange(() => statusInfo); 
            NotifyOfPropertyChange(() => isStatusInfo);} }
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

        private XmlDocument m_experimentXML;
        public XmlDocument experimentXML{get{return m_experimentXML;} set{m_experimentXML=value;}}

        public ExperimentViewModel(string name, XmlDocument experimentXML)
        {
            m_name = name;
            m_experimentXML = experimentXML;
        }
    }
 
}