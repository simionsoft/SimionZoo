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

using Herd.Files;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Herd.Network
{

    public class Monitoring
    {
        public enum State { UNITIALIZED, RUNNING, FINISHED, ERROR, SENDING, RECEIVING, WAITING_RESULT };
    
        public delegate void StateChangedCallback(string expUnitName, State state);
        public delegate void AllStatesChangedCallback(State state);
        public delegate void MessageReceivedCallback(string expUnitName, string messageId, string messageContent);

        public class MsgDispatcher
        {
            public Action<string> Log;
            public CancellationToken CancelToken;

            public Action<Job, State> AllStatesChanged;
            public Action<Job, string, State> StateChanged;
            public Action<Job, string, string, string> MessageReceived;
            public Action<Job, ExperimentalUnit> ExperimentalUnitLaunched;
            public Action<Job> JobAssigned;
            public Action<Job> JobFinished;

            public MsgDispatcher( Action<Job> onJobAssigned, Action<Job> onJobFinished
                , Action<Job, State> onAllStatesChanged, Action<Job, string, State> onStateChanged
                , Action<Job, string, string, string> onMessageReceived
                , Action<Job, ExperimentalUnit> onExperimentalUnitLaunched
                , Action<string> logFunction, CancellationToken cancelToken)
            {
                JobAssigned = onJobAssigned;
                JobFinished = onJobFinished;
                AllStatesChanged = onAllStatesChanged;
                StateChanged = onStateChanged;
                MessageReceived = onMessageReceived;
                ExperimentalUnitLaunched = onExperimentalUnitLaunched;
                Log = logFunction;
                CancelToken = cancelToken;
            }
        }
    }
}
