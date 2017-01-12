
using Caliburn.Micro;
using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Wpf;
using System.Threading;
using System.Collections.ObjectModel;


namespace Badger.ViewModels
{
    public class Stat
    {
        public string name { get; set; }
        public double min { get; set; }
        public double max { get; set; }
        public double avg { get; set; }
        public double stdDev { get; set; }

        public Stat(string _name,double _min,double _max, double _avg, double _stdDev)
        {
            name = _name;
            min = _min;
            max = _max;
            avg = _avg;
            stdDev = _stdDev;
        }
    }
    public class StatsViewModel: ReportViewModel
    {
        private ObservableCollection<Stat> m_stats = new ObservableCollection<Stat>();
        public ObservableCollection<Stat> stats { get { return m_stats; } set { } }

        public StatsViewModel(string title)
        {
            name= title;
            stats.Add(new Stat("test", 0, 1, 0.5, 0.2));
            stats.Add(new Stat("test-2", 0, 1, 0.5, 0.2));
            
            NotifyOfPropertyChange(() => stats);
        }

    }
}
