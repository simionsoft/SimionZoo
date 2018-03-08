using System;
using System.Collections.ObjectModel;
using System.IO;
using Caliburn.Micro;
using Badger.Simion;
using System.Diagnostics;

namespace Badger.ViewModels
{

    public class StatsViewModel : PropertyChangedBase
    {

        private ObservableCollection<StatViewModel> m_stats = new ObservableCollection<StatViewModel>();
        public ObservableCollection<StatViewModel> stats { get { return m_stats; } set { } }
        public StatViewModel SelectedStat{get;set;} = null;

        private string m_variable;
        public string variable
        {
            get { return m_variable; }
            set { m_variable = value;  NotifyOfPropertyChange(() => variable); }
        }

        public StatsViewModel(string variableName)
        {
            m_variable = variableName;
        }

        public void addStat(StatViewModel newStat)
        {
            stats.Add(newStat);
        }

        public void export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<" + XMLConfig.statVariableTag 
                + " " + XMLConfig.nameAttribute + "=\"" + variable + "\">");
            foreach(StatViewModel stat in stats)
            {
                fileWriter.WriteLine(leftSpace + "  <" + XMLConfig.statVariableItemTag 
                    + " " + XMLConfig.groupIdAttribute + "=\"" + stat.ExperimentId + "\" "
                    + XMLConfig.trackIdAttribute + "=\"" + stat.TrackId + "\">");
                stat.export(fileWriter, leftSpace + "    ");
                fileWriter.WriteLine(leftSpace + "  </" + XMLConfig.statVariableItemTag + ">");
            }
            fileWriter.WriteLine(leftSpace + "</" + XMLConfig.statVariableTag + ">");
        }

        public void Visualize()
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
    }
}
