using System;
using System.Collections.ObjectModel;
using System.IO;
using Caliburn.Micro;

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

        public void export(string outputFolder)
        {
            string outputFilename = outputFolder + "\\" + m_parent.name + ".xml";
            try
            {
                StreamWriter fileWriter = File.CreateText(outputFilename);
                fileWriter.WriteLine("<Stats>");
                foreach(StatViewModel stat in stats)
                {
                    fileWriter.WriteLine("  <Item Name=\"" + stat.name
                        + "\" Variable=\"" + stat.variable + "/>");
                    fileWriter.WriteLine("    <Last-Episode>");
                    fileWriter.WriteLine("      <Avg=\"" + stat.lastEpisodeStats.avg + "\"/>");
                    fileWriter.WriteLine("      <StdDev=\"" + stat.lastEpisodeStats.stdDev + "\"/>");
                    fileWriter.WriteLine("      <Max=\"" + stat.lastEpisodeStats.max + "\"/>");
                    fileWriter.WriteLine("      <Min=\"" + stat.lastEpisodeStats.min + "\"/>");
                    fileWriter.WriteLine("    </Last-Episode>");
                    fileWriter.WriteLine("    <Experiment>");
                    fileWriter.WriteLine("      <Avg=\"" + stat.experimentStats.avg + "\"/>");
                    fileWriter.WriteLine("      <StdDev=\"" + stat.experimentStats.stdDev + "\"/>");
                    fileWriter.WriteLine("      <Max=\"" + stat.experimentStats.max + "\"/>");
                    fileWriter.WriteLine("      <Min=\"" + stat.experimentStats.min + "\"/>");
                    fileWriter.WriteLine("    </Experiment>");
                    fileWriter.WriteLine("  </Item>");
                }
                fileWriter.WriteLine("</Stats>");
                fileWriter.Close();
            }
            catch(Exception ex)
            {
                Console.WriteLine("Error exporting stats file: " + outputFilename);
                Console.Write(ex.ToString());
            }
        }
    }
}
