using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppXML.ViewModels
{
    public class LoadDialogViewModel:Caliburn.Micro.Screen
    {
        public string FileName { get; set; }
        public LoadDialogViewModel(string fileName)
        {
            FileName = fileName;
        }
        public void Stop()
        {
            System.Threading.Thread.Sleep(500);
            TryClose();
        }
    }
}
