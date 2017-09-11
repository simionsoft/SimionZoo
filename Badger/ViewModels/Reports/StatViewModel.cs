using Caliburn.Micro;
using System.IO;

namespace Badger.ViewModels
{
    public class StatData
    {
        public double min { get; set; }
        public double max { get; set; }
        public double avg { get; set; }
        public double absAvg { get; set; }
        public double stdDev { get; set; }
    }
    public class StatViewModel : PropertyChangedBase
    {
        public string experimentId { get; set; }
        public string trackId { get; set; }
        public StatData lastEpisodeStats { get; set; }
        public StatData experimentStats { get; set; }
        public string variable { get; set; }

        public StatViewModel(string exp,string track)
        {
            experimentId = exp;
            trackId = track;
            lastEpisodeStats = new StatData();
            experimentStats = new StatData();
        }

        public void export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<Last-Episode-stats>");
            fileWriter.WriteLine(leftSpace + "  <Avg>" + lastEpisodeStats.avg + "</Avg>");
            fileWriter.WriteLine(leftSpace + "  <AbsAvg>" + lastEpisodeStats.absAvg + "</AbsAvg>");
            fileWriter.WriteLine(leftSpace + "  <StdDev>" + lastEpisodeStats.stdDev + "</StdDev>");
            fileWriter.WriteLine(leftSpace + "  <Max>" + lastEpisodeStats.max + "</Max>");
            fileWriter.WriteLine(leftSpace + "  <Min>" + lastEpisodeStats.min + "</Min>");
            fileWriter.WriteLine(leftSpace + "</Last-Episode-stats>");
            fileWriter.WriteLine(leftSpace + "<Experiment-stats>");
            fileWriter.WriteLine(leftSpace + "  <Avg>" + experimentStats.avg + "</Avg>");
            fileWriter.WriteLine(leftSpace + "  <AbsAvg>" + experimentStats.absAvg + "</AbsAvg>");
            fileWriter.WriteLine(leftSpace + "  <StdDev>" + experimentStats.stdDev + "</StdDev>");
            fileWriter.WriteLine(leftSpace + "  <Max>" + experimentStats.max + "</Max>");
            fileWriter.WriteLine(leftSpace + "  <Min>" + experimentStats.min + "</Min>");
            fileWriter.WriteLine(leftSpace + "</Experiment-stats>");
        }
    }
}