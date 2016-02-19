using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Collections.ObjectModel;
using AppXML.Models;
using System.Xml;
namespace AppXML.ViewModels
{
    public class MultiValuedViewModel: PropertyChangedBase
    {
        private ObservableCollection<IntegerViewModel> _aded;
        private IntegerViewModel _header;
        private CIntegerValue original;
        private string label;

        public MultiValuedViewModel(string label, string clas)
        {
            if (CNode.definitions != null)
            {
                if (CNode.definitions.ContainsKey(clas))
                {
                    //es un multi de clase o un enum
                    XmlNode nodo = CNode.definitions[clas];
                    if(nodo.Name.StartsWith("ENUMERATION"))
                    {
                        original = new CIntegerValue(nodo); 
                        _header = new IntegerViewModel(label, original);
                        this.label = label;
                    }
                    else
                    {
                        //es una clase
                    }
                }
                else if (clas != "DECIMAL")
                {
                    //es un integervalue
                    original = new CIntegerValue(clas);
                    _header = new IntegerViewModel(label, original);
                    this.label = label;
                }
                else
                {
                    //es un decimal
                }
            }
           
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
