using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Herd.Network
{
    public class JobDispatcherOptions
    {
        public bool LeaveOneFreeCore { get; set; } = false;
        public bool BalanceLoad { get; set; } = false;
    }
}
