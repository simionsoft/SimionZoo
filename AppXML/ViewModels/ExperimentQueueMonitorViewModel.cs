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
    public class ExperimentQueueMonitorViewModel : PropertyChangedBase
    {
        private List<HerdAgentViewModel> m_herdAgentList;
        private ObservableCollection<ExperimentBatchMonitorViewModel> m_monitoredExperimentBatchList;
        public ObservableCollection<ExperimentBatchMonitorViewModel> monitoredExperimentBatchList
            { get { return m_monitoredExperimentBatchList; } }
        private List<ExperimentViewModel> m_pendingExperiments;

        private CancellationTokenSource m_cancelTokenSource;

        //log stuff: a delegate log function must be passed via setLogFunction
        public delegate void LogFunction(string message);
        private LogFunction m_logFunction= null;
        public void setLogFunction(LogFunction function){m_logFunction= function;}
        private void logFunction (string message) { if (m_logFunction!=null) m_logFunction(message);}

        public ExperimentQueueMonitorViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<ExperimentViewModel> pendingExperiments)
        {
            m_herdAgentList = freeHerdAgents;
            m_pendingExperiments = pendingExperiments;
            m_monitoredExperimentBatchList = new ObservableCollection<ExperimentBatchMonitorViewModel>();
            //foreach (ExperimentViewModel exp in pendingExperiments)
            //    m_monitoredExperimentList.Add(new ExperimentMonitorViewModel(exp));
        }

        public void runExperiments(string batchName, bool monitorProgress= true, bool receiveJobResults= true)
        {
            Task.Run(() => runExperimentsAsync(batchName,monitorProgress,receiveJobResults));
        }

        private async void runExperimentsAsync(string batchName, bool monitorProgress, bool receiveJobResults)
        {
            m_cancelTokenSource = new CancellationTokenSource();
            //List<ExperimentMonitorViewModel> experimentMonitorViewModels
            //    = new List<ExperimentMonitorViewModel>();
            List<Task<ExperimentBatchMonitorViewModel>> experimentMonitorViewModelTaskList
                = new List<Task<ExperimentBatchMonitorViewModel>>();

            //assign experiments to free agents
            assignExperiments(ref m_pendingExperiments, ref m_herdAgentList
                , m_cancelTokenSource.Token, logFunction);
            try
            {
                while ((m_monitoredExperimentBatchList.Count > 0 || experimentMonitorViewModelTaskList.Count > 0
                    || m_pendingExperiments.Count > 0)
                    && !m_cancelTokenSource.IsCancellationRequested)
                {
                    foreach (ExperimentBatchMonitorViewModel ExperimentMonitorViewModel in m_monitoredExperimentBatchList)
                    {
                        experimentMonitorViewModelTaskList.Add(ExperimentMonitorViewModel.sendJobAndMonitor(batchName));
                    }
                    //all pending experiments sent? then we await completion to retry in case something fails
                    if (m_pendingExperiments.Count == 0)
                    {
                        Task.WhenAll(experimentMonitorViewModelTaskList).Wait();
                        logFunction("All the experiments have finished");
                        break;
                    }

                    //wait for the first agent to finish and give it something to do
                    Task<ExperimentBatchMonitorViewModel> finishedTask = await Task.WhenAny(experimentMonitorViewModelTaskList);
                    ExperimentBatchMonitorViewModel finishedTaskResult = await finishedTask;
                    logFunction("Job finished: " + finishedTaskResult.ToString());
                    experimentMonitorViewModelTaskList.Remove(finishedTask);

                    if (finishedTaskResult.failedExperiments.Count > 0)
                    {
                        foreach (ExperimentViewModel exp in finishedTaskResult.failedExperiments)
                            m_pendingExperiments.Add(exp);
                        logFunction(finishedTaskResult.failedExperiments.Count + " failed experiments enqueued again for further trials");
                    }

                    //just in case the freed agent hasn't still been discovered by the shepherd
                    if (!m_herdAgentList.Contains(finishedTaskResult.herdAgent))
                        m_herdAgentList.Add(finishedTaskResult.herdAgent);

                    //assign experiments to free agents
                    assignExperiments(ref m_pendingExperiments, ref m_herdAgentList, m_cancelTokenSource.Token, logFunction);
                }

            }
            catch (Exception ex)
            {
                logFunction("Exception in runExperimentQueueRemotely()");
                logFunction(ex.StackTrace);
            }
            finally
            {
                m_cancelTokenSource.Dispose();
            }
        }

        public void stopExperiments()
        {
            if (m_cancelTokenSource != null)
                m_cancelTokenSource.Cancel();
        }

        public void assignExperiments(ref List<ExperimentViewModel> pendingExperiments
            , ref List<HerdAgentViewModel> freeHerdAgents
            , CancellationToken cancelToken, LogFunction logFunction = null)
        {
            bool bListModified = false;
            List<ExperimentViewModel> experimentBatch;
            while (pendingExperiments.Count > 0 && freeHerdAgents.Count > 0)
            {
                HerdAgentViewModel agentVM = freeHerdAgents[0];
                freeHerdAgents.RemoveAt(0);
                //usedHerdAgents.Add(agentVM);
                int numProcessors = Math.Max(1, agentVM.numProcessors - 1); //we free one processor

                experimentBatch = new List<ExperimentViewModel>();
                int numPendingExperiments = pendingExperiments.Count;
                for (int i = 0; i < Math.Min(numProcessors, numPendingExperiments); i++)
                {
                    experimentBatch.Add(pendingExperiments[0]);
                    pendingExperiments.RemoveAt(0);
                }
                m_monitoredExperimentBatchList.Add(new ExperimentBatchMonitorViewModel(agentVM,experimentBatch,cancelToken,logFunction));
                bListModified = true;
            }
            if (bListModified) NotifyOfPropertyChange(() => monitoredExperimentBatchList);
        }
    }
}
