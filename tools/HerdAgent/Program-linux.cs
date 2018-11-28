using System;

namespace Herd
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
            HerdAgentServiceLinux service = new HerdAgentServiceLinux();
            service.DoStart();
        }
    }
}
