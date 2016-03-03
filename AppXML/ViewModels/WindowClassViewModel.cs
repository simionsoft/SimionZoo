using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Windows.Threading;
using System.Dynamic;
using System.Windows;

namespace AppXML.ViewModels
{
    public class WindowClassViewModel: Screen
    {

        private ClassViewModel _father;
        private ClassViewModel _result;

        public ClassViewModel Class { get { return _father.ResumeClass; } set { _father.ResumeClass = value; } }

        public WindowClassViewModel(string className,ClassViewModel father)
        {
            _father = father;

            if (father.ResumeClass == null)
                _father.ResumeClass = new ClassViewModel(className, false);
            
           
        }
        public void Save()
        {
            
           // _father.ResumeClass = this.Class;
            bool ok = Class.validate();
            if(!ok)
            {
                DialogViewModel dvm = new DialogViewModel(null, "The form is not validated. Do you want to save it?", DialogViewModel.DialogType.YesNo);
                dynamic settings = new ExpandoObject();
                settings.WindowStyle = WindowStyle.ToolWindow;
                settings.ShowInTaskbar = true;
                settings.Title = "WARNING";

                new WindowManager().ShowDialog(dvm, null, settings);   

                if(dvm.DialogResult==DialogViewModel.Result.OK)
                {
                    TryClose();
                }
            }
            else
                TryClose();
        }
    }
}
