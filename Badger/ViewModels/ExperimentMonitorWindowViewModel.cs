using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using Badger.Data;
using Badger.Simion;
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

        public Logger.LogFunction LogFunction { get; set; }

        public string BatchFileName { get; set; }

        ShepherdViewModel m_shepherd;
        /// <summary>
        ///     Constructor.
        /// </summary>
        /// <param name="logFunction"></param>
        /// <param name="batchFileName"></param>
        public ExperimentMonitorWindowViewModel(ShepherdViewModel shepherd, Logger.LogFunction logFunction)
        {
            m_shepherd = shepherd;
            EvaluationPlot = new PlotViewModel("Evaluation Episodes", 1.0, "Normalized Evaluation Episode", "Average Reward") { bShowOptions = false };
            EvaluationPlot.Plot.TitleFontSize = 14;
            EvaluationPlot.Properties.LegendVisible = false;

            LogFunction = logFunction;

            // Create the new ExperimentQueue for the selected experiment
            ExperimentQueueMonitor = new ExperimentQueueMonitorViewModel(EvaluationPlot, LogFunction);
        }

        bool m_bIsRunning = false;
        public bool IsRunning
        {
            get { return m_bIsRunning; }
            set { m_bIsRunning = value; NotifyOfPropertyChange(() => IsRunning); }
        }

        ///<summary>
        ///Deletes all the log files in the batch
        ///</summary>
        public void CleanExperimentBatch()
        {
            SimionFileData.LoadExperimentBatchFile(BatchFileName, SimionFileData.CleanExperimentalUnitLogFiles);
        }

        public void LoadExperimentBatch(string batchFileName)
        {
            BatchFileName = batchFileName;
            // Clear old LineSeries to avoid confusion on visualization
            EvaluationPlot.ClearLineSeries();

            if (!ExperimentQueueMonitor.LoadBatchFile(BatchFileName))
            {
                CaliburnUtility.ShowWarningDialog("Failed to initialize the experiment batch", "Error");
                return;
            }
        }

        List<HerdAgentViewModel> getFreeHerdAgentList()
        {
            if (m_shepherd.HerdAgentList.Count == 0)
            {
                CaliburnUtility.ShowWarningDialog(
                    "No Herd agents were detected, so experiments cannot be sent. " +
                    "Consider starting the local agent: \"net start HerdAgent\"", "No agents detected");
                return null;
            }

            List<HerdAgentViewModel> freeHerdAgents = new List<HerdAgentViewModel>();

            // Get available herd agents list. Inside the loop to update the list
            m_shepherd.getAvailableHerdAgents(ref freeHerdAgents);

            if (freeHerdAgents.Count == 0)
            {
                CaliburnUtility.ShowWarningDialog(
                    "There is no herd agents availables at this moment. Make sure you have selected at " +
                    "least one available agent and try again.", "No agents detected");
                return null;
            }
            return freeHerdAgents;
        }

        /// <summary>
        ///     Runs the selected experiment in the experiment editor.
        /// </summary>
        public void RunExperimentBatch()
        {
            List<HerdAgentViewModel> freeHerdAgents= getFreeHerdAgentList();
            IsRunning = true;
            ExperimentQueueMonitor.ExperimentTimer.Start();
            Task.Run(() => ExperimentQueueMonitor.RunExperimentsAsync(freeHerdAgents));
            IsRunning = false;
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
            if (close && ExperimentQueueMonitor!=null)
                ExperimentQueueMonitor.StopExperiments();
            base.OnDeactivate(close);
        }
    }
}
