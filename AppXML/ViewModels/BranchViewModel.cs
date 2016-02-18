using AppXML.Models;
using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppXML.ViewModels
{
    public class BranchViewModel: PropertyChangedBase
    {
        private string _name;
        private string _class;
        private ClassViewModel _classViewModel;
        public BranchViewModel(CNode node)
        {
            if(node is CBranch)
            {
                CBranch tmp = node as CBranch;
                _name = tmp.name;
                _class = tmp.klass;
                //we need ClassViewModel
                _classViewModel = new ClassViewModel(_class);

            }
        }
        public string Name { get { return _name; } set { _name = value; } }

    }
}
