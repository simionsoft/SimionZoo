using System;
using System.Collections.ObjectModel;
using System.IO;
using Caliburn.Micro;
using Badger.Simion;
using System.Diagnostics;
using Badger.Data;
using System.Runtime.Serialization;

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
            fileWriter.WriteLine(leftSpace + "<" + XMLConfig.statVariableTag 
                + " " + XMLConfig.nameAttribute + "=\"" + Variable + "\">");
            foreach(StatViewModel stat in Stats)
            {
                fileWriter.WriteLine(leftSpace + "  <" + XMLConfig.statVariableItemTag 
                    + " " + XMLConfig.groupIdAttribute + "=\"" + stat.ExperimentId + "\" "
                    + XMLConfig.trackIdAttribute + "=\"" + stat.TrackId + "\">");
                stat.Export(fileWriter, leftSpace + "    ");
                fileWriter.WriteLine(leftSpace + "  </" + XMLConfig.statVariableItemTag + ">");
            }
            fileWriter.WriteLine(leftSpace + "</" + XMLConfig.statVariableTag + ">");
        }

        public void VisualizeExperiment()
        {
            Process proc = new Process()
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = SimionFileData.logViewerExePath,
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
            MainWindowViewModel.m_mainWindowInstance.LoadExperimentalUnit(SelectedStat.ExperimentalUnitConfigFile);
        }
    }
}
