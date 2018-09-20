using Caliburn.Micro;
using System.IO;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class StatViewModel : PropertyChangedBase
    {
        public string ExperimentalUnitConfigFile { get; }
        public string LogBinaryFile { get; }
        public string LogDescriptorFile { get; }
        public string ExperimentId { get; }
        public string TrackId { get; }
        public Stats Stats { get; }

        public StatViewModel(string experimentId, string trackId, Stats stats
            , string logBinaryFile, string logDescriptorFile, string experimentalUnitConfigFile)
        {
            ExperimentId = experimentId;
            TrackId = trackId;
            Stats = stats;
            LogBinaryFile = logBinaryFile;
            LogDescriptorFile = logDescriptorFile;
            ExperimentalUnitConfigFile = experimentalUnitConfigFile;
        }

        public void Export(StreamWriter fileWriter, string leftSpace)
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