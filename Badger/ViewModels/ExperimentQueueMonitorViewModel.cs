using System.Collections.ObjectModel;
using Caliburn.Micro;
using System.Collections.Generic;
using System;
using System.Threading.Tasks;
using System.Linq;
using System.Threading;
using System.Globalization;
using Herd;
using Badger.Data;


namespace Badger.ViewModels
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

            //tasks, inputs and outputs
            foreach (MonitoredExperimentViewModel experiment in m_monitoredExperiments)
            {
                CTask task = new CTask();
                //we are assuming the same exe file is used in all the experiments!!!
                //IMPORTANT
                task.name = experiment.name;
                task.exe = experiment.exeFile;
                task.arguments = experiment.filePath + " -pipe=" + experiment.pipeName;
                task.pipe = experiment.pipeName;
                job.tasks.Add(task);
                //add EXE files
                
                if (!job.inputFiles.Contains(task.exe))
                        job.inputFiles.Add(task.exe);
                //add prerrequisites
                foreach (string pre in experiment.prerrequisites)
                    if (!job.inputFiles.Contains(pre))
                        job.inputFiles.Add(pre);

                //add experiment file to inputs
                if (!job.inputFiles.Contains(experiment.filePath))
                    job.inputFiles.Add(experiment.filePath);

                Utility.getInputsAndOutputs(experiment.exeFile, experiment.filePath,ref job);
            }
            return job;
        }

        public async Task<ExperimentBatch> sendJobAndMonitor()
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
                                    //Right now, my view on adding failed experiments back to the pending exp. list:
                                    //Some experiments may fail because the parameters are just invalid (i.e. FAST)
                                    //Much more likely than a network-related error or some other user-related problem
                                    //m_failedExperiments.Add(experimentVM);
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
                await m_shepherd.readAsync(new CancellationToken()); //we synchronously wait until we get the ack from the client
                
                m_monitoredExperiments.ForEach((exp) => { exp.resetState(); });
                m_herdAgent.status = "";
            }
            catch (Exception ex)
            {
                logMessage("Unhandled exception in Badger.sendJobAndMonitor(). Agent " + m_herdAgent.ipAddress);
                logMessage(ex.ToString());
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

        public double calculateGlobalProgress()
        {
            double sum = 0.0;
            foreach(MonitoredExperimentViewModel exp in m_monitoredExperimentBatchList)
            {
                sum += exp.progress;
            }
            return sum / (m_monitoredExperimentBatchList.Count*100);
        }


        public ExperimentQueueMonitorViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<Experiment> experiments, PlotViewModel evaluationMonitor
            , Logger.LogFunction logFunctionDelegate,MonitorWindowViewModel parent)
        {
            m_evaluationMonitor = evaluationMonitor;
            m_herdAgentList = freeHerdAgents;
            logFunction = logFunctionDelegate;
            foreach (Experiment exp in experiments)
            {
                MonitoredExperimentViewModel monitoredExperiment= 
                    new MonitoredExperimentViewModel(exp,evaluationMonitor,parent);
                m_monitoredExperimentBatchList.Add(monitoredExperiment);
                m_pendingExperiments.Add(monitoredExperiment);
            }
            NotifyOfPropertyChange(() => monitoredExperimentBatchList);
        }

        private bool m_bRunning = false;
        private bool m_bFinished = false;
        public bool bFinished { get { return m_bFinished; }
            set { m_bFinished = value; NotifyOfPropertyChange(() => bFinished); } }

        public async void runExperimentsAsync(bool monitorProgress, bool receiveJobResults)
        {
            m_bRunning = true;
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
                        experimentBatchTaskList.Add(batch.sendJobAndMonitor());
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
                if (m_pendingExperiments.Count == 0)
                    bFinished = true; // used to enable the "View reports" button
                m_bRunning = false;
                m_cancelTokenSource.Dispose();
            }
        }

        public void stopExperiments()
        {
            if (m_bRunning && m_cancelTokenSource != null)
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
