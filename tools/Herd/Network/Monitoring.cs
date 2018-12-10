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
        public enum State { RUNNING, FINISHED, ERROR, ENQUEUED, SENDING, RECEIVING, WAITING_EXECUTION, WAITING_RESULT };
    
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
