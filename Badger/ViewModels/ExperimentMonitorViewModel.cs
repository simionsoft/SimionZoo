using System.Diagnostics;
using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using Badger.Data;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ExperimentMonitorViewModel : Window
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

        public List<HerdAgentViewModel> FreeHerdAgents { get; set; }

        public Logger.LogFunction LogFunction { get; set; }

        public string BatchFileName { get; set; }

        private double m_globalProgress;
        public double GlobalProgress
        {
            get { return m_globalProgress; }
            set
            {
                m_globalProgress = value;
                NotifyOfPropertyChange(() => GlobalProgress);
            }
        }

        public Stopwatch ExperimentTimer { get; set; }

        private string m_estimatedEndTimeText = "";
        public string estimatedEndTime
        {
            get { return m_estimatedEndTimeText; }
            set
            {
                m_estimatedEndTimeText = value;
                NotifyOfPropertyChange(() => estimatedEndTime);
            }
        }

        /// <summary>
        ///     Constructor.
        /// </summary>
        /// <param name="freeHerdAgents"></param>
        /// <param name="logFunction"></param>
        /// <param name="batchFileName"></param>
        public ExperimentMonitorViewModel(List<HerdAgentViewModel> freeHerdAgents,
            Logger.LogFunction logFunction, string batchFileName)
        {
            evaluationPlot = new PlotViewModel("Evaluation episodes");
            evaluationPlot.bShowOptions = false;
            evaluationPlot.properties.bLegendVisible = false;
            evaluationPlot.setProperties();

            ExperimentTimer = new Stopwatch();

            FreeHerdAgents = freeHerdAgents;
            LogFunction = logFunction;
            BatchFileName = batchFileName;
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
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(FreeHerdAgents, evaluationPlot,
                LogFunction, BatchFileName, this);

            ExperimentTimer.Start();
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
        ///     Stops all experiments on Experiment Monitor window close.
        /// </summary>
        /// <param name="close"></param>
        protected override void OnDeactivate(bool close)
        {
            if (close)
                experimentQueueMonitor.StopExperiments();
            base.OnDeactivate(close);
        }

        /// <summary>
        ///     Express progress as a percentage unit to fill the global progress bar.
        /// </summary>
        public void updateGlobalProgress()
        {
            GlobalProgress = experimentQueueMonitor.calculateGlobalProgress();

            if (GlobalProgress > 0.0 && GlobalProgress < 100.0)
                estimatedEndTime = "Estimated time to end: "
                    + System.TimeSpan.FromSeconds(ExperimentTimer.Elapsed.TotalSeconds
                    * ((100 - GlobalProgress) / GlobalProgress)).ToString(@"hh\:mm\:ss");
            else
                estimatedEndTime = "";
        }

        /// <summary>
        ///     Shows a Report window with the data of the currently finished experiment(s)
        ///     already load and ready to make reports.
        /// </summary>
        public void ShowReports()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            plotEditor.loadExperimentBatch(BatchFileName);
            CaliburnUtility.ShowPopupWindow(plotEditor, "Plot editor");
        }
    }
}
