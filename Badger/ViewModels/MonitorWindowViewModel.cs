
using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using AppXML.Models;
using AppXML.ViewModels;
using AppXML.Data;


namespace AppXML.ViewModels
{
    public class MonitorWindowViewModel : Caliburn.Micro.Screen
    {
        public ExperimentQueueMonitorViewModel experimentQueueMonitor{get;set;}
        public PlotViewModel evaluationPlot{get;set;}

        public MonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<ExperimentViewModel> pendingExperiments, Logger.LogFunction logFunction)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(freeHerdAgents, pendingExperiments
                , evaluationPlot,logFunction);
        }

        public void runExperiments(string batchName, bool monitorProgress= true, bool receiveJobResults= true)
        {
            Task.Run(() => experimentQueueMonitor.runExperimentsAsync(batchName,monitorProgress,receiveJobResults));
        }

        protected override void OnDeactivate(bool close)
        {
            if (close)
                experimentQueueMonitor.stopExperiments();
            base.OnDeactivate(close);
        }
    }
}
