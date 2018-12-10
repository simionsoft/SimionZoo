using System.Threading;
using Herd.Network;

namespace Herd
{

    public partial class HerdAgentServiceLinux
    {
        private HerdAgent m_herdAgent;

        public HerdAgentServiceLinux()
        {
        }
 
        public void DoStart()
        {
            //Start herd-agent
            m_herdAgent = new HerdAgent(new CancellationTokenSource());

            m_herdAgent.StartListening();

            while (true)
            {
                Thread.Sleep(1000);
            }
        }

        public void DoStop()
        {
            m_herdAgent.StopListening();
        }
    }
}
