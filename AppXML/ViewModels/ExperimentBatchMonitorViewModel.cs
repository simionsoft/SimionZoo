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
    public class ExperimentBatchMonitorViewModel : PropertyChangedBase
    {
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
        public void logMessage(string message)
        {
            if (m_logFunction != null)
                m_logFunction(message);
        }

        private List<ExperimentViewModel> m_experiments;
        public List<ExperimentViewModel> experiments { get { return m_experiments; } set { } }
        private HerdAgentViewModel m_herdAgent;
        public HerdAgentViewModel herdAgent { get { return m_herdAgent; } set { m_herdAgent = value; } }
        public CancellationToken m_cancelToken;
        private Shepherd m_shepherd;
        private List<ExperimentViewModel> m_failedExperiments;
        public List<ExperimentViewModel> failedExperiments { get { return m_failedExperiments; } set { } }

        private ExperimentViewModel m_experiment;
        public ExperimentBatchMonitorViewModel(HerdAgentViewModel agent, List<ExperimentViewModel> experiments
            , CancellationToken cancelToken, ExperimentQueueMonitorViewModel.LogFunction logFunction = null)
        {
            m_herdAgent = agent;
            m_experiments = experiments;
            m_cancelToken = cancelToken;
            m_shepherd = new Shepherd();
            m_failedExperiments = new List<ExperimentViewModel>();
            m_logFunction = logFunction;
        }
        public override string ToString()
        {
            return "IP= " + m_herdAgent.ipAddressString + ", " + m_experiments.Count + " task(s)";
        }

        private CJob getJob(string experimentName, List<ExperimentViewModel> experiments)
        {
            CJob job = new CJob();
            job.name = experimentName;
            //prerrequisites
            if (Models.CApp.pre != null)
            {
                foreach (string prerec in Models.CApp.pre)
                    job.inputFiles.Add(prerec);
            }
            //tasks, inputs and outputs
            foreach (ExperimentViewModel experiment in experiments)
            {
                CTask task = new CTask();
                task.name = experiment.name;
                task.exe = Models.CApp.EXE;
                task.arguments = experiment.filePath + " " + experiment.pipeName;
                task.pipe = experiment.pipeName;
                job.tasks.Add(task);
                //add exe file to inputs
                if (!job.inputFiles.Contains(task.exe))
                    job.inputFiles.Add(task.exe);
                //add experiment file to inputs
                if (!job.inputFiles.Contains(experiment.filePath))
                    job.inputFiles.Add(experiment.filePath);
                Utility.getInputsAndOutputs(experiment.filePath, ref job);
            }
            return job;
        }

        public async Task<ExperimentBatchMonitorViewModel> sendJobAndMonitor(string experimentName)
        {
            m_failedExperiments.Clear();
            try
            {
                //SEND THE JOB DATA
                state = ExperimentState.WAITING_EXECUTION;// m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.WAITING_EXECUTION);
                CJob job = getJob(experimentName, m_experiments);

                bool bConnected = m_shepherd.connectToHerdAgent(m_herdAgent.ipAddress);
                if (bConnected)
                {
                    logMessage("Sending job to herd agent " + m_herdAgent.ipAddress);
                    state = ExperimentState.SENDING;// m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.SENDING);
                    m_herdAgent.status = "Sending job query";
                    m_shepherd.SendJobQuery(job, m_cancelToken);
                    logMessage("Job sent to herd agent " + m_herdAgent.ipAddress);
                    //await m_shepherd.waitAsyncWriteOpsToFinish();
                    state = ExperimentState.RUNNING;// m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RUNNING);
                    m_herdAgent.status = "Executing job query";
                }
                else
                {
                    foreach (ExperimentViewModel exp in m_experiments) m_failedExperiments.Add(exp);
                    state = ExperimentState.ERROR;// m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.ERROR);
                    logMessage("Failed to connect to herd agent " + m_herdAgent.ipAddress);
                    return this;
                }
                logMessage("Monitoring remote job run by herd agent " + m_herdAgent.ipAddress);
                //MONITOR THE REMOTE JOB
                while (true)
                {
                    int numBytesRead = await m_shepherd.readAsync(m_cancelToken);
                    string xmlItem = m_shepherd.m_xmlStream.processNextXMLItem();

                    while (xmlItem != "")
                    {
                        m_cancelToken.ThrowIfCancellationRequested();

                        XmlDocument doc = new XmlDocument();
                        doc.LoadXml(xmlItem);
                        XmlNode e = doc.DocumentElement;
                        string key = e.Name;
                        XmlNode message = e.FirstChild;
                        ExperimentViewModel experimentVM = m_experiments.Find(exp => exp.name == key);
                        string content = message.InnerText;
                        if (experimentVM != null)
                        {
                            if (message.Name == "Progress")
                            {
                                double progress = double.Parse(content, CultureInfo.InvariantCulture);
                                progress = Convert.ToInt32(progress); // experimentVM.progress = Convert.ToInt32(progress);
                            }
                            else if (message.Name == "Message")
                            {
                                addStatusInfoLine(content);// experimentVM.addStatusInfoLine(content);
                            }
                            else if (message.Name == "End")
                            {
                                if (content == "Ok")
                                {
                                    logMessage("Job finished sucessfully");
                                    state = ExperimentState.WAITING_RESULT;// experimentVM.state = ExperimentViewModel.ExperimentState.WAITING_RESULT;
                                }
                                else
                                {
                                    logMessage("Remote job execution wasn't successful");
                                    m_failedExperiments.Add(experimentVM);
                                    state = ExperimentState.ERROR;// experimentVM.state = ExperimentViewModel.ExperimentState.ERROR;
                                }
                            }
                        }
                        else
                        {
                            if (key == XMLStream.m_defaultMessageType)
                            {
                                //if (content == CJobDispatcher.m_endMessage)
                                {
                                    //job results can be expected to be sent back even if some of the tasks failed
                                    logMessage("Receiving job results");
                                    state = ExperimentState.RECEIVING;// m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RECEIVING);
                                    m_herdAgent.status = "Receiving output files";
                                    m_shepherd.ReceiveJobResult();
                                    state = ExperimentState.FINISHED;// m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.FINISHED);
                                    m_herdAgent.status = "Finished";
                                    logMessage("Job results received");
                                    return this;
                                }
                            }
                        }
                        xmlItem = m_shepherd.m_xmlStream.processNextXMLItem();
                    }
                }
            }

            catch (OperationCanceledException)
            {
                //quit remote jobs

                logMessage("Cancellation requested by user");
                m_shepherd.writeMessage(Shepherd.m_quitMessage, true);
                m_shepherd.read(); //we wait until we get the ack from the client

                //Thread.Sleep(1000); // we give the agents one second to process the quit request before closing the socket
                resetState();// m_experiments.ForEach((exp) => { exp.resetState(); });
                //exp.resetState(); if (!m_failedExperiments.Contains(exp)) m_failedExperiments.Add(exp); });
                m_herdAgent.status = "";
            }
            catch (Exception ex)
            {
                //to do: aqui salta cuando hay cualquier problema. Si hay problema hay que volver a lanzarlo
                //mandar a cualquier maquina que este libre
                //this.reRun(myPipes.Values);
                logMessage("Unhandled exception in Badger.sendJobAndMonitor(). Agent " + m_herdAgent.ipAddress);
                m_failedExperiments.Clear();
                foreach (ExperimentViewModel exp in m_experiments) m_failedExperiments.Add(exp);
                Console.WriteLine(ex.StackTrace);
            }
            finally
            {
                logMessage("Disconnected from herd agent " + m_herdAgent.ipAddress);
                m_shepherd.disconnect();
            }
            return this;
        }
    }
 }
