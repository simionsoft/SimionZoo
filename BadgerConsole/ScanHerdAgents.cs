using Herd.Network;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BadgerConsole
{
    class ScanHerdAgents
    {
        public static void Run()
        {
            Console.WriteLine("Scanning herd agents:\n");
            Shepherd shepherd = new Shepherd();
            shepherd.CallHerd();

            Thread.Sleep(2000);

            List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>();
            shepherd.GetHerdAgentList(ref herdAgents);

            for (int i= 0; i<herdAgents.Count; i++)
            {
                Console.WriteLine("#{0}: IP={1} Num.Processors={2} Architecture={3}", i, herdAgents[i].IpAddressString, herdAgents[i].NumProcessors, herdAgents[i].ProcessorArchitecture);
            }
        }
    }
}
