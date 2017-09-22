using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using Badger.Data;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ExperimentMonitorWindowViewModel : Screen
    {
        private ExperimentQueueMonitorViewModel m_experimentQueueMonitorViewModel;

        public ExperimentQueueMonitorViewModel ExperimentQueueMonitor
        {
            get { return m_experimentQueueMonitorViewModel; }
            set
            {
                m_experimentQueueMonitorViewModel = value;
                NotifyOfPropertyChange(() => ExperimentQueueMonitor);
            }
        }

        public PlotViewModel EvaluationPlot { get; set; }

        public List<HerdAgentViewModel> FreeHerdAgents { get; set; }

        public Logger.LogFunction LogFunction { get; set; }

        public string BatchFileName { get; set; }

        /// <summary>
        ///     Constructor.
        /// </summary>
        /// <param name="freeHerdAgents"></param>
        /// <param name="logFunction"></param>
        /// <param name="batchFileName"></param>
        public ExperimentMonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents,
            Logger.LogFunction logFunction, string batchFileName)
        {
            EvaluationPlot = new PlotViewModel("Evaluation Episodes", 1.0, "Normalized Evaluation Episode", "Average Reward") { bShowOptions = false };
            EvaluationPlot.Plot.TitleFontSize = 14;
            EvaluationPlot.Properties.LegendVisible = false;
            EvaluationPlot.SetProperties();

            FreeHerdAgents = freeHerdAgents;
            LogFunction = logFunction;
            BatchFileName = batchFileName;
        }

        /// <summary>
        ///     Runs the selected experiment in the experiment editor.
        /// </summary>
        /// <param name="monitorProgress"></param>
        /// <param name="receiveJobResults"></param>
        public bool RunExperiments(bool monitorProgress = true, bool receiveJobResults = true)
        {
            // Clear old LineSeries to avoid confusion on visualization
            EvaluationPlot.ClearLineSeries();
            // Create the new ExperimentQueue for the selected experiment
            ExperimentQueueMonitor = new ExperimentQueueMonitorViewModel(FreeHerdAgents, EvaluationPlot,
                LogFunction);

            if (!ExperimentQueueMonitor.InitializeExperiments(BatchFileName))
                return false;

            ExperimentQueueMonitor.ExperimentTimer.Start();
            Task.Run(() => ExperimentQueueMonitor.RunExperimentsAsync(monitorProgress, receiveJobResults));

            return true;
        }

        /// <summary>
        ///     Stop all experiments in progress and clean up the plot, that is, remove all
        ///     previous LineSeries.
        /// </summary>
        public void StopExperiments()
        {
            ExperimentQueueMonitor.StopExperiments();
            EvaluationPlot.ClearLineSeries();
        }

        /// <summary>
        ///     Shows a Report window with the data of the currently finished experiment(s)
        ///     already load and ready to make reports.
        /// </summary>
        public void ShowReports()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            plotEditor.LoadExperimentBatch(BatchFileName);
            CaliburnUtility.ShowPopupWindow(plotEditor, "Plot editor");
        }

        /// <summary>
        ///     Stops all experiments on Experiment Monitor window close.
        /// </summary>
        /// <param name="close"></param>
        protected override void OnDeactivate(bool close)
        {
            if (close)
                ExperimentQueueMonitor.StopExperiments();
            base.OnDeactivate(close);
        }
    }
}
