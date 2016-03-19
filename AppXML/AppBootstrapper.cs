using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using AppXML.ViewModels;
using System.ComponentModel.Composition;
using System.ComponentModel.Composition.Hosting;
using System.ComponentModel.Composition.Primitives;
 

namespace AppXML
{
        public class AppBootstrapper: BootstrapperBase {


        private CompositionContainer container;
        public AppBootstrapper() {
            Initialize();
        }

        protected override void OnStartup(object sender, System.Windows.StartupEventArgs e) {
            DisplayRootViewFor<ProcessesWindowViewModel>();
        }
/*
        protected override void Configure()
        {
          container = new CompositionContainer(new AggregateCatalog(AssemblySource.Instance.Select(x => new AssemblyCatalog(x)).OfType<ComposablePartCatalog>()));

          CompositionBatch batch = new CompositionBatch();

          batch.AddExportedValue<IWindowManager>(new AppWindowManager());
          batch.AddExportedValue<IEventAggregator>(new EventAggregator());
          batch.AddExportedValue(container);

          container.Compose(batch);
        }

        protected override object GetInstance(Type serviceType, string key)
        {
          string contract = string.IsNullOrEmpty(key) ? AttributedModelServices.GetContractName(serviceType) : key;
          var exports = container.GetExportedValues<object>(contract);

          if (exports.Count() > 0)
          {
            return exports.First();
          }

          throw new Exception(string.Format("Could not locate any instances of contract {0}.", contract));
        }
*/
      }
    
  
}
