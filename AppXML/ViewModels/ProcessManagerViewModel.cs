using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppXML.ViewModels
{
    public class ProcessManagerViewModel:Caliburn.Micro.PropertyChangedBase
    {
        private ObservableCollection<ProcessStateViewModel> _processes;
        public ObservableCollection<ProcessStateViewModel> Processes
        {
            get { return _processes; }
            set
            {
                _processes = value;
                NotifyOfPropertyChange(() => Processes);
            }
        }
        public ProcessManagerViewModel()
        {
            _processes = new ObservableCollection<ProcessStateViewModel>();
            //ProcessStateViewModel p1 = new ProcessStateViewModel("primero");
            //ProcessStateViewModel p2 = new ProcessStateViewModel();
            //_processes.Add(p1);
            //_processes.Add(p2);
        }
        public ProcessManagerViewModel(List<ProcessStateViewModel> processes)
        {
            _processes = new ObservableCollection<ProcessStateViewModel>(processes);
            
        }
        public void addProcess(ProcessStateViewModel process)
        {
            _processes.Add(process);
            NotifyOfPropertyChange(() => Processes);
        }
        public void addProcess(List<ProcessStateViewModel> processes)
        {
            foreach(ProcessStateViewModel process in processes)
                _processes.Add(process);
            NotifyOfPropertyChange(() => Processes);
        }
    }
}
