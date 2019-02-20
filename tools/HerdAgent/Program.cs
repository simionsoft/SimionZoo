using System;
using System.ServiceProcess;


namespace Herd
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {

            //windows
            if (Environment.UserInteractive)
            {
                HerdAgentServiceWindows service = new HerdAgentServiceWindows();
                service.TestStartupAndStop(null);
            }
            else
            {
                // Put the body of your old Main method here.  
                ServiceBase[] ServicesToRun;
                ServicesToRun = new ServiceBase[]
                {
                    new HerdAgentServiceWindows()
                };
                ServiceBase.Run(ServicesToRun);
            }
        }
    }
}
