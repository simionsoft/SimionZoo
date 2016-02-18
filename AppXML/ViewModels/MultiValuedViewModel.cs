using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Collections.ObjectModel;
using AppXML.Models;
namespace AppXML.ViewModels
{
    public class MultiValuedViewModel: PropertyChangedBase
    {
        private ObservableCollection<IntegerViewModel> _aded;
        private IntegerViewModel _header;
        private CIntegerValue original;
        private string label;

        public MultiValuedViewModel()
        {
            label = "etiqueta";
            CNode root = AppXML.Data.Utility.getRootNode("../config/RLSimion.xml");
            original = CNode.getInstance(CNode.definitions["VFA-Learner"].ChildNodes[1]) as CIntegerValue;
            _header = new IntegerViewModel(label, original);

            _aded = new ObservableCollection<IntegerViewModel>();

        }

        public IntegerViewModel Header { get { return _header; } set { } }
        
        public ObservableCollection<IntegerViewModel> Aded 
        {
            get { return _aded; }
            set
            {
                _aded = value;
                NotifyOfPropertyChange(() => Aded ); 
            }
        }
        public void Delete(IntegerViewModel delete)
        {
            _aded.Remove(delete);
            NotifyOfPropertyChange(() => Aded);

        }
          public void AddNew()
        {
       
            
            IntegerViewModel t = new IntegerViewModel(label, original);
            _aded.Add(t);


        }
    }
}
