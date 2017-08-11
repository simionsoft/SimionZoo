using Caliburn.Micro;
using Badger.ViewModels;
using Badger.Data;
using System.Windows.Threading;
using System.Diagnostics;

namespace Badger
{
    public class AppBootstrapper : BootstrapperBase
    {

        public AppBootstrapper()
        {
            Initialize();
        }

        protected override void OnStartup(object sender, System.Windows.StartupEventArgs e)
        {
            DisplayRootViewFor<MainWindowViewModel>();
        }
        protected override void OnUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            if (Debugger.IsAttached)
                e.Handled = true;
            else e.Handled = true;
            
            CaliburnUtility.ShowWarningDialog(e.Exception.GetBaseException().ToString(), "ERROR: Badger crashed");

            base.OnUnhandledException(sender, e);
           
            //base.Application.Shutdown();

        }
    }
}
