using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class WindowClassViewModel: PropertyChangedBase
    {

        private ClassViewModel _father;

        public ClassViewModel Class { get { return _father; } set { _father = value; } }

        public WindowClassViewModel(string className)
        {
            _father = new ClassViewModel(className,false);
           
        }
    }
}
