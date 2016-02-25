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
        private string _name;
        private ClassViewModel _class;
        public BranchViewModel()
        {
            AppXML.Data.Utility.getRootNode("../config/RLSimion.xml");
            _name = "World";
            _class = new ClassViewModel("EXPERIMENT");
        }
        public BranchViewModel(string name,string clas)
        {
            _name = name;
            _class = new ClassViewModel(clas);
        }
        public string Name{get{return _name;}set{}}
        public ClassViewModel Class { get { return _class; } set { } }
    }
}
