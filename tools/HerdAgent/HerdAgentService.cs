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
using System.ServiceProcess;
using System.Threading;
using Herd.Network;

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
