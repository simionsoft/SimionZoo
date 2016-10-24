
using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;


namespace Badger.ViewModels
{
    public class MonitorWindowViewModel : Caliburn.Micro.Screen
    {
        public ExperimentQueueMonitorViewModel experimentQueueMonitor{get;set;}
        public PlotViewModel evaluationPlot{get;set;}

        public MonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<AppViewModel> pendingExperiments, Logger.LogFunction logFunction)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(freeHerdAgents, pendingExperiments
                , evaluationPlot,logFunction);
        }

        public void runExperiments(bool monitorProgress= true, bool receiveJobResults= true)
        {
            Task.Run(() => experimentQueueMonitor.runExperimentsAsync(monitorProgress,receiveJobResults));
        }

        protected override void OnDeactivate(bool close)
        {
            if (close)
                experimentQueueMonitor.stopExperiments();
            base.OnDeactivate(close);
        }
    }
}
