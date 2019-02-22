/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using System.Collections.Generic;
using System.Threading.Tasks;
using System.Threading;
using System.Diagnostics;
using System.Xml;
using System.IO;
using System;
using System.Timers;

using Badger.Data;

using Herd.Files;

using Caliburn.Micro;
using Herd.Network;

namespace Badger.ViewModels
{
    public class MonitorWindowViewModel : Screen
    {
        public PlotViewModel Plot { get; set; }

        public ShepherdViewModel ShepherdViewModel { get; } = new ShepherdViewModel();
        /// <summary>
        ///     Constructor.
        /// </summary>
        /// <param name="MainWindowViewModel.Instance.LogToFile"></param>
        /// <param name="batchFileName"></param>
        public MonitorWindowViewModel()
        {
            Plot = new PlotViewModel("", "", "")
            { bShowOptions = false };
            Plot.Properties.LegendVisible = false;

            //set timers:
            //This first timer updates each 2s the progress bar when an experiment is running
            m_progressUpdateTimer = new System.Timers.Timer(2000);
            m_progressUpdateTimer.Elapsed += ProgressUpdateEvent;
            //This second timer checks every 5s the connection with each herd agent running experiments
            m_connectionStateTimer = new System.Timers.Timer(5000);
            m_connectionStateTimer.Elapsed += ConnectionCheckEvent;
        }

        ///<summary>
        ///Deletes all the log files in the batch
        ///</summary>
        public void CleanExperimentBatch()
        {
            int numFilesDeleted = ExperimentBatch.DeleteLogFiles(BatchFileName);

            //re-load the batch for execution
            LoadExperimentBatch(BatchFileName);
        }



        /// <summary>
        ///     Runs the selected experiment in the experiment editor.
        /// </summary>
        public void RunExperimentBatch()
        {
            List<HerdAgentInfo> freeHerdAgents = new List<HerdAgentInfo>();

            // Get available herd agents list. Inside the loop to update the list
            ShepherdViewModel.GetAvailableHerdAgents(ref freeHerdAgents);

            if (freeHerdAgents.Count == 0)
            {
                CaliburnUtility.ShowWarningDialog(
                    "There is no herd agents availables at this moment. Make sure you have selected at " +
                    "least one available agent and try again.", "No agents detected");
                return;
            }

            IsRunning = true;
            ExperimentTimer.Start();
            Task.Run(() => RunExperimentsAsync(freeHerdAgents));
            IsRunning = false;
        }


        /// <summary>
        ///     Shows a Report window with the data of the currently finished experiment(s)
        ///     already load and ready to make reports.
        /// </summary>
        public void ShowReports()
        {
            MainWindowViewModel.Instance.ShowReportWindow();
            MainWindowViewModel.Instance.ReportWindowVM.LoadExperimentBatch(BatchFileName);
        }

        /// <summary>
        ///     Stops all experiments on Experiment Monitor window close.
        /// </summary>
        /// <param name="close"></param>
        protected override void OnDeactivate(bool close)
        {
            if (close)
                StopExperiments();
            base.OnDeactivate(close);
        }

        private System.Timers.Timer m_progressUpdateTimer;
        private System.Timers.Timer m_connectionStateTimer;

        public bool IsBatchLoadedAndNotRunning
        {
            get { return IsBatchLoaded && !IsRunning; }
        }

        private bool m_bBatchLoaded = false;
        public bool IsBatchLoaded
        {
            get { return m_bBatchLoaded; }
            set
            {
                m_bBatchLoaded = value;
                NotifyOfPropertyChange(() => IsBatchLoaded);
                NotifyOfPropertyChange(() => IsBatchLoadedAndNotRunning);
            }
        }

        private bool m_bRunning = false;
        public bool IsRunning
        {
            get { return m_bRunning; }
            set
            {
                m_bRunning = value;
                NotifyOfPropertyChange(() => IsRunning);
                NotifyOfPropertyChange(() => IsNotRunning);
                NotifyOfPropertyChange(() => IsBatchLoadedAndNotRunning);
            }
        }
        public bool IsNotRunning
        {
            get { return !IsRunning; }
        }

        private bool m_bFinished;
        public bool IsFinished
        {
            get { return m_bFinished; }
            set { m_bFinished = value; NotifyOfPropertyChange(() => IsFinished); }
        }

        private List<ExperimentalUnit> m_pendingExperiments = new List<ExperimentalUnit>();
        private CancellationTokenSource m_cancelTokenSource;


        private string m_estimatedEndTimeText = "N/A";
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

        public Stopwatch ExperimentTimer { get; set; } = new Stopwatch();

        private BindableCollection<LoggedExperimentViewModel> m_loggedExperiments
            = new BindableCollection<LoggedExperimentViewModel>();
        public BindableCollection<LoggedExperimentViewModel> LoggedExperiments
        {
            get { return m_loggedExperiments; }
            set { m_loggedExperiments = value; NotifyOfPropertyChange(() => LoggedExperiments); }
        }

        public void SelectExperimentBatchFile()
        {
            string fileName = null;
            bool isOpen = Files.OpenFileDialog(ref fileName, Files.ExperimentBatchDescription
                , Files.ExperimentBatchFilter);
            if (!isOpen) return;
            LoadExperimentBatch(fileName);
        }

        string m_batchFileName;
        public string BatchFileName
        {
            get { return m_batchFileName; }
            set
            {
                m_batchFileName = value;
                NotifyOfPropertyChange(() => BatchFileName);
            }
        }

        int m_numExperimentalUnits = 0;
        public int NumExperimentalUnits
        {
            get { return m_numExperimentalUnits; }
            set { m_numExperimentalUnits = value; NotifyOfPropertyChange(() => NumExperimentalUnits); }
        }

        int m_numFinishedExperimentalUnitsBeforeStart = 0;
        public int NumFinishedExperimentalUnitsBeforeStart
        {
            get { return m_numFinishedExperimentalUnitsBeforeStart; }
            set
            {
                m_numFinishedExperimentalUnitsBeforeStart = value;
                NotifyOfPropertyChange(() => NumFinishedExperimentalUnits);

                GlobalProgress = CalculateGlobalProgress();
            }
        }

        int m_numFinishedExperimentalUnitsAfterStart = 0;
        public int NumFinishedExperimentalUnitsAfterStart
        {
            get { return m_numFinishedExperimentalUnitsAfterStart; }
            set
            {
                m_numFinishedExperimentalUnitsAfterStart = value;
                NotifyOfPropertyChange(() => NumFinishedExperimentalUnitsAfterStart);
                NotifyOfPropertyChange(() => NumFinishedExperimentalUnits);
                GlobalProgress = CalculateGlobalProgress();
            }
        }

        public int NumFinishedExperimentalUnits { get { return NumFinishedExperimentalUnitsAfterStart + NumFinishedExperimentalUnitsBeforeStart; } }

        /// <summary>
        ///     Initializes the experiments to be monitored through a batch file that
        ///     contains all required data for the task.
        /// </summary>
        /// <param name="batchFileName">The batch file with experiment data</param>
        public bool LoadExperimentBatch(string batchFileName)
        {
            //Load unfinished experiments
            LoadOptions loadOptionsUnfinished = new LoadOptions()
            {
                Selection = LoadOptions.ExpUnitSelection.OnlyUnfinished,
                LoadVariablesInLog = false
            };

            ExperimentBatch batchUnfinished = new ExperimentBatch();
            batchUnfinished.Load(batchFileName, loadOptionsUnfinished);

            LoggedExperiments.Clear();
            foreach (Experiment experiment in batchUnfinished.Experiments)
            {
                LoggedExperimentViewModel newExperiment = new LoggedExperimentViewModel(experiment);
                LoggedExperiments.Add(newExperiment);
            }
            //count unfinished experiments
            int numUnfinishedExperimentalUnits = batchUnfinished.CountExperimentalUnits();

            //load all experimental units and count them
            LoadOptions loadOptionsFinished = new LoadOptions()
            {
                Selection = LoadOptions.ExpUnitSelection.OnlyFinished,
                LoadVariablesInLog = false
            };

            //we use a temp variable first to get the count of finished units so that setting the value of NumFinishedExperimentalUnitsBeforeStart triggers the progress update
            //AFTER setting the value of NumExperimentalUnits
            NumFinishedExperimentalUnitsBeforeStart = ExperimentBatch.CountExperimentalUnits(batchFileName, LoadOptions.ExpUnitSelection.OnlyFinished);
            NumExperimentalUnits = numUnfinishedExperimentalUnits + NumFinishedExperimentalUnitsBeforeStart;
            NumFinishedExperimentalUnitsAfterStart = 0;

            //Initialize pending experiment list
            m_pendingExperiments.Clear();

            foreach (var experiment in LoggedExperiments)
            {
                foreach (AppVersion version in experiment.AppVersions)
                {
                    if (!ExistsRequiredFile(version.ExeFile))
                    {
                        CaliburnUtility.ShowWarningDialog("Cannot find required file: " + version.ExeFile + ". Check the app definition file in /config/apps", "ERROR");
                        return false;
                    }
                    foreach (string pre in version.Requirements.InputFiles)
                    {
                        if (!ExistsRequiredFile(pre))
                        {
                            CaliburnUtility.ShowWarningDialog("Cannot find required file: " + pre + ". Check the app definition file in /config/apps", "ERROR");
                            return false;
                        }
                    }
                }
                foreach (LoggedExperimentalUnitViewModel unit in experiment.ExperimentalUnits) m_pendingExperiments.Add(unit.Model);
            }

            BatchFileName = batchFileName;
            Plot.ClearLineSeries();
            AllMonitoredJobs.Clear();
            IsBatchLoaded = true;
            IsRunning = false;

            return true;
        }

        readonly Dictionary<Job, MonitoredJobViewModel> ViewModelFromModel = new Dictionary<Job, MonitoredJobViewModel>();

        public void DispatchOnMessageReceived(Job job, string experimentId, string messageId, string messageContent)
        {
            MonitoredJobViewModel jobVM = ViewModelFromModel[job];
            jobVM.OnMessageReceived(experimentId, messageId, messageContent);
        }

        public void DispatchOnStateChanged(Job job, string experimentId, Monitoring.State state)
        {
            MonitoredJobViewModel jobVM = ViewModelFromModel[job];
            jobVM.OnStateChanged(experimentId, state);
        }

        public void DispatchOnAllStatesChanged(Job job, Monitoring.State state)
        {
            MonitoredJobViewModel jobVM = ViewModelFromModel[job];
            jobVM.OnAllStatesChanged(state);
        }

        public void DispatchOnExperimentalUnitLaunched(Job job, ExperimentalUnit expUnit)
        {
            MonitoredJobViewModel jobVM = ViewModelFromModel[job];
            jobVM.OnExperimentalUnitLaunched(expUnit);
        }

        public void OnJobAssigned(Job job)
        {
            MonitoredJobViewModel monitoredJob
                            = new MonitoredJobViewModel(job, Plot, m_cancelTokenSource.Token, MainWindowViewModel.Instance.LogToFile);
            AllMonitoredJobs.Insert(0, monitoredJob);
            ViewModelFromModel[job] = monitoredJob;
        }

        public void OnJobFinished(Job job)
        {
            NumFinishedExperimentalUnitsAfterStart += job.ExperimentalUnits.Count
            - job.FailedExperimentalUnits.Count;
        }

        void SetRunning(bool running)
        {
            m_progressUpdateTimer.Enabled = running;
            m_connectionStateTimer.Enabled = running;

            IsRunning = running;
            IsFinished = !running;


            if (!running)
            {
                //We just disabled the timer that updates the connection status, so we have to change the icon manually for each job
                foreach (MonitoredJobViewModel job in AllMonitoredJobs)
                    job.ConnectionState.Icon = null;
            }
        }

        public async void RunExperimentsAsync(List<HerdAgentInfo> freeHerdAgents)
        {
            SetRunning(true);

            m_cancelTokenSource = new CancellationTokenSource();

            Monitoring.MsgDispatcher dispatcher
                = new Monitoring.MsgDispatcher( OnJobAssigned, OnJobFinished
                    , DispatchOnAllStatesChanged, DispatchOnStateChanged
                    , DispatchOnMessageReceived, DispatchOnExperimentalUnitLaunched
                    , MainWindowViewModel.Instance.LogToFile, m_cancelTokenSource.Token);

            int ret = await JobDispatcher.RunExperimentsAsync(m_pendingExperiments, freeHerdAgents, dispatcher, m_cancelTokenSource, ShepherdViewModel.DispatcherOptions);

            SetRunning(false);
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
        object m_globalProgressUpdateObj = new object();
        public double CalculateGlobalProgress()
        {
            lock (m_globalProgressUpdateObj)
            {
                double progress = 0.0;
                double sum = 0.0;
                if (NumExperimentalUnits > 0)
                {
                    //the experiment is running
                    foreach (MonitoredJobViewModel exp in AllMonitoredJobs)
                    {
                        if (exp.Running)
                            sum += exp.MonitoredExperimentalUnits.Count * exp.NormalizedProgress;
                    }

                    progress = (NumFinishedExperimentalUnitsAfterStart + NumFinishedExperimentalUnitsBeforeStart + sum) / (double)NumExperimentalUnits;
                }
                return progress;
            }

        }


        /// <summary>
        ///     Update the progress each time the timer generates an event
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void ProgressUpdateEvent(object source, ElapsedEventArgs e)
        {
            // Recalculate global progress each time
            GlobalProgress = CalculateGlobalProgress();
            // Then update the estimated time to end
            if (IsRunning)
            {
                m_timeRemaining = (int)(ExperimentTimer.Elapsed.TotalSeconds
                  * ((1 - m_globalProgress) / m_globalProgress));
                EstimatedEndTime = TimeSpan.FromSeconds(m_timeRemaining).ToString(@"hh\:mm\:ss");
            }
            else EstimatedEndTime = "N/A";
        }


        const int ConnectionTimeOutSeconds = 20;
        /// <summary>
        /// Checks how much time has passed since a message was received from the herd agent running the job and updates
        /// the icon accordingly
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void ConnectionCheckEvent(object source, ElapsedEventArgs e)
        {
            DateTime now = DateTime.Now;
            if (IsRunning)
            {
                foreach(MonitoredJobViewModel job in AllMonitoredJobs)
                {
                    if (job.Running)
                    {
                        if (now - job.LastHeartbeat > TimeSpan.FromSeconds(ConnectionTimeOutSeconds))
                            job.ConnectionState.Icon = MonitoredJobStateViewModel.ConnectionError;
                        else
                            job.ConnectionState.Icon = MonitoredJobStateViewModel.ConnectionOK;
                    }
                    else job.ConnectionState.Icon = null;
                }
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

        /// <summary>
        ///     Stops all experiments in progress.
        /// </summary>
        public void StopExperiments()
        {
            if (m_cancelTokenSource != null) m_cancelTokenSource.Dispose();

            SetRunning(false);

            LoadExperimentBatch(BatchFileName); //re-load the batch to check how many units were finished
        }
    }

}
