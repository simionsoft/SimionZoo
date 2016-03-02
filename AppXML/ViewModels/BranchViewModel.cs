using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
namespace AppXML.ViewModels
{
    public class BranchViewModel:PropertyChangedBase
    {
        private bool _isOptional;
        private string _name;
        private ClassViewModel _class;


        public string IsOptionalVisible 
        { 
            get 
            {
                if (_isOptional)
                    return "Visible";
                else
                    return "Hidden";
            }
            set { }
        }
        public BranchViewModel(string name,string clas,bool isOptional)
        {
            _name = name;
            _class = new ClassViewModel(clas);
        }
        public string Name{get{return _name;}set{}}
        public ClassViewModel Class { get { return _class; } set { } }
        public void removeViews()
        {
            _class.removeViews();
        }
    }
}
