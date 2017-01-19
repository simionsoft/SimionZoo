
using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using Badger.Data;

namespace Badger.ViewModels
{
    public class MonitorWindowViewModel : Caliburn.Micro.Screen
    {
        public ExperimentQueueMonitorViewModel experimentQueueMonitor{get;set;}
        public PlotViewModel evaluationPlot{get;set;}
        private string m_batchFilename = null;

        public MonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<Experiment> pendingExperiments, Logger.LogFunction logFunction, string batchFilename)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(freeHerdAgents, pendingExperiments
                , evaluationPlot,logFunction);
            m_batchFilename = batchFilename;
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

        public void showReports()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            plotEditor.loadExperimentBatch(m_batchFilename);
            CaliburnUtility.showVMDialog(plotEditor, "Plot editor");
        }
    }
}
