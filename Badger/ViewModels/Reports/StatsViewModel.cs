using System;
using System.Collections.ObjectModel;
using System.IO;
using Caliburn.Micro;
using Badger.Simion;

namespace Badger.ViewModels
{
  
    public class StatsViewModel: PropertyChangedBase
    {
        
        private ObservableCollection<StatViewModel> m_stats = new ObservableCollection<StatViewModel>();
        public ObservableCollection<StatViewModel> stats { get { return m_stats; } set { } }

        private ReportViewModel m_parent;
        private string m_variable;
        public string variable
        {
            get { return m_variable; }
            set { m_variable = value;  NotifyOfPropertyChange(() => variable); }
        }

        public StatsViewModel(string variableName,ReportViewModel parent)
        {
            m_variable = variableName;
            m_parent = parent;
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
                    + " " + XMLConfig.groupIdAttribute + "=\"" + stat.groupId + "\" "
                    + XMLConfig.trackIdAttribute + "=\"" + stat.trackId + "\">");
                stat.export(fileWriter, leftSpace + "    ");
                fileWriter.WriteLine(leftSpace + "  </" + XMLConfig.statVariableItemTag + ">");
            }
            fileWriter.WriteLine(leftSpace + "</" + XMLConfig.statVariableTag + ">");
        }

        public void addOrganized(StatViewModel newStat)
        {
            int paramIndex = 0;
            string[] parameters = newStat.groupIdParameters();
            int i = 0;
            bool inRange = false;
            foreach (StatViewModel stat in stats)
            {
                if (inRange && !parameters[paramIndex].Equals(stat.groupIdParameters()[paramIndex]))
                {
                    stats.Insert(i, newStat);
                    return;
                }
                if (parameters[paramIndex].Equals(stat.groupIdParameters()[paramIndex]))
                {
                    inRange = true;
                }
                i++;
            }
            stats.Add(newStat);   
        }
    }
}
