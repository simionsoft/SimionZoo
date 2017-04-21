using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
            DisplayRootViewFor<WindowViewModel>();
        }

    }
}
