
using Caliburn.Micro;
using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Wpf;
using System.Threading;
using System.Collections.ObjectModel;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class Stat
    {
        public string name;
        public double min, max, avg, stdDev;
    }
    public class StatsViewModel: ReportViewModel
    {
        ObservableCollection<Stat> stats = new ObservableCollection<Stat>();

        public StatsViewModel(string title)
        {
            name= title;
           
        }

    }
}
