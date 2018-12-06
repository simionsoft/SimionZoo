using System;
using System.Collections.Generic;
using Herd.Files;

namespace Herd.Network
{
    public class Dispatcher
    {
        static int jobId = 0;
        /// <summary>
        ///     Assigns experiments to availables herd agents.
        /// </summary>
        /// <param name="freeHerdAgents"></param>
        /// 
        public static void AssignExperiments(ref List<MonitoredExperimentalUnit> pendingExperiments
            , ref List<HerdAgentInfo> freeHerdAgents, ref List<MonitoredJob> assignedJobs)
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
                List<MonitoredExperimentalUnit> experiments = new List<MonitoredExperimentalUnit>();
                int numFreeCores = agent.NumProcessors;
                bool bAgentUsed = false;
                MonitoredExperimentalUnit experiment;
                bool bFailedToFindMatch = false;

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
                    MonitoredJob newJob = new MonitoredJob("Job #" + jobId, experiments, agent);
                    assignedJobs.Add(newJob);
                    usedHerdAgents.Add(agent);


                    ++jobId;
                }

                if (pendingExperiments.Count == 0) break;
            }
            //now we can remove used agents from the list
            foreach (HerdAgentInfo agent in usedHerdAgents) freeHerdAgents.Remove(agent);
        }

        static MonitoredExperimentalUnit FirstFittingExperiment(List<MonitoredExperimentalUnit> pendingExperiments, int numFreeCores, bool bAgentUsed, HerdAgentInfo agent)
        {
            foreach (MonitoredExperimentalUnit experiment in pendingExperiments)
            {
                AppVersion bestMatchingVersion = agent.BestMatch(experiment.AppVersions);
                if (bestMatchingVersion != null)
                {
                    //run-time requirements are calculated when a version is selected
                    experiment.SelectedVersion = agent.BestMatch(experiment.AppVersions);
                    experiment.GetRuntimeRequirements(experiment.SelectedVersion, experiment.AppVersions);

                    if (experiment.RunTimeReqs == null)
                        return null;


                    //Check that the version chosen for the agent supports the architecture requested by the run-time 
                    if ((experiment.RunTimeReqs.Architecture == Herd.Network.PropValues.None
                        || experiment.RunTimeReqs.Architecture == experiment.SelectedVersion.Requirements.Architecture)
                        &&
                        //If NumCPUCores = "all", then the experiment only fits the agent in case it hasn't been given any other experimental unit
                        ((experiment.RunTimeReqs.NumCPUCores == 0 && !bAgentUsed)
                        //If NumCPUCores != "all", then experiment only fits the agent if the number of cpu cores used is less than those available
                        || (experiment.RunTimeReqs.NumCPUCores > 0 && experiment.RunTimeReqs.NumCPUCores <= numFreeCores)))
                        return experiment;
                }
            }
            return null;
        }
    }
}
