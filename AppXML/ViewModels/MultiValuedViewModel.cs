using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Collections.ObjectModel;
using AppXML.Models;
using System.Xml;
using System.Windows.Controls;
namespace AppXML.ViewModels
{
    public class MultiValuedViewModel: PropertyChangedBase
    {
        private ObservableCollection<IntegerViewModel> _aded;
        private IntegerViewModel _header;
        private CIntegerValue original;
        private string label;
        private bool _isOptional;
        private bool _isNull = false;
        private string _comment;
        public string Comment { get { return _comment; } set { } }
        private XmlDocument _doc;
        public bool IsNull { get { return _isNull; } set { _isNull = value; NotifyOfPropertyChange(() => IsNull); NotifyOfPropertyChange(() => IsEnabled); } }
        public bool IsEnabled { get { return !IsNull; } set {} }
        private ClassViewModel _headerClass;
        private ObservableCollection<ClassViewModel> _adedClasses;
        private string clas;
        private string tag;
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
        public string simpleVisible { get { 
            if (_aded != null)
                return "Visible"; 
            else 
                return "Hidden"; }
            set { }
        }
        public string classVisible
        {
            get
            {
                if (_headerClass != null)
                    return "Visible";
                else
                    return "Hidden";
            }
            set { }
        }
        public string Label { get { return label; } set { } }

        public MultiValuedViewModel(string label, string clas,string comment ,bool isOptional, XmlDocument doc,string tag)
        {
            _comment = comment;
            _doc = doc;
            _isOptional = isOptional;
            if (tag == null || tag == "")
                this.tag = label;
            else
                this.tag = tag;
            if (CNode.definitions != null)
            {
                if (CNode.definitions.ContainsKey(clas))
                {
                    //es un multi de clase o un enum
                    XmlNode nodo = CNode.definitions[clas];
                    if(nodo.Name.StartsWith("ENUMERATION"))
                    {
                        original = new CIntegerValue(nodo); 
                        _header = new IntegerViewModel(label, original,_doc,tag);
                        this.label = label;
                        _aded = new ObservableCollection<IntegerViewModel>();

                    }
                    else
                    {
                        //es una clase
                        this.label = label;
                        this.clas = clas;
                        //List<ClassViewModel> list = new List<ClassViewModel>();
                        //list.Add(new ClassViewModel(clas));
                        _headerClass = new ClassViewModel(clas,doc);
                       //new ObservableCollection<ClassViewModel>(list);
                        _adedClasses = new ObservableCollection<ClassViewModel>();

                    }
                }
                else 
                {
                    //es un integervalue
                    original = new CIntegerValue(clas);
                    _header = new IntegerViewModel(label, original,_doc,tag);
                    this.label = label;
                    _aded = new ObservableCollection<IntegerViewModel>();
                }
                
            }
           
           

        }
        
        public IntegerViewModel Header { get { return _header; } set { } }

        public ClassViewModel HeaderClass { get { return _headerClass; } set { } }
        public ObservableCollection<ClassViewModel> AdedClasses { get { return _adedClasses; } set { _adedClasses = value; NotifyOfPropertyChange(()=> AdedClasses); } }

        public ObservableCollection<IntegerViewModel> Aded 
        {
            get { return _aded; }
            set
            {
                _aded = value;
                NotifyOfPropertyChange(() => Aded ); 
            }
        }
        public String RowCount { get { if (AdedClasses != null)return "" + AdedClasses.Count; else return "0"; } set { } }
        public void Delete(IntegerViewModel delete)
        {
            _aded.Remove(delete);
            NotifyOfPropertyChange(() => Aded);

        }
        public void DeleteClass(ClassViewModel delete)
        {
            _adedClasses.Remove(delete);
            /*int index = delete.index;
            for (int i = index; i < _adedClasses.Count;i++)
            {
                _adedClasses[i].index--;
            }*/
            if (_adedClasses.Count == 0)
                NotifyOfPropertyChange(() => DeleteVisible);
            NotifyOfPropertyChange(() => AdedClasses);

        }
        public void AddNew()
        {
           /* if (_aded.Count > 0)
            {
                var x = _aded[_aded.Count - 1];
                if (x.ComboBox != null)
                {

                }
                else if (x.TextBox != null)
                {
                    string valor = x.TextBox[0].Default;
                    original.defaultValue = valor;
                }
                else if (x.TextBoxFile != null)
                {
                    string valor = x.TextBoxFile[0].Default;
                    original.defaultValue = valor;
                }
            }
            else*/
            {
                var x = Header;
                if (x.ComboBox != null)
                {

                }
                else if (x.TextBox != null) 
                {
                    string valor =  x.TextBox[0].Default;
                    original.defaultValue = valor;
                }
                else if(x.TextBoxFile !=null)
                {
                    string valor = x.TextBoxFile[0].Default;
                    original.defaultValue = valor;
                }
            }
            IntegerViewModel t = new IntegerViewModel(label, original,_doc,tag);
            _aded.Add(t);
        }
        public void DeleteLast()
        {
            _adedClasses.RemoveAt(_adedClasses.Count - 1);
            if (_adedClasses.Count == 0)
                NotifyOfPropertyChange(() => DeleteVisible);
        }
        public void Add()
        {
            ClassViewModel cvm = new ClassViewModel(this.clas,_doc);
            //int index = _adedClasses.Count;
            //_adedClasses.Add(new ClassViewWithIndex(cvm,index));
            _adedClasses.Add(cvm);
            if (_adedClasses.Count == 1)
                NotifyOfPropertyChange(() => DeleteVisible);
           
        }
        public void removeViews()
        {
            if (this._headerClass != null)
                _headerClass.removeViews();
            if(this._adedClasses!=null)
            {
                foreach(ClassViewModel clas in _adedClasses)
                {
                    clas.removeViews();
                }
            }
        }
        public void Click(Object sender)
        {
            CheckBox cb = sender as CheckBox;
            if (Convert.ToBoolean(cb.IsChecked))
            {
                IsNull = true;
            }
            else
                IsNull = false;
            

        }
        public String DeleteVisible { get { if (_adedClasses != null && _adedClasses.Count > 0) return "Visible"; else return "Hidden"; } set { } }

        public bool validate()
        {
            if(_isNull)
                return true;
            if(HeaderClass!=null)
            {
               
                if (!HeaderClass.validate())
                    return false;
                foreach(ClassViewModel cvm in AdedClasses)
                {
                    if (!cvm.validate())
                        return false;
                }
                
            }
            if(Header!=null)
            {
                if (!Header.validateIntegerViewModel())
                    return false;
                foreach(IntegerViewModel ivm in Aded)
                {
                    if (!ivm.validateIntegerViewModel())
                        return false;
                }
                
            }
            return true;
        }
        public bool cleanAndValidate()
        {
            if(_isOptional && _isNull)
            {
                this.AdedClasses = null;
                this.HeaderClass = null;
            }
            return validate();
        }

        internal List<XmlNode> getXmlNode()
        {
            if (_isNull)
                return null;
            List<XmlNode> nodes = new List<XmlNode>();
            if(HeaderClass!=null)
            {
                XmlNode nodo = AppXML.Data.Utility.resolveTag(tag, _doc);
                XmlNode lastChild = AppXML.Data.Utility.getLastChild(nodo);
                List<XmlNode> nodos = HeaderClass.getXmlNodes();
                foreach (XmlNode child in nodos)
                    lastChild.AppendChild(child);
                nodes.Add(nodo);
                if(AdedClasses!=null)
                {
                    foreach(ClassViewModel cvm in AdedClasses)
                    {
                        XmlNode nodo2 = AppXML.Data.Utility.resolveTag(tag, _doc);
                        XmlNode lastChild2 = AppXML.Data.Utility.getLastChild(nodo2);
                        List<XmlNode> nodos2 = HeaderClass.getXmlNodes();
                        foreach (XmlNode child in nodos2)
                            lastChild2.AppendChild(child);
                        nodes.Add(nodo2);
                    }
                }
            }
            else
            {
                nodes.Add(Header.getXmlNode());
                if(Aded!=null)
                {
                    foreach(IntegerViewModel ivm in Aded)
                    {
                        nodes.Add(ivm.getXmlNode());
                    }
                }
            }
            return nodes;
        }
    }
   
}
