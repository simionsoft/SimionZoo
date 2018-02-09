using Badger.Data;
using Badger.Simion;
using Caliburn.Micro;
using Herd;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Badger.ViewModels
{
    public class MonitoredJobViewModel: PropertyChangedBase
    {
        private BindableCollection<MonitoredExperimentalUnitViewModel> m_monitoredExperimentalUnits =
            new BindableCollection<MonitoredExperimentalUnitViewModel>();
        public BindableCollection<MonitoredExperimentalUnitViewModel> MonitoredExperimentalUnits
        {
            get { return m_monitoredExperimentalUnits; }
            set
            {
                m_monitoredExperimentalUnits = value;
                NotifyOfPropertyChange(() => MonitoredExperimentalUnits);
            }
        }

        private HerdAgentViewModel m_herdAgent;
        public HerdAgentViewModel HerdAgent { get { return m_herdAgent; } }
        private string m_name;
        public string Name
        {
            get { return m_name; }
            set
            {
                m_name = value;
                NotifyOfPropertyChange(() => Name);
            }
        }
        public CancellationToken m_cancelToken;
        private Shepherd m_shepherd;

        private PlotViewModel m_evaluationPlot;
        private Dictionary<string, int> m_experimentSeriesId;

        private List<MonitoredExperimentalUnitViewModel> m_failedExperiments = new List<MonitoredExperimentalUnitViewModel>();
        public List<MonitoredExperimentalUnitViewModel> FailedExperiments { get { return m_failedExperiments; } set { } }

        private Logger.LogFunction m_logFunction = null;
        private void logMessage(string message)
        {
            m_logFunction?.Invoke(message);
        }

        public MonitoredJobViewModel(string name, List<MonitoredExperimentalUnitViewModel> experiments, HerdAgentViewModel herdAgent,
            PlotViewModel evaluationPlot, CancellationToken cancelToken, Logger.LogFunction logFunction)
        {
            int taskId = 0;
            m_name = name;
            foreach (MonitoredExperimentalUnitViewModel expUnit in experiments)
            {
                expUnit.TaskName = "Task #" + taskId;
                taskId++;
                MonitoredExperimentalUnits.Add(expUnit);
            }

            m_herdAgent = herdAgent;
            m_logFunction = logFunction;
            m_shepherd = new Shepherd();
            m_shepherd.setLogMessageHandler(logFunction);
            m_cancelToken = cancelToken;
            m_evaluationPlot = evaluationPlot;
            m_experimentSeriesId = new Dictionary<string, int>();
        }

        private HerdJob AsHerdJob()
        {
            HerdJob job = new HerdJob();
            job.name = m_name;

            // tasks, inputs and outputs
            foreach (MonitoredExperimentalUnitViewModel experiment in MonitoredExperimentalUnits)
            {
                HerdTask task = new HerdTask();
                // We are assuming the same exe file is used in all the experiments!!!
                // IMPORTANT

                //Because the experiment file might well be outside the RLSimion folder structure
                //we need make all paths to experiment files relative and let the herd agent
                //know that they must be renamed
                string relPathExperimentFile = SimionFileData.experimentRelativeDir
                     + "\\" + Utility.RemoveDirectories(experiment.FilePath, 2);
                task.name = experiment.Name;
                task.exe = experiment.ExeFile;
                task.arguments = relPathExperimentFile + " -pipe=" + experiment.PipeName;
                task.pipe = experiment.PipeName;
                task.authenticationToken = m_herdAgent.AuthenticationCode; //TODO: Add Settings?

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

                Utility.GetInputsAndOutputs(experiment.ExeFile, experiment.FilePath, ref job);

                //add rename rules for experiments outside RLSimion's folder structure
                //the experiment file itself
                if (experiment.FilePath != relPathExperimentFile && !job.renameRules.Keys.Contains(experiment.FilePath))
                    job.renameRules.Add(experiment.FilePath, relPathExperimentFile);
                //the output files
                foreach (string outputFile in job.outputFiles)
                {
                    string renamedFile = SimionFileData.experimentRelativeDir + "\\"
                        + Utility.RemoveDirectories(outputFile, 2);
                    if (outputFile != renamedFile && !job.renameRules.Keys.Contains(outputFile))
                        job.renameRules.Add(outputFile, renamedFile);
                }
            }

            return job;
        }

        MonitoredExperimentalUnitViewModel Find(string id)
        {
            foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                if (expUnit.Name == id)
                    return expUnit;
            return null;
        }

        public async Task<MonitoredJobViewModel> SendJobAndMonitor()
        {
            m_failedExperiments.Clear();
            try
            {
                //SEND THE JOB DATA
                foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                    expUnit.State= MonitoredExperimentalUnitViewModel.ExperimentState.WAITING_EXECUTION;
                HerdJob job = AsHerdJob();

                bool bConnected = m_shepherd.connectToHerdAgent(m_herdAgent.IpAddress);
                if (bConnected)
                {
                    logMessage("Sending job to herd agent " + m_herdAgent.IpAddress);
                    foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                        expUnit.State = MonitoredExperimentalUnitViewModel.ExperimentState.SENDING;
                    //m_herdAgent.State = "Sending job query";
                    m_shepherd.SendJobQuery(job, m_cancelToken);
                    logMessage("Job sent to herd agent " + m_herdAgent.IpAddress);
                    //await m_shepherd.waitAsyncWriteOpsToFinish();
                    foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                        expUnit.State = MonitoredExperimentalUnitViewModel.ExperimentState.RUNNING;
                    //m_herdAgent.State = "Executing job query";
                }
                else
                {
                    foreach (MonitoredExperimentalUnitViewModel exp in MonitoredExperimentalUnits) m_failedExperiments.Add(exp);
                    foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                        expUnit.State = MonitoredExperimentalUnitViewModel.ExperimentState.ERROR;
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
                        MonitoredExperimentalUnitViewModel experimentVM = Find(experimentId);
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
                                        seriesId = m_evaluationPlot.AddLineSeries(seriesName);
                                        m_experimentSeriesId.Add(seriesName, seriesId);
                                    }
                                    else seriesId = m_experimentSeriesId[seriesName];

                                    m_evaluationPlot.AddLineSeriesValue(seriesId, double.Parse(values[0], CultureInfo.InvariantCulture)
                                        , double.Parse(values[1], CultureInfo.InvariantCulture));
                                }
                            }
                            else if (messageId == "Message")
                            {
                                experimentVM.AddStatusInfoLine(messageContent);
                            }
                            else if (messageId == "End")
                            {
                                if (messageContent == "Ok")
                                {
                                    logMessage("Job finished sucessfully");
                                    experimentVM.State = MonitoredExperimentalUnitViewModel.ExperimentState.WAITING_RESULT;
                                }
                                else
                                {
                                    logMessage("Remote job execution wasn't successful");
                                    //Right now, my view on adding failed experiments back to the pending exp. list:
                                    //Some experiments may fail because the parameters are just invalid (i.e. FAST)
                                    //Much more likely than a network-related error or some other user-related problem
                                    //m_failedExperiments.Add(experimentVM);
                                    experimentVM.State = MonitoredExperimentalUnitViewModel.ExperimentState.ERROR;
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
                                    foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                                        expUnit.State = MonitoredExperimentalUnitViewModel.ExperimentState.RECEIVING;
                                    //m_herdAgent.State = "Receiving output files";
                                    bool bret = await m_shepherd.ReceiveJobResult(m_cancelToken);
                                    foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                                        expUnit.State = MonitoredExperimentalUnitViewModel.ExperimentState.FINISHED;
                                    //m_herdAgent.State = "Finished";
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

                foreach(MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                    expUnit.resetState();
                //m_herdAgent.State = "";
            }
            catch (Exception ex)
            {
                logMessage("Unhandled exception in Badger.sendJobAndMonitor(). Agent " + m_herdAgent.IpAddress);
                logMessage(ex.ToString());
                m_failedExperiments.Clear();
                foreach (MonitoredExperimentalUnitViewModel exp in MonitoredExperimentalUnits)
                    m_failedExperiments.Add(exp);
                Console.WriteLine(ex.StackTrace);
            }
            finally
            {
                logMessage("Disconnected from herd agent " + m_herdAgent.IpAddress);
                m_shepherd.disconnect();
            }
            return this;
        }
        /// <summary>
        /// Normalized progress in this job [0,1]
        /// </summary>
        public double NormalizedProgress
        {
            get
            {
                double progressSum = 0.0;
                foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                    progressSum += (expUnit.Progress / 100.0); //<- they are percentages
                return progressSum / MonitoredExperimentalUnits.Count;
            }
        }
    }

}
