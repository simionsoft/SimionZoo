using System;
#if !(__LINUX)
using System.ServiceProcess;
#endif


namespace Herd
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
#if !(__LINUX)
            //windows
            if (Environment.UserInteractive)
            {
                HerdService service = new HerdService();
                service.TestStartupAndStop(null);
            }
            else
            {
                // Put the body of your old Main method here.  
                ServiceBase[] ServicesToRun;
                ServicesToRun = new ServiceBase[]
                {
                    new HerdService()
                };
                ServiceBase.Run(ServicesToRun);
            }
#else
            //linux
            HerdService service = new HerdService();
            service.DoStart();
#endif

        }
    }
}
