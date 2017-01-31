
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

        private double m_globalProgress = 0.0;
        public double globalProgress
        {
            get { return m_globalProgress; }
            set { m_globalProgress = value; NotifyOfPropertyChange(() => globalProgress); }
        }

        public void updateGlobalProgress()
        {
            globalProgress = experimentQueueMonitor.calculateGlobalProgress();
        }

        public MonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<Experiment> pendingExperiments, Logger.LogFunction logFunction, string batchFilename)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(freeHerdAgents, pendingExperiments
                , evaluationPlot,logFunction,this);
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
