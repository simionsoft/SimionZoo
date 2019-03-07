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

using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Herd.Files;

namespace Herd.Network
{
    public class JobDispatcher
    {
        public const string ProgressMessage = "Progress";
        public const string EvaluationMessage = "Evaluation";
        public const string GeneralMessage = "Message";
        public const string EndMessage = "End";

        public const string EndMessageOk = "Ok";

        static int jobId = 0;
        /// <summary>
        /// Assigns experiments to available herd agents.
        /// </summary>
        /// <param name="freeHerdAgents"></param>
        public static void AssignExperiments(ref List<ExperimentalUnit> pendingExperiments
            , ref List<HerdAgentInfo> freeHerdAgents, ref List<Job> assignedJobs, JobDispatcherOptions options= null)
        {
            //Clear the list: these are jobs which have to be sent
            assignedJobs.Clear();
            //Create a list of agents that are given work. We need to remove them from the "free" list out of the loop
            List<HerdAgentInfo> usedHerdAgents = new List<HerdAgentInfo>();


            //We iterate on the free agents to decide what jobs to give each of them until:
            //  -either there are no more pending experiments
            //  -all agents have been given work
            foreach (HerdAgentInfo agent in freeHerdAgents)
            {
                List<ExperimentalUnit> experiments = new List<ExperimentalUnit>();
                int numFreeCores = agent.NumProcessors;
                bool bAgentUsed = false;
                ExperimentalUnit experiment;
                bool bFailedToFindMatch = false;

                //leave one free core??
                if (options != null && options.LeaveOneFreeCore && numFreeCores>1)
                    numFreeCores--;

                while (numFreeCores > 0 && !bFailedToFindMatch)
                {
                    experiment = FirstFittingExperiment(pendingExperiments, numFreeCores, bAgentUsed, agent);
                    if (experiment != null)
                    {
                        //remove the experiment from the list and add it to running experiments
                        experiments.Add(experiment);
                        pendingExperiments.Remove(experiment);

                        //update the number of free cpu cores
                        if (experiment.RunTimeReqs.NumCPUCores > 0)
                            numFreeCores -= experiment.RunTimeReqs.NumCPUCores;
                        else numFreeCores = 0;

                        bAgentUsed = true;
                    }
                    else bFailedToFindMatch = true;
                }

                if (bAgentUsed)
                {
                    Job newJob = new Job(experiments, agent);
                    assignedJobs.Add(newJob);
                    usedHerdAgents.Add(agent);


                    ++jobId;
                }

                if (pendingExperiments.Count == 0) break;
            }
            //now we can remove used agents from the list
            foreach (HerdAgentInfo agent in usedHerdAgents) freeHerdAgents.Remove(agent);
        }

        public static async Task<int> RunExperimentsAsync(List<ExperimentalUnit> experiments, List<HerdAgentInfo> freeHerdAgents
            , Monitoring.MsgDispatcher dispatcher
            , CancellationTokenSource cancellationTokenSource
            , JobDispatcherOptions jobDispatcherOptions = null)
        {
            List<Job> assignedJobs = new List<Job>();
            List<Task<Job>> monitoredJobTasks = new List<Task<Job>>();
            int numExperimentalUnitsRun = 0;

            // Calculate run-time requirements
            foreach (ExperimentalUnit experimentalUnit in experiments) experimentalUnit.RequestRuntimeRequirements();

            // Assign experiments to free agents
            AssignExperiments(ref experiments, ref freeHerdAgents, ref assignedJobs, jobDispatcherOptions);

            if (assignedJobs.Count == 0)
                return 0;
            try
            {
                while ((assignedJobs.Count > 0 || monitoredJobTasks.Count > 0
                    || experiments.Count > 0) && !cancellationTokenSource.IsCancellationRequested)
                {
                    //Create view-models for the jobs and execute them remotely
                    foreach (Job job in assignedJobs)
                    {
                        dispatcher.JobAssigned?.Invoke(job);
                        
                        monitoredJobTasks.Add(job.SendJobAndMonitor(dispatcher));
                    }

                    // All pending experiments sent? Then we await completion to retry in case something fails
                    if (experiments.Count == 0)
                    {
                        Task.WhenAll(monitoredJobTasks).Wait();
                        foreach (Task<Job> task in monitoredJobTasks)
                            dispatcher.JobFinished?.Invoke(task.Result);
                        dispatcher.Log?.Invoke("All the experiments have finished");
                        break;
                    }

                    // Wait for the first agent to finish and give it something to do
                    Task<Job> finishedTask = await Task.WhenAny(monitoredJobTasks);
                    Job finishedJob = await finishedTask;
                    dispatcher.Log?.Invoke("Job finished: " + finishedJob.ToString());

                    //A job finished
                    monitoredJobTasks.Remove(finishedTask);
                    if (!cancellationTokenSource.IsCancellationRequested)
                    {
                        numExperimentalUnitsRun += finishedJob.ExperimentalUnits.Count;
                        dispatcher.JobFinished?.Invoke(finishedJob);
                    }


                    if (finishedJob.FailedExperimentalUnits.Count > 0)
                    {
                        experiments.AddRange(finishedJob.FailedExperimentalUnits);
                        dispatcher.Log?.Invoke(finishedJob.FailedExperimentalUnits.Count + " failed experiments enqueued again for further trials");
                    }

                    // Add the herd agent to the free agent list
                    if (!freeHerdAgents.Contains(finishedJob.HerdAgent))
                        freeHerdAgents.Add(finishedJob.HerdAgent);

                    // Assign experiments to free agents
                    if (!cancellationTokenSource.IsCancellationRequested)
                        AssignExperiments(ref experiments, ref freeHerdAgents, ref assignedJobs, jobDispatcherOptions);
                }
            }
            catch (Exception ex)
            {
                dispatcher.Log?.Invoke("Exception in runExperimentQueueRemotely()");
                dispatcher.Log?.Invoke(ex.StackTrace);
            }
            finally
            {
                if (cancellationTokenSource.IsCancellationRequested)
                {
                    //the user cancelled, need to add unfinished experimental units to the pending list
                    foreach (Job job in assignedJobs)
                        experiments.AddRange(job.ExperimentalUnits);
                }
                else
                {
                    foreach (Task<Job> job in monitoredJobTasks)
                        numExperimentalUnitsRun += job.Result.ExperimentalUnits.Count - job.Result.FailedExperimentalUnits.Count;
                }
            }
            return numExperimentalUnitsRun;
        }

        /// <summary>
        /// Returns the first experimental unit that fits the agent
        /// </summary>
        /// <param name="pendingExperiments">The pending experimental units</param>
        /// <param name="numFreeCores">The number agent's free cores</param>
        /// <param name="bAgentUsed">Is the agent already being used for another experimental unit?</param>
        /// <param name="agent">The herd agent</param>
        /// <returns></returns>
        static ExperimentalUnit FirstFittingExperiment(List<ExperimentalUnit> pendingExperiments, int numFreeCores, bool bAgentUsed, HerdAgentInfo agent)
        {
            foreach (ExperimentalUnit experiment in pendingExperiments)
            {
                experiment.SelectedVersion = agent.BestMatch(experiment);
                if (experiment.SelectedVersion != null)
                {
                    //If NumCPUCores = "all", then the experiment only fits the agent in case it hasn't been given any other experimental unit
                    if ( (experiment.RunTimeReqs.NumCPUCores == 0 && !bAgentUsed)
                        //If NumCPUCores != "all", then experiment only fits the agent if the number of cpu cores used is less than those available
                        || (experiment.RunTimeReqs.NumCPUCores > 0 && experiment.RunTimeReqs.NumCPUCores <= numFreeCores) )
                        return experiment;
                }
            }
            return null;
        }
    }
}
