using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Herd;
using AppXML.ViewModels;
using System.Threading;
using System.Xml;
using System.Globalization;

namespace AppXML.Data
{
    class RemoteJob
    {
        //log stuff
        public delegate void LogFunction(string message);
        private LogFunction m_logFunction = null;
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

        public RemoteJob(HerdAgentViewModel agent, List<ExperimentViewModel> experiments
            , CancellationToken cancelToken,LogFunction logFunction= null)
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

        public async Task<RemoteJob> sendJobAndMonitor(string experimentName)
        {
            m_failedExperiments.Clear();
            try
            {
                //SEND THE JOB DATA
                m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.WAITING_EXECUTION);
                CJob job = getJob(experimentName, m_experiments);

                bool bConnected = m_shepherd.connectToHerdAgent(m_herdAgent.ipAddress);
                if (bConnected)
                {
                    logMessage("Sending job to herd agent " + m_herdAgent.ipAddress);
                    m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.SENDING);
                    m_herdAgent.status = "Sending job query";
                    m_shepherd.SendJobQuery(job,m_cancelToken);
                    logMessage("Job sent to herd agent " + m_herdAgent.ipAddress);
                    //await m_shepherd.waitAsyncWriteOpsToFinish();
                    m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RUNNING);
                    m_herdAgent.status = "Executing job query";
                }
                else
                {
                    foreach(ExperimentViewModel exp in m_experiments) m_failedExperiments.Add(exp);
                    m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.ERROR);
                    logMessage("Failed to connect to herd agent " + m_herdAgent.ipAddress);
                    return this;
                }
                logMessage("Monitoring remote job run by herd agent " + m_herdAgent.ipAddress);
                //MONITOR THE REMOTE JOB
                while (true)
                {
                    int numBytesRead= await m_shepherd.readAsync(m_cancelToken);
                    string xmlItem= m_shepherd.m_xmlStream.processNextXMLItem();
                        
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
                                experimentVM.progress = Convert.ToInt32(progress);
                            }
                            else if (message.Name == "Message")
                            {
                                experimentVM.addStatusInfoLine(content);
                            }
                            else if (message.Name == "End")
                            {
                                if (content == "Ok")
                                {
                                    logMessage("Job finished sucessfully");
                                    experimentVM.state = ExperimentViewModel.ExperimentState.WAITING_RESULT;
                                }
                                else
                                {
                                    logMessage("Remote job execution wasn't successful");
                                    m_failedExperiments.Add(experimentVM);
                                    experimentVM.state = ExperimentViewModel.ExperimentState.ERROR;
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
                                    m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.RECEIVING);
                                    m_herdAgent.status = "Receiving output files";
                                    m_shepherd.ReceiveJobResult();
                                    m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.FINISHED);
                                    m_herdAgent.status = "Finished";
                                    logMessage("Job results received");
                                    return this;
                                }
                                //else if (content == CJobDispatcher.m_errorMessage)
                                //{
                                //    m_herdAgent.status = "Finished";
                                //    m_experiments.ForEach((exp) => exp.state = ExperimentViewModel.ExperimentState.ERROR);
                                //    return failedExperiments;
                                //}
                            }
                        }
                        xmlItem= m_shepherd.m_xmlStream.processNextXMLItem();
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
                m_experiments.ForEach((exp) => {exp.resetState();});
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
                foreach(ExperimentViewModel exp in m_experiments) m_failedExperiments.Add(exp);
                Console.WriteLine(ex.StackTrace);
            }
            finally
            {
                logMessage("Disconnected from herd agent " + m_herdAgent.ipAddress);
                m_shepherd.disconnect();
            }
            return this;
        }

        public static void assignExperiments(ref List<ExperimentViewModel> pendingExperiments
            , ref List<HerdAgentViewModel> freeHerdAgents
            , ref List<RemoteJob> assignments
            , CancellationToken cancelToken
            , LogFunction logFunction= null)
        {
            //lock (pendingExperiments)
            {
                assignments.Clear();
                List<ExperimentViewModel> experimentList;
                while (pendingExperiments.Count > 0 && freeHerdAgents.Count > 0)
                {
                    HerdAgentViewModel agentVM = freeHerdAgents[0];
                    freeHerdAgents.RemoveAt(0);
                    //usedHerdAgents.Add(agentVM);
                    int numProcessors = Math.Max(1, agentVM.numProcessors - 1); //we free one processor

                    experimentList = new List<ExperimentViewModel>();
                    int numPendingExperiments= pendingExperiments.Count;
                    for (int i = 0; i < Math.Min(numProcessors, numPendingExperiments); i++)
                    {
                        experimentList.Add(pendingExperiments[0]);
                        pendingExperiments.RemoveAt(0);
                    }
                    assignments.Add(new RemoteJob(agentVM, experimentList, cancelToken,logFunction));

                }
            }
        }
    }
}
