using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class WindowClassViewModel: Screen
    {

        private ClassViewModel _father;
        private ClassViewModel _result;

        public ClassViewModel Class { get { return _result; } set { _result = value; } }

        public WindowClassViewModel(string className,ClassViewModel father)
        {
            _father = father;

            if (father.ResumeClass == null)
                _result = new ClassViewModel(className, false);
            else
                _result = father.ResumeClass;
           
        }
        public void Save()
        {
            _father.ResumeClass = this.Class;
            TryClose();
        }
    }
}
