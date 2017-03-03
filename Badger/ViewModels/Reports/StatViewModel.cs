using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class StatData
    {
        public double min { get; set; }
        public double max { get; set; }
        public double avg { get; set; }
        public double stdDev { get; set; }
    }
    public class StatViewModel : PropertyChangedBase
    {
        public string name { get; set; }
        public StatData lastEpisodeStats { get; set; }
        public StatData experimentStats { get; set; }
        public string variable { get; set; }

        public StatViewModel(string _name)
        {
            name = _name;
            lastEpisodeStats = new StatData();
            experimentStats = new StatData();
        }
    }
}