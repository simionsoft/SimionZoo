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

        public class Dispatcher
        {
            public CancellationToken CancelToken;
            public Action<State> AllStatesChanged;
            public Action<string, State> StateChanged;
            public Action<string, string, string> MessageReceived;
            public Action<string> Log;
            public Action<ExperimentalUnit> ExperimentalUnitLaunched;

            public Dispatcher(Action<State> onAllStatesChanged, Action<string, State> onStateChanged, Action<string, string, string> onMessageReceived
                , Action<ExperimentalUnit> onExperimentalUnitLaunched
                , Action<string> logFunction, CancellationToken cancelToken)
            {
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
