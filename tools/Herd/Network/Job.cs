using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Herd.Files;

namespace Herd.Network
{
    public class Job
    {
        static int jobId = 0;
        /// <summary>
        /// Constructor used from Network.Dispatcher
        /// </summary>
        public Job(List<ExperimentalUnit> experimentalUnits, HerdAgentInfo herdAgent)
        {
            Name = "Job #" + jobId;
            jobId++;
            ExperimentalUnits = experimentalUnits;
            HerdAgent = herdAgent;
        }

        /// <summary>
        /// Parameter-less constructor using for reading a job from the network: Network.JobTransmitter
        /// </summary>
        public Job()
        {
        }

        public string Name { get; set; }
        public List<ExperimentalUnit> ExperimentalUnits { get; }
        public HerdAgentInfo HerdAgent { get; }


        public List<ExperimentalUnit> FailedExperimentalUnits { get; set; }

        public void PrepareForExecution()
        {
            FailedExperimentalUnits = new List<ExperimentalUnit>();
            // tasks, inputs and outputs
            foreach (ExperimentalUnit experiment in ExperimentalUnits)
            {
                HerdTask task = new HerdTask();
                // We are assuming the same exe file is used in all the experiments!!!
                // IMPORTANT

                //Because the experiment file might well be outside the RLSimion folder structure
                //we need make all paths to experiment files relative and let the herd agent
                //know that they must be renamed
                
                task.Name = experiment.Name;
                task.Exe = experiment.SelectedVersion.ExeFile;
                task.Arguments = experiment.RelativeExperimentFileName + " -pipe=" + experiment.Name;
                task.Pipe = experiment.Name;
                task.AuthenticationToken = "";// HerdAgent.AuthenticationCode; //TODO: Add Settings?

                Tasks.Add(task);

                //////App Version requirements
                //Exe file
                AddInputFile(task.Exe);

                //Rename rules
                AddRenameRules(experiment.SelectedVersion.Requirements.RenameRules);

                //Input files
                AddInputFiles(experiment.SelectedVersion.Requirements.InputFiles);
                AddInputFile(experiment.ExperimentFileName);
                //No output files come from the app version requirements in principle

                //////Run-time requirements
                //Input files
                AddInputFiles(experiment.RunTimeReqs.InputFiles);
                AddOutputFiles(experiment.RunTimeReqs.OutputFiles);


                /////Fix relative paths outside RLSimion's folder structure
                //we add rename rules:
                //  -for the experiment file itself
                if (experiment.ExperimentFileName != experiment.RelativeExperimentFileName && !RenameRules.Keys.Contains(experiment.ExperimentFileName))
                    RenameRules.Add(experiment.ExperimentFileName, experiment.RelativeExperimentFileName);
                //  -for the output files
                foreach (string outputFile in OutputFiles)
                {
                    string renamedFile = Folders.experimentRelativeDir + "/"
                        + Herd.Utils.RemoveDirectories(outputFile, 2);
                    if (outputFile != renamedFile && !RenameRules.Keys.Contains(outputFile))
                        RenameRules.Add(outputFile, renamedFile);
                }
            }
        }

        //tasks
        public List<HerdTask> Tasks { get; }= new List<HerdTask>();

        //input files
        List<string> m_inputFiles = new List<string>();
        public void AddInputFiles(List<string> source) { foreach (string file in source) AddInputFile(file); }
        public bool AddInputFile(string file)
        {
            if (!m_inputFiles.Contains(file))
            {
                m_inputFiles.Add(file);
                return true;
            }
            else return false;
        }
        public List<string> InputFiles { get { return m_inputFiles; } }

        //output files
        List<string> m_outputFiles = new List<string>();
        public void AddOutputFiles(List<string> source) { foreach (string file in source) AddOutputFile(file); }
        public bool AddOutputFile(string file)
        {
            if (!m_outputFiles.Contains(file))
            {
                m_outputFiles.Add(file);
                return true;
            }
            else return false;
        }
        public List<string> OutputFiles { get { return m_outputFiles; } }

        //rename rules
        public Dictionary<string, string> RenameRules { get; } = new Dictionary<string, string>();
        public void AddRenameRule(string original, string renamed) { RenameRules[original] = renamed; }
        public void AddRenameRules(Dictionary<string, string> source) { foreach (string key in source.Keys) RenameRules[key] = source[key]; }


        public override string ToString()
        {
            string ret = "Job: " + Name;
            foreach (HerdTask task in Tasks)
                ret += "||" + task.ToString();
            return ret;
        }

        public string RenamedFilename(string filename)
        {
            if (RenameRules != null && RenameRules.ContainsKey(filename))
                return RenameRules[filename];
            return filename;
        }
        public string OriginalFilename(string filename)
        {
            if (RenameRules != null)
            {
                foreach (string originalName in RenameRules.Keys)
                {
                    if (RenameRules[originalName] == filename)
                        return originalName;
                }
            }
            return filename;
        }

        public bool Finished { get; set; } = false;

        //Remote execution and monitoring of the job
        public async Task<Job> SendJobAndMonitor(Monitoring.MsgDispatcher dispatcher)
        {
            Shepherd shepherd = new Shepherd();
            shepherd.SetLogMessageHandler(dispatcher.Log);

            try
            {
                PrepareForExecution(); // compute inputs/outputs...

                //Let the dispatcher know which experimental units were created
                foreach(ExperimentalUnit expUnit in ExperimentalUnits)
                    dispatcher.ExperimentalUnitLaunched?.Invoke(this, expUnit);

                bool bConnected = shepherd.ConnectToHerdAgent(HerdAgent.ipAddress);
                if (bConnected)
                {
                    dispatcher.Log?.Invoke("Sending job to herd agent " + HerdAgent.ipAddress);
                    dispatcher.AllStatesChanged?.Invoke(this, Monitoring.State.SENDING);
                    
                    shepherd.SendJobQuery(this, dispatcher.CancelToken);
                    dispatcher.Log?.Invoke("Job sent to herd agent " + HerdAgent.ipAddress);

                    dispatcher.AllStatesChanged(this, Monitoring.State.RUNNING);
                }
                else
                {
                    ///SOLVE THIS: failed experimental units should be dealt by the caller
                    ///
                    foreach (ExperimentalUnit exp in ExperimentalUnits) FailedExperimentalUnits.Add(exp);

                    dispatcher.AllStatesChanged?.Invoke(this, Monitoring.State.ERROR);
                    dispatcher.Log?.Invoke("Failed to connect to herd agent " + HerdAgent.ipAddress);

                    return this;
                }
                dispatcher.Log?.Invoke("Monitoring remote job run by herd agent " + HerdAgent.ipAddress);
                // Monitor the remote job
                while (true)
                {
                    int numBytesRead = await shepherd.ReadAsync(dispatcher.CancelToken);
                    dispatcher.CancelToken.ThrowIfCancellationRequested();

                    string xmlItem = shepherd.m_xmlStream.processNextXMLItem();

                    while (xmlItem != "")
                    {
                        string experimentId = shepherd.m_xmlStream.getLastXMLItemTag();
                        string message = shepherd.m_xmlStream.getLastXMLItemContent();

                        string messageId = shepherd.m_xmlStream.getLastXMLItemTag(); //previous call to getLastXMLItemContent resets lastXMLItem
                        string messageContent = shepherd.m_xmlStream.getLastXMLItemContent();

                        if (experimentId == XMLStream.m_defaultMessageType)
                        {
                            //The message comes from the herd agent, must be sending results
                            dispatcher.Log?.Invoke("Receiving job results");
                            dispatcher.AllStatesChanged?.Invoke(this, Monitoring.State.RECEIVING);

                            bool bret = await shepherd.ReceiveJobResult(dispatcher.CancelToken);
                            dispatcher.AllStatesChanged?.Invoke(this, Monitoring.State.FINISHED);

                            Finished = true;

                            dispatcher.Log?.Invoke("Job results received");
                            return this;
                        }
                        else //the message comes from an experimental unit, the dispatcher will deal with it
                            dispatcher.MessageReceived?.Invoke(this, experimentId, messageId, messageContent);
                        
                        xmlItem = shepherd.m_xmlStream.processNextXMLItem();
                    }
                }
            }
            catch (OperationCanceledException)
            {
                //quit remote jobs
                dispatcher.Log?.Invoke("Cancellation requested by user");
                shepherd.WriteMessage(Shepherd.m_quitMessage, true);
                await shepherd.ReadAsync(new CancellationToken()); //we synchronously wait until we get the ack from the client
            }
            catch (Exception ex)
            {
                dispatcher.Log?.Invoke("Unhandled exception in Badger.sendJobAndMonitor(). Agent " + HerdAgent.ipAddress);
                dispatcher.Log?.Invoke(ex.ToString());
                FailedExperimentalUnits.Clear();
                FailedExperimentalUnits.AddRange(ExperimentalUnits);
            }
            finally
            {
                dispatcher.Log?.Invoke("Disconnected from herd agent " + HerdAgent.ipAddress);
                shepherd.Disconnect();
            }
            return this;
        }
    }
}
