using System.Diagnostics;
using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using Badger.Data;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class MonitorWindowViewModel : PropertyChangedBase
    {
        private ExperimentQueueMonitorViewModel m_experimentQueueMonitorViewModel;

        public ExperimentQueueMonitorViewModel experimentQueueMonitor
        {
            get { return m_experimentQueueMonitorViewModel; }
            set
            {
                m_experimentQueueMonitorViewModel = value;
                NotifyOfPropertyChange(() => experimentQueueMonitor);
            }
        }
        public PlotViewModel evaluationPlot { get; set; }

        private List<HerdAgentViewModel> m_freeHerdAgents;

        public List<HerdAgentViewModel> FreeHerdAgents
        {
            get { return m_freeHerdAgents; }
            set { m_freeHerdAgents = value; }
        }

        private List<Experiment> m_pendingExperiments;

        public List<Experiment> PendingExperiments
        {
            get { return m_pendingExperiments; }
            set
            {
                m_pendingExperiments = value;
            }
        }

        private Logger.LogFunction m_logFunction;

        public Logger.LogFunction LogFunction
        {
            get { return m_logFunction; }
            set { m_logFunction = value; }
        }

        private string m_batchFilename = null;

        public string BatchFileName
        {
            get { return m_batchFilename; }
            set { m_batchFilename = value; }
        }

        private double m_globalProgress = 0.0;
        public double globalProgress
        {
            get { return m_globalProgress; }
            set { m_globalProgress = value; NotifyOfPropertyChange(() => globalProgress); }
        }

        private Stopwatch m_experimentTimer = new Stopwatch();

        private string m_estimatedEndTimeText = "";
        public string estimatedEndTime
        {
            get { return m_estimatedEndTimeText; }
            set { m_estimatedEndTimeText = value; NotifyOfPropertyChange(() => estimatedEndTime); }
        }

        //progress expressed as a percentage
        public void updateGlobalProgress()
        {
            globalProgress = experimentQueueMonitor.calculateGlobalProgress();

            if (globalProgress > 0.0 && globalProgress < 100.0)
                estimatedEndTime = "Estimated time to end: "
                    + System.TimeSpan.FromSeconds(m_experimentTimer.Elapsed.TotalSeconds
                    * ((100 - globalProgress) / globalProgress)).ToString(@"hh\:mm\:ss");
            else
                estimatedEndTime = "";
        }

        private WindowViewModel m_parent;

        public MonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents,
            List<Experiment> pendingExperiments, Logger.LogFunction logFunction, string batchFilename, WindowViewModel parent)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            evaluationPlot.bShowOptions = false;
            evaluationPlot.properties.bLegendVisible = false;
            evaluationPlot.setProperties();

            m_freeHerdAgents = freeHerdAgents;
            m_pendingExperiments = pendingExperiments;
            m_logFunction = logFunction;
            m_batchFilename = batchFilename;
            m_parent = parent;
        }

        public void runExperiments(bool monitorProgress = true, bool receiveJobResults = true)
        {
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(m_freeHerdAgents, m_pendingExperiments,
                evaluationPlot, m_logFunction, m_parent);

            m_experimentTimer.Start();
            Task.Run(() => experimentQueueMonitor.runExperimentsAsync(monitorProgress, receiveJobResults));
        }

        /*protected override void OnDeactivate(bool close)
        {
            if (close)
                experimentQueueMonitor.stopExperiments();
            base.OnDeactivate(close);
        }*/

        public void showReports()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            plotEditor.loadExperimentBatch(m_batchFilename);
            CaliburnUtility.showVMDialog(plotEditor, "Plot editor");
        }
    }
}
