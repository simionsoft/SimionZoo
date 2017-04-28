using System.Diagnostics;
using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using Badger.Data;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ExperimentMonitorViewModel : PropertyChangedBase
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

        private List<ExperimentalUnit> m_pendingExperiments;

        public List<ExperimentalUnit> PendingExperiments
        {
            get { return m_pendingExperiments; }
            set { m_pendingExperiments = value; }
        }

        private Logger.LogFunction m_logFunction;

        public Logger.LogFunction LogFunction
        {
            get { return m_logFunction; }
            set { m_logFunction = value; }
        }

        private string m_batchFilename;

        public string BatchFileName
        {
            get { return m_batchFilename; }
            set { m_batchFilename = value; }
        }

        

        private Window m_parent;


        public ExperimentMonitorViewModel(string batchFilename)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            evaluationPlot.bShowOptions = false;
            evaluationPlot.properties.bLegendVisible = false;
            evaluationPlot.setProperties();

            m_batchFilename = batchFilename;
        }

        /// <summary>
        ///     Runs the selected experiment in the experiment editor.
        /// </summary>
        /// <param name="monitorProgress"></param>
        /// <param name="receiveJobResults"></param>
        public void RunExperiments(bool monitorProgress = true, bool receiveJobResults = true)
        {
            // Clear old LineSeries to avoid confusion on visualization
            evaluationPlot.clearLineSeries();
            // Create the new ExperimentQueue for the selected experiment
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(m_freeHerdAgents, m_pendingExperiments,
                evaluationPlot, m_logFunction, BatchFileName);

            experimentQueueMonitor.ExperimentTimer.Start();
            Task.Run(() => experimentQueueMonitor.runExperimentsAsync(monitorProgress, receiveJobResults));
        }

        /// <summary>
        ///     Stop all experiments in progress and clean up the plot, that is, remove all
        ///     previous LineSeries.
        /// </summary>
        public void StopExperiments()
        {
            experimentQueueMonitor.StopExperiments();
            evaluationPlot.clearLineSeries();
        }

        /// <summary>
        ///     Shows a Report window with the data of the currently finished experiment(s)
        ///     already load and ready to make reports.
        /// </summary>
        public void ShowReports()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            plotEditor.loadExperimentBatch(m_batchFilename);
            CaliburnUtility.showVMDialog(plotEditor, "Plot editor");
        }
    }
}
