using Caliburn.Micro;
using System.IO;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class StatViewModel : PropertyChangedBase
    {
        public string ExperimentId { get; set; }
        public string TrackId { get; set; }
        public Stats Stats { get; set; }

        public StatViewModel(string experimentId, string trackId, Stats stats)
        {
            ExperimentId = experimentId;
            TrackId = trackId;
            Stats = stats;
        }

        public void export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<Stats>");
            fileWriter.WriteLine(leftSpace + "  <Avg>" + Stats.avg + "</Avg>");
            fileWriter.WriteLine(leftSpace + "  <StdDev>" + Stats.stdDev + "</StdDev>");
            fileWriter.WriteLine(leftSpace + "  <Max>" + Stats.max + "</Max>");
            fileWriter.WriteLine(leftSpace + "  <Min>" + Stats.min + "</Min>");
            fileWriter.WriteLine(leftSpace + "</Stats>");
        }
    }
}