using System.Diagnostics;
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
        private Stopwatch m_experimentTimer= new Stopwatch();
        
        private string m_estimatedEndTimeText = "";
        public string estimatedEndTime
        {
            get { return m_estimatedEndTimeText; }
            set { m_estimatedEndTimeText = value;  NotifyOfPropertyChange(() => estimatedEndTime); }
        }

        //progress expressed as a percentage
        public void updateGlobalProgress()
        {
            globalProgress = experimentQueueMonitor.calculateGlobalProgress();

            if (globalProgress > 0.0 && globalProgress<100.0)
                estimatedEndTime = "Estimated time to end: "
                    + System.TimeSpan.FromSeconds(m_experimentTimer.Elapsed.TotalSeconds
                    * ((100 - globalProgress) / globalProgress)).ToString(@"hh\:mm\:ss");
            else
                estimatedEndTime = "";
        }

        public MonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<Experiment> pendingExperiments, Logger.LogFunction logFunction, string batchFilename)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            evaluationPlot.bShowOptions = false;
            evaluationPlot.properties.bLegendVisible = false;
            evaluationPlot.setProperties();
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(freeHerdAgents, pendingExperiments
                , evaluationPlot,logFunction,this);
            m_batchFilename = batchFilename;
        }

        public void runExperiments(bool monitorProgress= true, bool receiveJobResults= true)
        {
            m_experimentTimer.Start();
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
