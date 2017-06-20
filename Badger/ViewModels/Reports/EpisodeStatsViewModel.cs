using Caliburn.Micro;
using System.IO;

namespace Badger.ViewModels
{
    public class StatData
    {
        public double min { get; set; }
        public double max { get; set; }
        public double avg { get; set; }
        public double stdDev { get; set; }
    }
    public class EpisodeStatsViewModel : PropertyChangedBase
    {
        public StatData statData { get; set; }
        public string episodeId { get; set; }

        public EpisodeStatsViewModel(string episode)
        {
            statData = new StatData();
            episodeId = episode;
        }
        
        public void export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<Episode-"+episodeId+"-Stats>");
            fileWriter.WriteLine(leftSpace + "  <Avg>" + statData.avg + "</Avg>");
            fileWriter.WriteLine(leftSpace + "  <StdDev>" + statData.stdDev + "</StdDev>");
            fileWriter.WriteLine(leftSpace + "  <Max>" + statData.max + "</Max>");
            fileWriter.WriteLine(leftSpace + "  <Min>" + statData.min + "</Min>");
            fileWriter.WriteLine(leftSpace + "</Episode-Stats>");
        }
        
    }
}