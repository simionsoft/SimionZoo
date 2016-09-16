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
using AppXML.Models;
using AppXML.ViewModels;
using AppXML.Data;


namespace AppXML.ViewModels
{
    public class ExperimentBatch
    {
        private List<MonitoredExperimentViewModel> m_monitoredExperiments;
        private HerdAgentViewModel m_herdAgent;
        public HerdAgentViewModel herdAgent { get { return m_herdAgent; } }
        private string m_name;
        public CancellationToken m_cancelToken;
        private Shepherd m_shepherd;

        private PlotViewModel m_evaluationPlot;
        private Dictionary<string, int> m_experimentSeriesId;

        private List<MonitoredExperimentViewModel> m_failedExperiments= new List<MonitoredExperimentViewModel>();
        public List<MonitoredExperimentViewModel> failedExperiments { get { return m_failedExperiments; } set { } }

        private Logger.LogFunction m_logFunction = null;
        private void logMessage(string message)
        {
            m_logFunction?.Invoke(message);
        }

        public ExperimentBatch(string name, List<MonitoredExperimentViewModel> experiments,HerdAgentViewModel herdAgent
            ,PlotViewModel evaluationPlot,CancellationToken cancelToken, Logger.LogFunction logFunction)
        {
            m_name = name;
            m_monitoredExperiments = experiments;
            m_herdAgent = herdAgent;
            m_logFunction = logFunction;
            m_shepherd = new Shepherd();
            m_shepherd.setLogMessageHandler(logFunction);
            m_cancelToken = cancelToken;
            m_evaluationPlot = evaluationPlot;
            m_experimentSeriesId = new Dictionary<string, int>();
        }

        private CJob getJob()
        {
            CJob job = new CJob();
            job.name = m_name;
            //prerrequisites
            if (Models.CApp.pre != null)
            {
                foreach (string prerec in Models.CApp.pre)
                    job.inputFiles.Add(prerec);
            }
            //tasks, inputs and outputs
            foreach (MonitoredExperimentViewModel monitoredExperiment in m_monitoredExperiments)
            {
                CTask task = new CTask();
                task.name = monitoredExperiment.name;
                task.exe = Models.CApp.EXE;
                task.arguments = monitoredExperiment.filePath + " " + monitoredExperiment.pipeName;
                task.pipe = monitoredExperiment.pipeName;
                job.tasks.Add(task);
                //add exe file to inputs
                if (!job.inputFiles.Contains(task.exe))
                    job.inputFiles.Add(task.exe);
                //add experiment file to inputs
                if (!job.inputFiles.Contains(monitoredExperiment.filePath))
                    job.inputFiles.Add(monitoredExperiment.filePath);
                Utility.getInputsAndOutputs(monitoredExperiment.filePath, ref job);
            }
            return job;
        }

        public async Task<ExperimentBatch> sendJobAndMonitor(string experimentName)
        {
            m_failedExperiments.Clear();
            try
            {
                //SEND THE JOB DATA
                m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.WAITING_EXECUTION);
                CJob job = getJob();

                bool bConnected = m_shepherd.connectToHerdAgent(m_herdAgent.ipAddress);
                if (bConnected)
                {
                    logMessage("Sending job to herd agent " + m_herdAgent.ipAddress);
                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.SENDING);
                    m_herdAgent.status = "Sending job query";
                    m_shepherd.SendJobQuery(job, m_cancelToken);
                    logMessage("Job sent to herd agent " + m_herdAgent.ipAddress);
                    //await m_shepherd.waitAsyncWriteOpsToFinish();
                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.RUNNING);
                    m_herdAgent.status = "Executing job query";
                }
                else
                {
                    foreach (MonitoredExperimentViewModel exp in m_monitoredExperiments) m_failedExperiments.Add(exp);
                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.ERROR);
                    logMessage("Failed to connect to herd agent " + m_herdAgent.ipAddress);
                    return this;
                }
                logMessage("Monitoring remote job run by herd agent " + m_herdAgent.ipAddress);
                //MONITOR THE REMOTE JOB
                while (true)
                {
                    int numBytesRead = await m_shepherd.readAsync(m_cancelToken);
                    m_cancelToken.ThrowIfCancellationRequested();

                    string xmlItem = m_shepherd.m_xmlStream.processNextXMLItem();

                    while (xmlItem != "")
                    {
                        string experimentId = m_shepherd.m_xmlStream.getLastXMLItemTag();
                        string message = m_shepherd.m_xmlStream.getLastXMLItemContent();
                        MonitoredExperimentViewModel experimentVM = m_monitoredExperiments.Find(exp => exp.name == experimentId);
                        string messageId = m_shepherd.m_xmlStream.getLastXMLItemTag(); //previous call to getLastXMLItemContent reset lastXMLItem
                        string messageContent = m_shepherd.m_xmlStream.getLastXMLItemContent();
                        if (experimentVM != null)
                        {
                            if (messageId == "Progress")
                            {
                                double progress = double.Parse(messageContent, CultureInfo.InvariantCulture);
                                experimentVM.progress = Convert.ToInt32(progress);
                            }
                            else if (messageId=="Evaluation")
                            {
                                //<Evaluation>0.0,-1.23</Evaluation>
                                string [] values= messageContent.Split(',');
                                string seriesName= experimentVM.name;
                                int seriesId;
                                if (values.Length==2)
                                {
                                    if (!m_experimentSeriesId.Keys.Contains(experimentVM.name))
                                    {
                                        seriesId = m_evaluationPlot.addLineSeries(seriesName);
                                        m_experimentSeriesId.Add(seriesName, seriesId);
                                    }
                                    else seriesId = m_experimentSeriesId[seriesName];

                                    m_evaluationPlot.addLineSeriesValue(seriesId,double.Parse(values[0], CultureInfo.InvariantCulture)
                                        ,double.Parse(values[1], CultureInfo.InvariantCulture));
                                }
                            }
                            else if (messageId == "Message")
                            {
                                experimentVM.addStatusInfoLine(messageContent);
                            }
                            else if (messageId == "End")
                            {
                                if (messageContent == "Ok")
                                {
                                    logMessage("Job finished sucessfully");
                                    experimentVM.state = MonitoredExperimentViewModel.ExperimentState.WAITING_RESULT;
                                }
                                else
                                {
                                    logMessage("Remote job execution wasn't successful");
                                    m_failedExperiments.Add(experimentVM);
                                    experimentVM.state = MonitoredExperimentViewModel.ExperimentState.ERROR;
                                }
                            }
                        }
                        else
                        {
                            if (experimentId == XMLStream.m_defaultMessageType)
                            {
                                //if (content == CJobDispatcher.m_endMessage)
                                {
                                    //job results can be expected to be sent back even if some of the tasks failed
                                    logMessage("Receiving job results");
                                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.RECEIVING);
                                    m_herdAgent.status = "Receiving output files";
                                    bool bret= await m_shepherd.ReceiveJobResult(m_cancelToken);
                                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.FINISHED);
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
                m_shepherd.readAsync(new CancellationToken()); //we synchronously wait until we get the ack from the client
                
                m_monitoredExperiments.ForEach((exp) => { exp.resetState(); });
                m_herdAgent.status = "";
            }
            catch (Exception ex)
            {
                //to do: aqui salta cuando hay cualquier problema. Si hay problema hay que volver a lanzarlo
                //mandar a cualquier maquina que este libre
                logMessage("Unhandled exception in Badger.sendJobAndMonitor(). Agent " + m_herdAgent.ipAddress);
                m_failedExperiments.Clear();
                foreach (MonitoredExperimentViewModel exp in m_monitoredExperiments) m_failedExperiments.Add(exp);
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
    public class ExperimentQueueMonitorViewModel : PropertyChangedBase
    {
        private List<HerdAgentViewModel> m_herdAgentList;
        private ObservableCollection<MonitoredExperimentViewModel> m_monitoredExperimentBatchList
            = new ObservableCollection<MonitoredExperimentViewModel>();
        public ObservableCollection<MonitoredExperimentViewModel> monitoredExperimentBatchList
            { get { return m_monitoredExperimentBatchList; } }
        private List<MonitoredExperimentViewModel> m_pendingExperiments
            = new List<MonitoredExperimentViewModel>();

        private CancellationTokenSource m_cancelTokenSource;

        //log stuff: a delegate log function must be passed via setLogFunction
       
        private Logger.LogFunction logFunction= null;
        private PlotViewModel m_evaluationMonitor;


        public ExperimentQueueMonitorViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<ExperimentViewModel> pendingExperiments, PlotViewModel evaluationMonitor
            , Logger.LogFunction logFunctionDelegate)
        {
            m_evaluationMonitor = evaluationMonitor;
            m_herdAgentList = freeHerdAgents;
            logFunction = logFunctionDelegate;
            foreach (ExperimentViewModel exp in pendingExperiments)
            {
                MonitoredExperimentViewModel monitoredExperiment= new MonitoredExperimentViewModel(exp,evaluationMonitor);
                m_monitoredExperimentBatchList.Add(monitoredExperiment);
                m_pendingExperiments.Add(monitoredExperiment);
            }
            NotifyOfPropertyChange(() => monitoredExperimentBatchList);
        }

        private bool m_bRunningExperiments = false;
        public async void runExperimentsAsync(string batchName, bool monitorProgress, bool receiveJobResults)
        {
            m_bRunningExperiments = true;
            m_cancelTokenSource = new CancellationTokenSource();

            List<Task<ExperimentBatch>> experimentBatchTaskList= new List<Task<ExperimentBatch>>();
            List<ExperimentBatch> experimentBatchList= new List<ExperimentBatch>();

            //assign experiments to free agents
            assignExperiments(ref m_pendingExperiments, ref m_herdAgentList, ref experimentBatchList
                , m_cancelTokenSource.Token, logFunction);
            try
            {
                while ((experimentBatchList.Count > 0 || experimentBatchTaskList.Count > 0
                    || m_pendingExperiments.Count > 0)
                    && !m_cancelTokenSource.IsCancellationRequested)
                {
                    foreach (ExperimentBatch batch in experimentBatchList)
                    {
                        experimentBatchTaskList.Add(batch.sendJobAndMonitor(batchName));
                    }
                    //all pending experiments sent? then we await completion to retry in case something fails
                    if (m_pendingExperiments.Count == 0)
                    {
                        Task.WhenAll(experimentBatchTaskList).Wait();
                        logFunction("All the experiments have finished");
                        break;
                    }

                    //wait for the first agent to finish and give it something to do
                    Task<ExperimentBatch> finishedTask = await Task.WhenAny(experimentBatchTaskList);
                    ExperimentBatch finishedTaskResult = await finishedTask;
                    logFunction("Job finished: " + finishedTaskResult.ToString());
                    experimentBatchTaskList.Remove(finishedTask);

                    if (finishedTaskResult.failedExperiments.Count > 0)
                    {
                        foreach (MonitoredExperimentViewModel exp in finishedTaskResult.failedExperiments)
                            m_pendingExperiments.Add(exp);
                        logFunction(finishedTaskResult.failedExperiments.Count + " failed experiments enqueued again for further trials");
                    }

                    //just in case the freed agent hasn't still been discovered by the shepherd
                    if (!m_herdAgentList.Contains(finishedTaskResult.herdAgent))
                        m_herdAgentList.Add(finishedTaskResult.herdAgent);

                    //assign experiments to free agents
                    assignExperiments(ref m_pendingExperiments, ref m_herdAgentList, ref experimentBatchList
                        ,m_cancelTokenSource.Token, logFunction);
                }

            }
            catch (Exception ex)
            {
                logFunction("Exception in runExperimentQueueRemotely()");
                logFunction(ex.StackTrace);
            }
            finally
            {
                m_bRunningExperiments = false;
                m_cancelTokenSource.Dispose();
            }
        }

        public void stopExperiments()
        {
            if (m_bRunningExperiments && m_cancelTokenSource != null)
                m_cancelTokenSource.Cancel();
        }
        private int batchId = 0;
        public void assignExperiments(ref List<MonitoredExperimentViewModel> pendingExperiments
            , ref List<HerdAgentViewModel> freeHerdAgents
            , ref  List<ExperimentBatch> experimentAssignments
            , CancellationToken cancelToken, Logger.LogFunction logFunction = null)
        {
            experimentAssignments.Clear();
            List<MonitoredExperimentViewModel> experimentBatch;
            while (pendingExperiments.Count > 0 && freeHerdAgents.Count > 0)
            {
                HerdAgentViewModel agentVM = freeHerdAgents[0];
                freeHerdAgents.RemoveAt(0);
                //usedHerdAgents.Add(agentVM);
                int numProcessors = Math.Max(1, agentVM.numProcessors - 1); //we free one processor

                experimentBatch = new List<MonitoredExperimentViewModel>();
                int numPendingExperiments = pendingExperiments.Count;
                for (int i = 0; i < Math.Min(numProcessors, numPendingExperiments); i++)
                {
                    experimentBatch.Add(pendingExperiments[0]);
                    pendingExperiments.RemoveAt(0);
                }
                experimentAssignments.Add(new ExperimentBatch("batch-" + batchId, experimentBatch, agentVM,m_evaluationMonitor
                    , cancelToken, logFunction));
                ++batchId;
            }
        }
    }
}
