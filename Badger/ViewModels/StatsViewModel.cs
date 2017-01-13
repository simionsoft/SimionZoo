using System;
using System.Collections.ObjectModel;
using System.IO;

namespace Badger.ViewModels
{
    public class Stat
    {
        public string name { get; set; }
        public double min { get; set; }
        public double max { get; set; }
        public double avg { get; set; }
        public double stdDev { get; set; }
        public string variable { get; set; }

        public Stat(string _name,string _variable, double _min,double _max, double _avg, double _stdDev)
        {
            name = _name;
            min = _min;
            max = _max;
            avg = _avg;
            stdDev = _stdDev;
            variable = _variable;
        }
        public Stat(string _name,string _variable)
        {
            name = _name;
            variable = _variable;
        }
    }
    public class StatsViewModel: ReportViewModel
    {
        private ObservableCollection<Stat> m_stats = new ObservableCollection<Stat>();
        public ObservableCollection<Stat> stats { get { return m_stats; } set { } }

        public StatsViewModel(string title)
        {
            name= title;
        }

        public void addStat(Stat newStat)
        {
            stats.Add(newStat);
        }

        public override void export(string outputFolder)
        {
            string outputFilename = outputFolder + "\\" + name + ".xml";
            try
            {
                StreamWriter fileWriter = File.CreateText(outputFilename);
                fileWriter.WriteLine("<Stats>");
                foreach(Stat stat in stats)
                {
                    fileWriter.WriteLine("  <Item Name=\"" + stat.name + "\" Variable=\"" + stat.variable + "\" Avg=\"" + stat.avg
                        + "\" StdDev=\"" + stat.stdDev + "\" Max=\"" + stat.max + "\" Min=\"" + stat.min + "\"/>");
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
