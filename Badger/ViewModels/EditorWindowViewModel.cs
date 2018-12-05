using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

using Badger.Data;

using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class EditorWindowViewModel: Caliburn.Micro.Screen
    {
        public BindableCollection<ExperimentViewModel> ExperimentViewModels
        { get; set; } = new BindableCollection<ExperimentViewModel>();

        private ExperimentViewModel m_selectedExperiment;
        public ExperimentViewModel SelectedExperiment
        {
            get { return m_selectedExperiment; }
            set
            {
                m_selectedExperiment = value;
                NotifyOfPropertyChange(() => SelectedExperiment);
            }
        }

        private bool appNameExists(string name)
        {
            return ExperimentViewModels.Any(app => app.Name == name);
        }

        public string getValidAppName(string originalName)
        {
            int id = 1;
            string newName = originalName;
            bool bNameExists = appNameExists(newName);
            while (bNameExists)
            {
                newName = originalName + "-" + id;
                id++;
                bNameExists = appNameExists(newName);
            }
            return newName;
        }

        private bool m_bCanLaunchExperiment;

        public bool CanLaunchExperiment
        {
            get { return m_bCanLaunchExperiment; }
            set
            {
                m_bCanLaunchExperiment = value;
                NotifyOfPropertyChange(() => CanLaunchExperiment);
            }
        }

        private bool m_bIsExperimentListNotEmpty;

        public bool IsExperimentListNotEmpty
        {
            get { return m_bIsExperimentListNotEmpty; }
            set
            {
                m_bIsExperimentListNotEmpty = value;
                NotifyOfPropertyChange(() => IsExperimentListNotEmpty);
            }
        }

        private void CheckEmptyExperimentList()
        {
            bool wasEmpty = !m_bIsExperimentListNotEmpty;
            if (wasEmpty != (ExperimentViewModels.Count == 0))
            {
                m_bIsExperimentListNotEmpty = ExperimentViewModels.Count != 0;
                NotifyOfPropertyChange(() => IsExperimentListNotEmpty);
            }
        }

        public ObservableCollection<string> AppNames { get; set; } = new ObservableCollection<string>();


        // Key element is the apps name, and the value is the .xml definition file
        private Dictionary<string, string> appDefinitions = new Dictionary<string, string>();

        private string m_selectedAppName;
        public string selectedAppName
        {
            get { return m_selectedAppName; }
            set
            {
                int index = AppNames.IndexOf(value);
                if (index == -1)
                    return;
                m_selectedAppName = value;
                NotifyOfPropertyChange(() => selectedAppName);
            }
        }

        public void NewExperiment()
        {
            if (m_selectedAppName == null) return;

            string xmlDefinitionFile = appDefinitions[m_selectedAppName];
            ExperimentViewModel newExperiment = new ExperimentViewModel(xmlDefinitionFile, null, "New");
            ExperimentViewModels.Add(newExperiment);
            NotifyOfPropertyChange(() => ExperimentViewModels);
            CheckEmptyExperimentList();

            CanLaunchExperiment = m_bIsExperimentListNotEmpty;

            SelectedExperiment = newExperiment;
        }

        public EditorWindowViewModel()
        {
            LoadAppDefinitions();

            // Check for aditional required configuration files
            if (!File.Exists("../config/definitions.xml"))
                CaliburnUtility.ShowWarningDialog("Unable to find required configuration file.", "Fatal Error");
        }


        private void LoadAppDefinitions()
        {
            char[] splitters = new char[] { '\\', '/' };
            foreach (string app in Directory.GetFiles(Files.appConfigRelativeDir))
            {
                //char[] spliter = "\\".ToCharArray();
                string[] tmp = app.Split(splitters);
                tmp = tmp[tmp.Length - 1].Split('.');
                string name = tmp[0];
                appDefinitions.Add(name, app);
                AppNames.Add(name);
            }

            selectedAppName = AppNames[0];
            NotifyOfPropertyChange(() => AppNames);
        }

        public void SaveSelectedExperimentOrProject()
        {
            if (SelectedExperiment == null || !SelectedExperiment.Validate())
            {
                CaliburnUtility.ShowWarningDialog("The app can't be validated. See error log.", "Error");
                return;
            }

            string savedFilename = null;
            if (SelectedExperiment.getNumForkCombinations() > 1)
                Files.SaveExperiments(ExperimentViewModels);
            else
                savedFilename = Files.SaveExperiment(SelectedExperiment);

            if (savedFilename != null)
                SelectedExperiment.Name = Herd.Utils.GetFilename(savedFilename, true, 2);
        }

        //This method can be used from any child window to load experimental units (i.e, from the ReportViewer)
        public void LoadExperimentalUnit(string experimentalUnitConfigFile)
        {
            ExperimentViewModel newExperiment =
                        Files.LoadExperiment(appDefinitions, experimentalUnitConfigFile);

            if (newExperiment != null)
            {
                ExperimentViewModels.Add(newExperiment);

                SelectedExperiment = newExperiment;
            }
        }

        public void LoadExperimentOrProject()
        {
            string extension = Files.ProjectDescription;
            string filter = Files.ExperimentOrProjectFilter;

            List<string> filenames = Files.OpenFileDialogMultipleFiles(extension, filter);
            foreach (string filename in filenames)
            {
                string fileExtension = Herd.Utils.GetExtension(filename, 2);
                if (fileExtension == Herd.Files.Extensions.ExperimentExtension)
                {
                    ExperimentViewModel newExperiment =
                        Files.LoadExperiment(appDefinitions, filename);

                    if (newExperiment != null)
                    {
                        ExperimentViewModels.Add(newExperiment);

                        SelectedExperiment = newExperiment;
                    }

                }
                else if (fileExtension == Herd.Files.Extensions.ProjectExtension)
                {
                    BindableCollection<ExperimentViewModel> newExperiments = new BindableCollection<ExperimentViewModel>();
                    Files.LoadExperiments(ref newExperiments, appDefinitions
                        , MainWindowViewModel.Instance.LogToFile
                        , filename);
                    ExperimentViewModels.AddRange(newExperiments);

                    if (ExperimentViewModels.Count > 0)
                        SelectedExperiment = ExperimentViewModels[0];
                }
            }

            CheckEmptyExperimentList();

            CanLaunchExperiment = m_bIsExperimentListNotEmpty;
        }

        public void ClearExperiments()
        {
            SelectedExperiment = null;
            if (ExperimentViewModels != null) ExperimentViewModels.Clear();
        }

        /// <summary>
        ///     Close a tab (experiment view) and remove it from experiments list.
        /// </summary>
        /// <param name="e">The experiment to be removed</param>
        public void Close(ExperimentViewModel e)
        {
            ExperimentViewModels.Remove(e);
            CheckEmptyExperimentList();

            CanLaunchExperiment = (m_bIsExperimentListNotEmpty);
        }

        /// <summary>
        ///     Runs locally the experiment with its currently selected parameters
        /// </summary>
        /// <param name="experiment">The experiment to be run</param>
        public void RunExperimentalUnitLocallyWithCurrentParameters(ExperimentViewModel experiment)
        {
            experiment.RunLocallyCurrentConfiguration();
        }

        /// <summary>
        ///     Shows the wires defined in the current experiment
        /// </summary>
        /// <param name="experiment">The selected experiment</param>
        public void ShowWires(ExperimentViewModel experiment)
        {
            experiment.ShowWires();
        }


        private List<LoggedExperimentViewModel> m_loggedExperiments = new List<LoggedExperimentViewModel>();

        public List<LoggedExperimentViewModel> LoggedExperiments
        {
            get { return m_loggedExperiments; }
            set { m_loggedExperiments = value; NotifyOfPropertyChange(() => LoggedExperiments); }
        }


        /// <summary>
        ///     Runs all the experiments open in the editor. Saves a batch file read by the experiment monitor, and also a project to be
        ///     able to modify the experiment and rerun it later
        /// </summary>
        public void RunExperimentsInEditor()
        {
            //Validate all the experiments
            foreach (ExperimentViewModel experiment in ExperimentViewModels)
            {
                if (!experiment.Validate())
                {
                    CaliburnUtility.ShowWarningDialog("The app can't be validated. See error log.", "Error");
                    return;
                }
            }

            //Ask the user where to save the batch
            string batchFileName;
            //Save dialog -> returns the experiment batch file
            string suggestedBatchFileName = SelectedExperiment.Name;
            var sfd = Files.SaveFileDialog(Files.ExperimentBatchDescription, Files.ExperimentBatchFilter, suggestedBatchFileName);
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                batchFileName = sfd.FileName;
            }
            else
            {
                MainWindowViewModel.Instance.LogToFile("Error saving the experiment queue");
                return;
            }

            //Save the experiment batch while showing a progress bar dialog window
            ProgressBarDialogViewModel progressBarDialogVM = new ProgressBarDialogViewModel("Saving", "Saving the experiment batch");

            CancellationTokenSource cancellation = new CancellationTokenSource();
            Task.Run(() =>
            {
                //Save the experiment batch, read by the Experiment Monitor
                int experimentalUnitsCount = Files.SaveExperimentBatchFile(ExperimentViewModels,
                    batchFileName, MainWindowViewModel.Instance.LogToFile
                    , progressBarDialogVM.UpdateProgressBar);

                //Save the badger project to allow later changes and re-runs of the experiment
                string badgerProjFileName = Herd.Utils.RemoveExtension(batchFileName, Herd.Utils.NumParts(Herd.Files.Extensions.ProjectExtension, '.'))
                    + Herd.Files.Extensions.ProjectExtension;
                Files.SaveExperiments(ExperimentViewModels, badgerProjFileName);

                progressBarDialogVM.TryClose();
            }, cancellation.Token);

            CaliburnUtility.ShowPopupWindow(progressBarDialogVM, "Saving");

            if (progressBarDialogVM.Cancelled)
            {
                MainWindowViewModel.Instance.LogToFile("Experiment batch saving operation was cancelled by the user");
                cancellation.Cancel();
                return;
            }

            //Experiments are sent and executed remotely
            MainWindowViewModel.Instance.LogToFile("Running experiment queue remotely");

            MainWindowViewModel.Instance.ShowExperimentMonitor();

            MainWindowViewModel.Instance.MonitorWindowVM.LoadExperimentBatch(batchFileName);
        }
    }
}
