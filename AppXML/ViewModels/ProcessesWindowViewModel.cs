using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace AppXML.ViewModels
{
    public class ProcessesWindowViewModel: Screen
    {
        public ProcessManagerViewModel Manager { get; set; }
        public bool isOver { get; set; }


        public ProcessesWindowViewModel()
        {
            Manager = new ProcessManagerViewModel();
            isOver = false;
        }
        public ProcessesWindowViewModel(List<ProcessStateViewModel> processes)
        {
            Manager = new ProcessManagerViewModel(processes);
            isOver = false;
        }
        public void Close()
        {
            if (isOver)
                TryClose();
                
        }
        public void addProcess(ProcessStateViewModel process)
        {
            Manager.addProcess(process);
        }
        public void addProcess(List<ProcessStateViewModel> processes)
        {
            Manager.addProcess(processes);
        }
    }
}
