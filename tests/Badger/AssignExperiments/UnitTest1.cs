using Herd.Network;
using Herd.Files;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;
using System.Threading;

namespace AssignExperiments
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void Badger_AssignExperiments1()
        {
            //Easy configuration:
            //Agent 1: x64, 4 cores
            //Agent 2: x32, 2 cores
            //Agent 3: x64, 2 cores
            //Exp.Unit 1: 0 cores
            //Exp.Unit 2: 1 cores
            //Exp.Unit 3: 2 cores
            //Exp.Unit 4: 1 cores
            //Exp.Unit 5: 0 cores
            //Exp.Unit 6: 1 cores
            //Exp.Unit 7: 2 cores
            //Exp.Unit 8: 1 cores
            
            //create fake herd agents
            List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>
            {
                new HerdAgentInfo("Agent-1", 4, PropValues.Win64, PropValues.None, "1.1.0"),
                new HerdAgentInfo("Agent-2", 2, PropValues.Win32, "8.1.2", "1.1.0"),
                new HerdAgentInfo("Agent-3", 2, PropValues.Win64, "8.1.2", "1.1.0")
            };

            //create app versions
            AppVersionRequirements x64Reqs = new AppVersionRequirements(PropValues.Win64);
            AppVersionRequirements x32Reqs = new AppVersionRequirements(PropValues.Win32);
            AppVersion x32Version = new AppVersion("x32", "RLSimion.exe", x32Reqs);
            AppVersion x64Version = new AppVersion("x64", "RLSimion-x64.exe", x64Reqs);
            List<AppVersion> appVersions = new List<AppVersion>() { x32Version, x64Version };

            //create run-time requirements
            RunTimeRequirements cores_all = new RunTimeRequirements(0);
            RunTimeRequirements cores_2 = new RunTimeRequirements(2);
            RunTimeRequirements cores_1 = new RunTimeRequirements(1);
            //create fake experiments
            List<ExperimentalUnit> pendingExperiments = new List<ExperimentalUnit>()
            {
                new ExperimentalUnit("Experiment-1", appVersions, cores_all),
                new ExperimentalUnit("Experiment-2", appVersions, cores_1),
                new ExperimentalUnit("Experiment-3", appVersions, cores_2),
                new ExperimentalUnit("Experiment-4", appVersions, cores_1),
                new ExperimentalUnit("Experiment-5", appVersions, cores_all),
                new ExperimentalUnit("Experiment-6", appVersions, cores_1),
                new ExperimentalUnit("Experiment-7", appVersions, cores_1),
                new ExperimentalUnit("Experiment-8", appVersions, cores_1)
            };

            //create output list to receive assigned jobs
            List<Job> assignedJobs = new List<Job>();

            //Assign experiments
            JobDispatcher.AssignExperiments(ref pendingExperiments, ref herdAgents, ref assignedJobs);

            //Check everything went as expected
            Assert.IsTrue(assignedJobs.Count == 3);
            //  -assigned experimental units and agents
            Assert.IsTrue(assignedJobs[0].HerdAgent.ProcessorId == "Agent-1");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits.Count == 1);
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].Name == "Experiment-1");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win64);
            Assert.IsTrue(assignedJobs[1].HerdAgent.ProcessorId == "Agent-2");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits.Count == 2);
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[0].Name == "Experiment-2");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win32);
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[1].Name == "Experiment-4");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[1].SelectedVersion.Requirements.Architecture == PropValues.Win32);
            Assert.IsTrue(assignedJobs[2].HerdAgent.ProcessorId == "Agent-3");
            Assert.IsTrue(assignedJobs[2].ExperimentalUnits.Count == 1);
            Assert.IsTrue(assignedJobs[2].ExperimentalUnits[0].Name == "Experiment-3");
            Assert.IsTrue(assignedJobs[2].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win64);

            //  -used agents are removed
            Assert.IsTrue(herdAgents.Count == 0);
            //  -assigned experiments are removed
            Assert.IsTrue(pendingExperiments.Count == 4);
        }
        [TestMethod]
        public void Badger_AssignExperiments2()
        {
            //Case: Cannot find match for x64 agents because only x32 version is defined
            //Agent 1: x64, 4 cores
            //Agent 2: x32, 2 cores
            //Agent 3: x64, 2 cores
            //Exp.Unit 1: 0 cores
            //Exp.Unit 2: 1 cores
            //Exp.Unit 3: 2 cores
            //Exp.Unit 4: 1 cores
            //Exp.Unit 5: 0 cores
            //Exp.Unit 6: 1 cores
            //Exp.Unit 7: 2 cores
            //Exp.Unit 8: 1 cores

            //create fake herd agents
            List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>
            {
                new HerdAgentInfo("Agent-1", 4, PropValues.Win64, PropValues.None, "1.1.0"),
                new HerdAgentInfo("Agent-2", 2, PropValues.Win32, "8.1.2", "1.1.0"),
                new HerdAgentInfo("Agent-3", 2, PropValues.Win64, "8.1.2", "1.1.0")
            };

            //create app versions
            AppVersionRequirements x64Reqs = new AppVersionRequirements(PropValues.Win64);
            AppVersionRequirements x32Reqs = new AppVersionRequirements(PropValues.Win32);
            AppVersion x32Version = new AppVersion("x32", "RLSimion.exe", x32Reqs);
            List<AppVersion> appVersions = new List<AppVersion>() { x32Version };

            //create run-time requirements
            RunTimeRequirements cores_all = new RunTimeRequirements(0);
            RunTimeRequirements cores_2 = new RunTimeRequirements(2);
            RunTimeRequirements cores_1 = new RunTimeRequirements(1);
            //create fake experiments
            List<ExperimentalUnit> pendingExperiments = new List<ExperimentalUnit>()
            {
                new ExperimentalUnit("Experiment-1", appVersions, cores_all),
                new ExperimentalUnit("Experiment-2", appVersions, cores_1),
                new ExperimentalUnit("Experiment-3", appVersions, cores_2),
                new ExperimentalUnit("Experiment-4", appVersions, cores_1),
                new ExperimentalUnit("Experiment-5", appVersions, cores_all),
                new ExperimentalUnit("Experiment-6", appVersions, cores_1),
                new ExperimentalUnit("Experiment-7", appVersions, cores_1),
                new ExperimentalUnit("Experiment-8", appVersions, cores_1)
            };

            //create output list to receive assigned jobs
            List<Job> assignedJobs = new List<Job>();

            //Assign experiments
            JobDispatcher.AssignExperiments(ref pendingExperiments, ref herdAgents, ref assignedJobs);

            //Check everything went as expected
            Assert.IsTrue(assignedJobs.Count == 1);
            //  -assigned experimental units and agents
            Assert.IsTrue(assignedJobs[0].HerdAgent.ProcessorId == "Agent-2");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits.Count == 1);
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].Name == "Experiment-1");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win32);


            //  -used agents are removed
            Assert.IsTrue(herdAgents.Count == 2);
            //  -assigned experiments are removed
            Assert.IsTrue(pendingExperiments.Count == 7);
        }
        [TestMethod]
        public void Badger_AssignExperiments3()
        {
            //Case: Cannot find match for 1 agent because there is no core-matching experiment
            //Agent 1: x64, 4 cores
            //Agent 2: x32, 1 cores
            //Agent 3: x64, 3 cores
            //Exp.Unit 1: 5 cores
            //Exp.Unit 2: 5 cores
            //Exp.Unit 3: 5 cores
            //Exp.Unit 4: 2 cores
            //Exp.Unit 5: 3 cores
            //Exp.Unit 6: 5 cores
            //Exp.Unit 7: 2 cores
            //Exp.Unit 8: 5 cores

            //create fake herd agents
            List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>
            {
                new HerdAgentInfo("Agent-1", 4, PropValues.Win64, PropValues.None, "1.1.0"),
                new HerdAgentInfo("Agent-2", 1, PropValues.Win32, "8.1.2", "1.1.0"),
                new HerdAgentInfo("Agent-3", 3, PropValues.Win32, "8.1.2", "1.1.0")
            };

            //create app versions
            AppVersionRequirements x64Reqs = new AppVersionRequirements(PropValues.Win64);
            AppVersionRequirements x32Reqs = new AppVersionRequirements(PropValues.Win32);
            AppVersion x32Version = new AppVersion("x32", "RLSimion.exe", x32Reqs);
            AppVersion x64Version = new AppVersion("x64", "RLSimion-x64.exe", x64Reqs);
            List<AppVersion> appVersions = new List<AppVersion>() { x32Version, x64Version };

            //create run-time requirements
            RunTimeRequirements cores_1 = new RunTimeRequirements(1);
            RunTimeRequirements cores_2 = new RunTimeRequirements(2);
            RunTimeRequirements cores_3 = new RunTimeRequirements(3);
            RunTimeRequirements cores_5 = new RunTimeRequirements(5);

            //create fake experiments
            List<ExperimentalUnit> pendingExperiments = new List<ExperimentalUnit>()
            {
                new ExperimentalUnit("Experiment-1", appVersions, cores_5),
                new ExperimentalUnit("Experiment-2", appVersions, cores_5),
                new ExperimentalUnit("Experiment-3", appVersions, cores_5),
                new ExperimentalUnit("Experiment-4", appVersions, cores_2),
                new ExperimentalUnit("Experiment-5", appVersions, cores_3),
                new ExperimentalUnit("Experiment-6", appVersions, cores_5),
                new ExperimentalUnit("Experiment-7", appVersions, cores_2),
                new ExperimentalUnit("Experiment-8", appVersions, cores_5)
            };

            //create output list to receive assigned jobs
            List<Job> assignedJobs = new List<Job>();

            //Assign experiments
            JobDispatcher.AssignExperiments(ref pendingExperiments, ref herdAgents, ref assignedJobs);

            //Check everything went as expected
            Assert.IsTrue(assignedJobs.Count == 2);
            //  -assigned experimental units and agents
            Assert.IsTrue(assignedJobs[0].HerdAgent.ProcessorId == "Agent-1");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits.Count == 2);
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].Name == "Experiment-4");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[1].Name == "Experiment-7");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win64);
            Assert.IsTrue(assignedJobs[1].HerdAgent.ProcessorId == "Agent-3");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits.Count == 1);
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[0].Name == "Experiment-5");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win32);


            //  -used agents are removed
            Assert.IsTrue(herdAgents.Count == 1);
            //  -assigned experiments are removed
            Assert.IsTrue(pendingExperiments.Count == 5);
        }

        [TestMethod]
        public void Badger_AssignExperiments4()
        {
            //Easy configuration with a x64-only app:
            //Agent 1: x64, 4 cores
            //Agent 2: x32, 2 cores
            //Agent 3: x64, 2 cores
            //Exp.Unit 1: 0 cores
            //Exp.Unit 2: 1 cores
            //Exp.Unit 3: 2 cores
            //Exp.Unit 4: 1 cores
            //Exp.Unit 5: 0 cores
            //Exp.Unit 6: 1 cores
            //Exp.Unit 7: 2 cores
            //Exp.Unit 8: 1 cores

            //create fake herd agents
            List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>
            {
                new HerdAgentInfo("Agent-1", 4, PropValues.Win64, PropValues.None, "1.1.0"),
                new HerdAgentInfo("Agent-2", 2, PropValues.Win32, "8.1.2", "1.1.0"),
                new HerdAgentInfo("Agent-3", 2, PropValues.Win64, "8.1.2", "1.1.0")
            };

            //create app versions
            AppVersionRequirements x64Reqs = new AppVersionRequirements(PropValues.Win64);
            AppVersionRequirements x32Reqs = new AppVersionRequirements(PropValues.Win32);
            AppVersion x32Version = new AppVersion("x32", "RLSimion.exe", x32Reqs);
            AppVersion x64Version = new AppVersion("x64", "RLSimion-x64.exe", x64Reqs);
            List<AppVersion> appVersions = new List<AppVersion>() { x32Version, x64Version };

            //create run-time requirements
            RunTimeRequirements cores_all = new RunTimeRequirements(0, PropValues.Win64);
            RunTimeRequirements cores_2 = new RunTimeRequirements(2, PropValues.Win64);
            RunTimeRequirements cores_1 = new RunTimeRequirements(1, PropValues.Win64);
            //create fake experiments
            List<ExperimentalUnit> pendingExperiments = new List<ExperimentalUnit>()
            {
                new ExperimentalUnit("Experiment-1", appVersions, cores_all),
                new ExperimentalUnit("Experiment-2", appVersions, cores_1),
                new ExperimentalUnit("Experiment-3", appVersions, cores_2),
                new ExperimentalUnit("Experiment-4", appVersions, cores_1),
                new ExperimentalUnit("Experiment-5", appVersions, cores_all),
                new ExperimentalUnit("Experiment-6", appVersions, cores_1),
                new ExperimentalUnit("Experiment-7", appVersions, cores_1),
                new ExperimentalUnit("Experiment-8", appVersions, cores_1)
            };

            //create output list to receive assigned jobs
            List<Job> assignedJobs = new List<Job>();

            //Assign experiments
            JobDispatcher.AssignExperiments(ref pendingExperiments, ref herdAgents, ref assignedJobs);

            //Check everything went as expected
            Assert.IsTrue(assignedJobs.Count == 2);
            //  -assigned experimental units and agents
            Assert.IsTrue(assignedJobs[0].HerdAgent.ProcessorId == "Agent-1");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits.Count == 1);
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].Name == "Experiment-1");
            Assert.IsTrue(assignedJobs[0].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win64);
            Assert.IsTrue(assignedJobs[1].HerdAgent.ProcessorId == "Agent-3");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits.Count == 2);
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[0].Name == "Experiment-2");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[0].SelectedVersion.Requirements.Architecture == PropValues.Win64);
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[1].Name == "Experiment-4");
            Assert.IsTrue(assignedJobs[1].ExperimentalUnits[1].SelectedVersion.Requirements.Architecture == PropValues.Win64);

            //  -used agents are removed
            Assert.IsTrue(herdAgents.Count == 1);
            //  -assigned experiments are removed
            Assert.IsTrue(pendingExperiments.Count == 5);
        }
    }
}
