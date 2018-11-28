using System;
using System.ServiceProcess;
using System.Threading;

//Installing/uninstalling the service: https://msdn.microsoft.com/en-us/library/zt39148a%28v=vs.110%29.aspx?f=255&MSPPError=-2147217396#BK_Install

namespace Herd
{

    public partial class HerdAgentServiceWindows : ServiceBase
    {
        private HerdAgent m_herdAgent;


        public HerdAgentServiceWindows()
        {
            InitializeComponent();
        }


        internal void TestStartupAndStop(string[] args)
        {
            this.OnStart(args);
            Console.ReadLine();
            this.OnStop();
        }

        protected override void OnStart(string[] args)
        {
            DoStart();
        }

        public void DoStart()
        {
            //Start herd-agent
            m_herdAgent = new HerdAgent(new CancellationTokenSource());

            m_herdAgent.StartListening();
        }

        protected override void OnStop()
        {
            DoStop();
        }
        public void DoStop()
        {
            m_herdAgent.StopListening();
        }
    }
}
