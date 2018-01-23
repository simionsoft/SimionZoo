using System.Collections.ObjectModel;
using Caliburn.Micro;
using System.Collections.Generic;
using System;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Threading;
using System.IO;
using System.Timers;
using System.Xml;
using Herd;
using Badger.Data;
using Badger.Simion;

namespace Badger.ViewModels
{
   
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
        private List<MonitoredExperimentalUnitViewModel> m_pendingExperiments = new List<MonitoredExperimentalUnitViewModel>();

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

        private int LoadLoggedExperiment(XmlNode node, string baseDirectory
            , SimionFileData.LoadUpdateFunction loadUpdateFunction= null)
        {
            LoggedExperimentViewModel newExperiment
                = new LoggedExperimentViewModel(node, baseDirectory, false, loadUpdateFunction);
            LoggedExperiments.Add(newExperiment);
            return newExperiment.ExperimentalUnits.Count;
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

        int NumExperimentalUnits = 0;

        /// <summary>
        ///     Initializes the experiments to be monitored through a batch file that
        ///     contains all required data for the task.
        /// </summary>
        /// <param name="batchFileName">The batch file with experiment data</param>
        public bool InitializeExperiments(string batchFileName)
        {
            //Load the batch
            NumExperimentalUnits= SimionFileData.LoadExperimentBatchFile(batchFileName, LoadLoggedExperiment);

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
                    MonitoredExperimentalUnitViewModel monitoredExperiment =
                        new MonitoredExperimentalUnitViewModel(unit, experiment.ExeFile, prerequisites, renameRules, m_evaluationMonitor);
                    m_pendingExperiments.Add(monitoredExperiment);
                }
            }
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
            foreach (MonitoredJobViewModel exp in AllMonitoredJobs)
                sum += exp.MonitoredExperimentalUnits.Count * exp.NormalizedProgress;
            return sum / NumExperimentalUnits;
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
                * ((1 - m_globalProgress) / m_globalProgress));
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
            if (m_globalProgress >= 0.0 && m_globalProgress < 1.0 && m_timeRemaining >= 0)
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

        List <MonitoredJobViewModel> m_monitoredJobs = new List<MonitoredJobViewModel>();
        public List<MonitoredJobViewModel> MonitoredJobs
        {
            get { return m_monitoredJobs; }
            set
            {
                m_monitoredJobs = value;
                NotifyOfPropertyChange(() => MonitoredJobs);
            }
        }
        BindableCollection<MonitoredJobViewModel> m_allMonitoredJobs 
            = new BindableCollection<MonitoredJobViewModel>();
        public BindableCollection<MonitoredJobViewModel> AllMonitoredJobs
        {
            get { return m_allMonitoredJobs; }
            set
            {
                m_allMonitoredJobs = value;
                NotifyOfPropertyChange(() => AllMonitoredJobs);
            }
        }


        public async void RunExperimentsAsync(bool monitorProgress, bool receiveJobResults)
        {
            m_timer = new System.Timers.Timer(1000);
            m_timer.Elapsed += OnTimedEvent;
            m_timer.Enabled = true;

            bRunning = true;
            m_cancelTokenSource = new CancellationTokenSource();

            List<Task<MonitoredJobViewModel>> experimentBatchTaskList 
                = new List<Task<MonitoredJobViewModel>>();

            // Assign experiments to free agents
            AssignExperiments(ref m_pendingExperiments, ref m_herdAgentList, ref m_monitoredJobs,
                m_cancelTokenSource.Token, logFunction);
            try
            {
                while ((MonitoredJobs.Count > 0 || experimentBatchTaskList.Count > 0
                    || m_pendingExperiments.Count > 0) && !m_cancelTokenSource.IsCancellationRequested)
                {
                    foreach (MonitoredJobViewModel batch in MonitoredJobs)
                        experimentBatchTaskList.Add(batch.SendJobAndMonitor());

                    // All pending experiments sent? Then we await completion to retry in case something fails
                    if (m_pendingExperiments.Count == 0)
                    {
                        Task.WhenAll(experimentBatchTaskList).Wait();
                        logFunction("All the experiments have finished");
                        break;
                    }

                    // Wait for the first agent to finish and give it something to do
                    Task<MonitoredJobViewModel> finishedTask = await Task.WhenAny(experimentBatchTaskList);
                    MonitoredJobViewModel finishedTaskResult = await finishedTask;
                    logFunction("Job finished: " + finishedTaskResult.ToString());
                    experimentBatchTaskList.Remove(finishedTask);

                    if (finishedTaskResult.FailedExperiments.Count > 0)
                    {
                        foreach (MonitoredExperimentalUnitViewModel exp in finishedTaskResult.FailedExperiments)
                            m_pendingExperiments.Add(exp);
                        logFunction(finishedTaskResult.FailedExperiments.Count + " failed experiments enqueued again for further trials");
                    }

                    // Just in case the freed agent hasn't still been discovered by the shepherd
                    if (!m_herdAgentList.Contains(finishedTaskResult.HerdAgent))
                        m_herdAgentList.Add(finishedTaskResult.HerdAgent);

                    // Assign experiments to free agents
                    AssignExperiments(ref m_pendingExperiments, ref m_herdAgentList, ref m_monitoredJobs,
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

        //integer value incremented to generate job ids
        int jobId = 0;
        /// <summary>
        ///     Assigns experiments to availables herd agents.
        /// </summary>
        /// <param name="pendingExperiments"></param>
        /// <param name="freeHerdAgents"></param>
        /// <param name="experimentAssignments"></param>
        /// <param name="cancelToken"></param>
        /// <param name="logFunction"></param>
        /// 
        public void AssignExperiments(ref List<MonitoredExperimentalUnitViewModel> pendingExperiments,
            ref List<HerdAgentViewModel> freeHerdAgents, ref List<MonitoredJobViewModel> experimentAssignments,
            CancellationToken cancelToken, Logger.LogFunction logFunction = null)
        {
            experimentAssignments.Clear();


            while (pendingExperiments.Count > 0 && freeHerdAgents.Count > 0)
            {
                HerdAgentViewModel herdAgent = freeHerdAgents[0];
                freeHerdAgents.RemoveAt(0);

                int numProcessors = Math.Max(1, herdAgent.NumProcessors - 1); // Let's free one processor

                List<MonitoredExperimentalUnitViewModel> experiments = new List<MonitoredExperimentalUnitViewModel>();
                int len = Math.Min(numProcessors, pendingExperiments.Count);

                for (int i = 0; i < len; i++)
                {
                    experiments.Add(pendingExperiments[0]);
                    pendingExperiments.RemoveAt(0);
                }

                MonitoredJobViewModel newJob = new MonitoredJobViewModel("Job #" + jobId, experiments,
                    herdAgent, m_evaluationMonitor, cancelToken, logFunction);
                experimentAssignments.Add(newJob);
                AllMonitoredJobs.Insert(0,newJob);
                ++jobId;
            }
        }
    }
}
