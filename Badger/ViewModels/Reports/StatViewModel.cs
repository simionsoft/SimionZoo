using Caliburn.Micro;
using System.IO;
using Badger.Simion;
using System.Runtime.Serialization;

namespace Badger.ViewModels
{
    [DataContract]
    public class StatViewModel : PropertyChangedBase
    {
        [DataMember]
        public string ExperimentalUnitConfigFile { get; set; }
        [DataMember]
        public string LogBinaryFile { get; set; }
        [DataMember]
        public string LogDescriptorFile { get; set; }
        [DataMember]
        public string ExperimentId { get; set; }
        [DataMember]
        public string TrackId { get; set; }
        [DataMember]
        public Stats Stats { get; set; }

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