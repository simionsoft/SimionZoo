using Caliburn.Micro;
using Badger.ViewModels;


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

    }
}
