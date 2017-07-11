using System.Collections.ObjectModel;
using Caliburn.Micro;
using System.Collections.Generic;
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Linq;
using System.Threading;
using System.Globalization;
using System.IO;
using System.Timers;
using System.Windows.Markup;
using System.Xml;
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

        private List<MonitoredExperimentViewModel> m_failedExperiments = new List<MonitoredExperimentViewModel>();
        public List<MonitoredExperimentViewModel> failedExperiments { get { return m_failedExperiments; } set { } }

        private Logger.LogFunction m_logFunction = null;
        private void logMessage(string message)
        {
            m_logFunction?.Invoke(message);
        }

        public ExperimentBatch(string name, List<MonitoredExperimentViewModel> experiments, HerdAgentViewModel herdAgent,
            PlotViewModel evaluationPlot, CancellationToken cancelToken, Logger.LogFunction logFunction)
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

            // tasks, inputs and outputs
            foreach (MonitoredExperimentViewModel experiment in m_monitoredExperiments)
            {
                CTask task = new CTask();
                // We are assuming the same exe file is used in all the experiments!!!
                // IMPORTANT

                task.name = experiment.Name;
                task.exe = experiment.ExeFile;
                task.arguments = experiment.FilePath + " -pipe=" + experiment.PipeName;
                task.pipe = experiment.PipeName;

                job.tasks.Add(task);
                // add EXE files

                if (!job.inputFiles.Contains(task.exe))
                    job.inputFiles.Add(task.exe);

                //add rename rules
                job.renameRules = experiment.RenameRules;

                //add prerrequisites
                foreach (string pre in experiment.Prerequisites)
                    if (!job.inputFiles.Contains(pre))
                        job.inputFiles.Add(pre);

                //add experiment file to inputs
                if (!job.inputFiles.Contains(experiment.FilePath))
                    job.inputFiles.Add(experiment.FilePath);

                Utility.getInputsAndOutputs(experiment.ExeFile, experiment.FilePath, ref job);
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

                bool bConnected = m_shepherd.connectToHerdAgent(m_herdAgent.IpAddress);
                if (bConnected)
                {
                    logMessage("Sending job to herd agent " + m_herdAgent.IpAddress);
                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.SENDING);
                    m_herdAgent.Status = "Sending job query";
                    m_shepherd.SendJobQuery(job, m_cancelToken);
                    logMessage("Job sent to herd agent " + m_herdAgent.IpAddress);
                    //await m_shepherd.waitAsyncWriteOpsToFinish();
                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.RUNNING);
                    m_herdAgent.Status = "Executing job query";
                }
                else
                {
                    foreach (MonitoredExperimentViewModel exp in m_monitoredExperiments) m_failedExperiments.Add(exp);
                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.ERROR);
                    logMessage("Failed to connect to herd agent " + m_herdAgent.IpAddress);
                    return this;
                }
                logMessage("Monitoring remote job run by herd agent " + m_herdAgent.IpAddress);
                // Monitor the remote job
                while (true)
                {
                    int numBytesRead = await m_shepherd.readAsync(m_cancelToken);
                    m_cancelToken.ThrowIfCancellationRequested();

                    string xmlItem = m_shepherd.m_xmlStream.processNextXMLItem();

                    while (xmlItem != "")
                    {
                        string experimentId = m_shepherd.m_xmlStream.getLastXMLItemTag();
                        string message = m_shepherd.m_xmlStream.getLastXMLItemContent();
                        MonitoredExperimentViewModel experimentVM = m_monitoredExperiments.Find(exp => exp.Name == experimentId);
                        string messageId = m_shepherd.m_xmlStream.getLastXMLItemTag(); //previous call to getLastXMLItemContent reset lastXMLItem
                        string messageContent = m_shepherd.m_xmlStream.getLastXMLItemContent();
                        if (experimentVM != null)
                        {
                            if (messageId == "Progress")
                            {
                                double progress = double.Parse(messageContent, CultureInfo.InvariantCulture);
                                experimentVM.Progress = Convert.ToInt32(progress);
                            }
                            else if (messageId == "Evaluation")
                            {
                                //<Evaluation>0.0,-1.23</Evaluation>
                                string[] values = messageContent.Split(',');
                                string seriesName = experimentVM.Name;
                                int seriesId;
                                if (values.Length == 2)
                                {
                                    if (!m_experimentSeriesId.Keys.Contains(experimentVM.Name))
                                    {
                                        seriesId = m_evaluationPlot.addLineSeries(seriesName);
                                        m_experimentSeriesId.Add(seriesName, seriesId);
                                    }
                                    else seriesId = m_experimentSeriesId[seriesName];

                                    m_evaluationPlot.addLineSeriesValue(seriesId, double.Parse(values[0], CultureInfo.InvariantCulture)
                                        , double.Parse(values[1], CultureInfo.InvariantCulture));
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
                                    m_herdAgent.Status = "Receiving output files";
                                    bool bret = await m_shepherd.ReceiveJobResult(m_cancelToken);
                                    m_monitoredExperiments.ForEach((exp) => exp.state = MonitoredExperimentViewModel.ExperimentState.FINISHED);
                                    m_herdAgent.Status = "Finished";
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
                m_herdAgent.Status = "";
            }
            catch (Exception ex)
            {
                logMessage("Unhandled exception in Badger.sendJobAndMonitor(). Agent " + m_herdAgent.IpAddress);
                logMessage(ex.ToString());
                m_failedExperiments.Clear();
                foreach (MonitoredExperimentViewModel exp in m_monitoredExperiments) m_failedExperiments.Add(exp);
                Console.WriteLine(ex.StackTrace);
            }
            finally
            {
                logMessage("Disconnected from herd agent " + m_herdAgent.IpAddress);
                m_shepherd.disconnect();
            }
            return this;
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public class ExperimentQueueMonitorViewModel : PropertyChangedBase
    {
        // Update time estimation according to porgress every half second
        private const int m_globalProgressUpdateRate = 15;
        // Intial value set to 20 in order to make an initial estimation after 10s (15 - 5 = 10)
        private int m_lastProgressUpdate = 5;

        private System.Timers.Timer m_timer;
        private bool m_bRunning;

        public bool bRunning
        {
            get { return m_bRunning; }
            set { m_bRunning = value; NotifyOfPropertyChange(() => bRunning); }
        }

        private bool m_bFinished;
        public bool bFinished
        {
            get { return m_bFinished; }
            set { m_bFinished = value; NotifyOfPropertyChange(() => bFinished); }
        }

        private List<HerdAgentViewModel> m_herdAgentList;
        private List<MonitoredExperimentViewModel> m_pendingExperiments = new List<MonitoredExperimentViewModel>();

        public ObservableCollection<MonitoredExperimentViewModel> MonitoredExperimentList { get; set; }

        private CancellationTokenSource m_cancelTokenSource;

        //log stuff: a delegate log function must be passed via setLogFunction
        private Logger.LogFunction logFunction = null;
        private PlotViewModel m_evaluationMonitor;

        private string m_estimatedEndTimeText = "";
        public string EstimatedEndTime
        {
            get { return m_estimatedEndTimeText; }
            set
            {
                m_estimatedEndTimeText = value;
                NotifyOfPropertyChange(() => EstimatedEndTime);
            }
        }

        private int m_timeRemaining;

        public int TimeRemaining
        {
            get { return m_timeRemaining; }
            set
            {
                m_timeRemaining = value;
                NotifyOfPropertyChange(() => TimeRemaining);
            }
        }

        private double m_globalProgress;
        public double GlobalProgress
        {
            get { return m_globalProgress; }
            set
            {
                m_globalProgress = value;
                NotifyOfPropertyChange(() => GlobalProgress);
            }
        }

        public Stopwatch ExperimentTimer { get; set; }

        private BindableCollection<LoggedExperimentViewModel> m_loggedExperiments
            = new BindableCollection<LoggedExperimentViewModel>();
        public BindableCollection<LoggedExperimentViewModel> LoggedExperiments
        {
            get { return m_loggedExperiments; }
            set { m_loggedExperiments = value; NotifyOfPropertyChange(() => LoggedExperiments); }
        }

        private void LoadLoggedExperiment(XmlNode node, string baseDirectory)
        {
            LoggedExperimentViewModel newExperiment = new LoggedExperimentViewModel(node,baseDirectory, false);
            LoggedExperiments.Add(newExperiment);
        }

        /// <summary>
        ///     Class constructor.
        /// </summary>
        /// <param name="freeHerdAgents"></param>
        /// <param name="evaluationMonitor"></param>
        /// <param name="logFunctionDelegate"></param>
        public ExperimentQueueMonitorViewModel(List<HerdAgentViewModel> freeHerdAgents,
            PlotViewModel evaluationMonitor, Logger.LogFunction logFunctionDelegate)
        {
            m_bRunning = false;
            m_evaluationMonitor = evaluationMonitor;
            m_herdAgentList = freeHerdAgents;
            logFunction = logFunctionDelegate;
            ExperimentTimer = new Stopwatch();
        }

        /// <summary>
        ///     Initializes the experiments to be monitored through a batch file that
        ///     contains all required data for the task.
        /// </summary>
        /// <param name="batchFileName">The batch file with experiment data</param>
        public bool InitializeExperiments(string batchFileName)
        {
            SimionFileData.LoadExperimentBatchFile(LoadLoggedExperiment, batchFileName);

            MonitoredExperimentList = new ObservableCollection<MonitoredExperimentViewModel>();
            Dictionary<string, string> renameRules = new Dictionary<string, string>();

            foreach (var experiment in LoggedExperiments)
            {
                if (!ExistsRequiredFile(experiment.ExeFile))
                    return false;

                List<string> prerequisites = new List<string>();

                foreach (var prerequisite in experiment.Prerequisites)
                {
                    if (!ExistsRequiredFile(prerequisite.Value))
                        return false;

                    prerequisites.Add(prerequisite.Value);
                    if (prerequisite.Rename != null)
                        renameRules[prerequisite.Value] = prerequisite.Rename;
                }

                foreach (var unit in experiment.ExperimentalUnits)
                {
                    MonitoredExperimentViewModel monitoredExperiment =
                    new MonitoredExperimentViewModel(unit, experiment.ExeFile, prerequisites, renameRules, m_evaluationMonitor);
                    MonitoredExperimentList.Add(monitoredExperiment);
                    m_pendingExperiments.Add(monitoredExperiment);
                }
            }

            NotifyOfPropertyChange(() => MonitoredExperimentList);
            return true;
        }

        /// <summary>
        ///     Check whether a required file to run an experiment exits or not.
        /// </summary>
        /// <param name="file">The file to check</param>
        /// <returns>A boolean value indicating the existance of the file</returns>
        public bool ExistsRequiredFile(string file)
        {
            if (!File.Exists(file))
            {
                CaliburnUtility.ShowWarningDialog("Unable to find " + file + "."
                    + " Experiment cannot run without this file.", "File not found");
                return false;
            }

            return true;
        }

        /// <summary>
        ///     Calculate the global progress of experiments in queue.
        /// </summary>
        /// <returns>The progress as a percentage.</returns>
        public double CalculateGlobalProgress()
        {
            double sum = 0.0;
            foreach (MonitoredExperimentViewModel exp in MonitoredExperimentList)
                sum += exp.Progress; //<- these are expressed as percentages

            return 100 * (sum / (MonitoredExperimentList.Count * 100));
        }

        /// <summary>
        ///     Express progress as a percentage unit to fill the global progress bar.
        /// </summary>
        public void UpdateGlobalProgress()
        {
            // Recalculate global progress each time
            GlobalProgress = CalculateGlobalProgress();
            // Then update the estimated time to end
            m_timeRemaining = (int)(ExperimentTimer.Elapsed.TotalSeconds
                * ((100 - m_globalProgress) / m_globalProgress));
            EstimatedEndTime = "Time remaining: "
                + TimeSpan.FromSeconds(m_timeRemaining).ToString(@"hh\:mm\:ss");
        }

        /// <summary>
        ///     Specify what is going to happen when the Elapsed event is raised.
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            if (m_globalProgress >= 0.0 && m_globalProgress < 100.0 && m_timeRemaining >= 0)
            {
                m_timeRemaining = m_timeRemaining - 1;
                EstimatedEndTime = "Time remaining: "
                    + TimeSpan.FromSeconds(m_timeRemaining).ToString(@"hh\:mm\:ss");
            }

            m_lastProgressUpdate++;

            if (m_lastProgressUpdate > m_globalProgressUpdateRate)
            {
                UpdateGlobalProgress();
                m_lastProgressUpdate = 0;
            }
        }

        public async void RunExperimentsAsync(bool monitorProgress, bool receiveJobResults)
        {
            m_timer = new System.Timers.Timer(1000);
            m_timer.Elapsed += OnTimedEvent;
            m_timer.Enabled = true;

            bRunning = true;
            m_cancelTokenSource = new CancellationTokenSource();

            List<Task<ExperimentBatch>> experimentBatchTaskList = new List<Task<ExperimentBatch>>();
            List<ExperimentBatch> experimentBatchList = new List<ExperimentBatch>();

            // Assign experiments to free agents
            AssignExperiments(ref m_pendingExperiments, ref m_herdAgentList, ref experimentBatchList,
                m_cancelTokenSource.Token, logFunction);
            try
            {
                while ((experimentBatchList.Count > 0 || experimentBatchTaskList.Count > 0
                    || m_pendingExperiments.Count > 0) && !m_cancelTokenSource.IsCancellationRequested)
                {
                    foreach (ExperimentBatch batch in experimentBatchList)
                        experimentBatchTaskList.Add(batch.sendJobAndMonitor());

                    // All pending experiments sent? Then we await completion to retry in case something fails
                    if (m_pendingExperiments.Count == 0)
                    {
                        Task.WhenAll(experimentBatchTaskList).Wait();
                        logFunction("All the experiments have finished");
                        break;
                    }

                    // Wait for the first agent to finish and give it something to do
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

                    // Just in case the freed agent hasn't still been discovered by the shepherd
                    if (!m_herdAgentList.Contains(finishedTaskResult.herdAgent))
                        m_herdAgentList.Add(finishedTaskResult.herdAgent);

                    // Assign experiments to free agents
                    AssignExperiments(ref m_pendingExperiments, ref m_herdAgentList, ref experimentBatchList,
                        m_cancelTokenSource.Token, logFunction);
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

                bRunning = false;
                m_cancelTokenSource.Dispose();
            }
        }

        /// <summary>
        ///     Stops all experiments in progress.
        /// </summary>
        public void StopExperiments()
        {
            if (m_bRunning && m_cancelTokenSource != null)
                m_cancelTokenSource.Cancel();
        }

        /// <summary>
        ///     Assigns experiments to availables herd agents.
        /// </summary>
        /// <param name="pendingExperiments"></param>
        /// <param name="freeHerdAgents"></param>
        /// <param name="experimentAssignments"></param>
        /// <param name="cancelToken"></param>
        /// <param name="logFunction"></param>
        public void AssignExperiments(ref List<MonitoredExperimentViewModel> pendingExperiments,
            ref List<HerdAgentViewModel> freeHerdAgents, ref List<ExperimentBatch> experimentAssignments,
            CancellationToken cancelToken, Logger.LogFunction logFunction = null)
        {
            experimentAssignments.Clear();
            int batchId = 0;

            while (pendingExperiments.Count > 0 && freeHerdAgents.Count > 0)
            {
                HerdAgentViewModel herdAgent = freeHerdAgents[0];
                freeHerdAgents.RemoveAt(0);

                int numProcessors = Math.Max(1, herdAgent.NumProcessors - 1); // Let's free one processor

                List<MonitoredExperimentViewModel> experiments = new List<MonitoredExperimentViewModel>();
                int len = Math.Min(numProcessors, pendingExperiments.Count);

                for (int i = 0; i < len; i++)
                {
                    experiments.Add(pendingExperiments[0]);
                    pendingExperiments.RemoveAt(0);
                }

                experimentAssignments.Add(new ExperimentBatch("batch-" + batchId, experiments,
                    herdAgent, m_evaluationMonitor, cancelToken, logFunction));
                ++batchId;
            }
        }
    }
}
