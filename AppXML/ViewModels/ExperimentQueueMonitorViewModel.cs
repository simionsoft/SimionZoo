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


        private ExperimentQueueViewModel m_experimentQueueViewModel = new ExperimentQueueViewModel();
        public ExperimentQueueViewModel experimentQueueViewModel { get { return m_experimentQueueViewModel; }
            set { m_experimentQueueViewModel = value;
            NotifyOfPropertyChange(() => experimentQueueViewModel);
            }
        }
        private CancellationTokenSource m_cancelTokenSource;

        public delegate void LogFunction(string message);
        private LogFunction m_logFunction= null;
        public void setLogFunction(LogFunction function){m_logFunction= function;}
        private void logFunction (string message) { if (m_logFunction!=null) m_logFunction(message);}

        public ExperimentQueueMonitorViewModel(List<HerdAgentViewModel> freeHerdAgents, List<ExperimentViewModel> pendingExperiments)
        {
            Task.Run(() => runExperimentsAndMonitor(freeHerdAgents, pendingExperiments));
        }

        private async void runExperimentsAndMonitor(List<HerdAgentViewModel> freeHerdAgents, List<ExperimentViewModel> pendingExperiments)
        {
            m_cancelTokenSource = new CancellationTokenSource();
            List<RemoteJob> badgers = new List<RemoteJob>();
            List<Task<RemoteJob>> badgerTaskList = new List<Task<RemoteJob>>();
            //assign experiments to free agents
            RemoteJob.assignExperiments(ref pendingExperiments, ref freeHerdAgents
                , ref badgers, m_cancelTokenSource.Token, logFunction);
            try
            {
                while ((badgers.Count > 0 || badgerTaskList.Count > 0 || pendingExperiments.Count > 0)
                    && !m_cancelTokenSource.IsCancellationRequested)
                {
                    foreach (RemoteJob badger in badgers)
                    {
                        badgerTaskList.Add(badger.sendJobAndMonitor(experimentQueueViewModel.name));
                    }
                    //all pending experiments sent? then we await completion to retry in case something fails
                    if (pendingExperiments.Count == 0)
                    {
                        Task.WhenAll(badgerTaskList).Wait();
                        logFunction("All the experiments have finished");
                        break;
                    }

                    //wait for the first agent to finish and give it something to do
                    Task<RemoteJob> finishedTask = await Task.WhenAny(badgerTaskList);
                    RemoteJob finishedTaskResult = await finishedTask;
                    logFunction("Job finished: " + finishedTaskResult.ToString());
                    badgerTaskList.Remove(finishedTask);

                    if (finishedTaskResult.failedExperiments.Count > 0)
                    {
                        foreach (ExperimentViewModel exp in finishedTaskResult.failedExperiments)
                            pendingExperiments.Add(exp);
                        logFunction(finishedTaskResult.failedExperiments.Count + " failed experiments enqueued again for further trials");
                    }

                    //just in case the freed agent hasn't still been discovered by the shepherd
                    if (!freeHerdAgents.Contains(finishedTaskResult.herdAgent))
                        freeHerdAgents.Add(finishedTaskResult.herdAgent);

                    //assign experiments to free agents
                    RemoteJob.assignExperiments(ref pendingExperiments, ref freeHerdAgents
                        , ref badgers, m_cancelTokenSource.Token, logFunction);
                }

            }
            catch (Exception ex)
            {
                logFunction("Exception in runExperimentQueueRemotely()");
                logFunction(ex.StackTrace);
            }
            finally
            {
                experimentQueueViewModel.enableEdition(true);
                m_cancelTokenSource.Dispose();
            }
        }

        public void stopExperiments()
        {
            if (m_cancelTokenSource != null)
                m_cancelTokenSource.Cancel();

            experimentQueueViewModel.resetState();
        }
    }
}
