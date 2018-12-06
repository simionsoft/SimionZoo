using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Herd.Network
{
    public class MonitoredJob
    {
        public string Name;
        public List<MonitoredExperimentalUnit> ExperimentalUnits { get; }
        public HerdAgentInfo HerdAgent;

        public MonitoredJob(string name, List<MonitoredExperimentalUnit> expUnits, HerdAgentInfo agent)
        {
            Name = name;
            ExperimentalUnits = expUnits;
            HerdAgent = agent;
        }
    }
}
