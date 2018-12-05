using System.Collections.ObjectModel;
using System.IO;
using System.Diagnostics;
using System.Runtime.Serialization;

using Caliburn.Micro;

using Badger.Data;

using Herd.Files;

namespace Badger.ViewModels
{
    [DataContract]
    public class StatsViewModel : PropertyChangedBase
    {
        [DataMember]
        public ObservableCollection<StatViewModel> Stats { get; set; }= new ObservableCollection<StatViewModel>();
        [DataMember]
        public StatViewModel SelectedStat { get; set; } = null;

        private string m_variable;
        [DataMember]
        public string Variable
        {
            get { return m_variable; }
            set { m_variable = value;  NotifyOfPropertyChange(() => Variable); }
        }

        public StatsViewModel(string variableName)
        {
            m_variable = variableName;
        }

        public void addStat(StatViewModel newStat)
        {
            Stats.Add(newStat);
        }

        public void Export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<" + XMLTags.statVariableTag 
                + " " + XMLTags.nameAttribute + "=\"" + Variable + "\">");
            foreach(StatViewModel stat in Stats)
            {
                fileWriter.WriteLine(leftSpace + "  <" + XMLTags.statVariableItemTag 
                    + " " + XMLTags.groupIdAttribute + "=\"" + stat.ExperimentId + "\" "
                    + XMLTags.trackIdAttribute + "=\"" + stat.TrackId + "\">");
                stat.Export(fileWriter, leftSpace + "    ");
                fileWriter.WriteLine(leftSpace + "  </" + XMLTags.statVariableItemTag + ">");
            }
            fileWriter.WriteLine(leftSpace + "</" + XMLTags.statVariableTag + ">");
        }

        public void VisualizeExperiment()
        {
            Process proc = new Process()
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = Files.logViewerExePath,
                    Arguments = SelectedStat.LogDescriptorFile,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };
            proc.Start();
        }

        public void VisualizeFunctions()
        {
            CaliburnUtility.ShowPopupWindow(new FunctionLogViewModel(SelectedStat.LogDescriptorFile), "Function Log Viewer");
        }

        public void OpenInEditor()
        {
            MainWindowViewModel.Instance.ShowEditorWindow();
            MainWindowViewModel.Instance.EditorWindowVM.LoadExperimentalUnit(SelectedStat.ExperimentalUnitConfigFile);
        }
    }
}
