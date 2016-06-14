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
    public class MultiValuedViewModel: ValidableAndNodeViewModel
    {
        private ObservableCollection<IntegerViewModel> _added;
        private IntegerViewModel _header;
        private CIntegerValue original;
        private string label;
        private string _comment;
        public string Comment { get { return _comment; } set { } }
        private XmlDocument _doc;
        private ClassViewModel _headerClass;
        private ObservableCollection<ClassViewModel> _addedClasses;
        private string m_class;
        private string tag;
        public string Tag { get { return tag; } set { } }
       
        public string simpleVisible { get { 
            if (_added != null)
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

        public MultiValuedViewModel(string label, string clas,string comment , XmlDocument doc,string tag)
        {
            _comment = comment;
            _doc = doc;
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
                        _added = new ObservableCollection<IntegerViewModel>();

                    }
                    else
                    {
                        //es una clase
                        this.label = label;
                        this.m_class = clas;

                        _headerClass = new ClassViewModel(clas,label,doc,this,true);
                       // _headerClass.setMultiButtonInfo(MultibuttonInfo.MultiType.header, this);
                       //new ObservableCollection<ClassViewModel>(list);
                        _addedClasses = new ObservableCollection<ClassViewModel>();

                    }
                }
               
                else 
                {
                    //es un integervalue
                    original = new CIntegerValue(clas);
                    _header = new IntegerViewModel(label, original,_doc,tag);
                    this.label = label;
                    _added = new ObservableCollection<IntegerViewModel>();
                }
                
            }
           
           

        }
        
        public IntegerViewModel Header { get { return _header; } set { } }

        public ClassViewModel HeaderClass { get { return _headerClass; } set { _headerClass = value; NotifyOfPropertyChange(() => HeaderClass); } }
        public ObservableCollection<ClassViewModel> AdedClasses { get { return _addedClasses; } set { _addedClasses = value; NotifyOfPropertyChange(()=> AdedClasses); } }

        public ObservableCollection<IntegerViewModel> Aded 
        {
            get { return _added; }
            set
            {
                _added = value;
                NotifyOfPropertyChange(() => Aded ); 
            }
        }
        public String RowCount { get { if (AdedClasses != null)return "" + AdedClasses.Count; else return "0"; } set { } }
        public void Delete(IntegerViewModel delete)
        {
            _added.Remove(delete);
            NotifyOfPropertyChange(() => Aded);

        }
        public void DeleteClass(ClassViewModel delete)
        {
            _addedClasses.Remove(delete);
            delete.removeFromNewClass();
            /*int index = delete.index;
            for (int i = index; i < _adedClasses.Count;i++)
            {
                _adedClasses[i].index--;
            }*/
            if (_addedClasses.Count == 0)
                NotifyOfPropertyChange(() => DeleteVisible);
            NotifyOfPropertyChange(() => AdedClasses);

        }
        public void AddNew()
        {
           
            {
                var x = Header;
                if (x.ComboBox != null)
                {
                    string valor = x.ComboBox[0].SelectedComboValue;
                    original.defaultValue = valor;
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
            if(t.TextBoxFile!=null)
            {
                t.TextBoxFile[0].copyDefault = Header.TextBoxFile[0].copyDefault;
            }
            _added.Add(t);
        }
        public void DeleteLast()
        {
            _addedClasses[AdedClasses.Count - 1].removeFromNewClass();
            _addedClasses.RemoveAt(_addedClasses.Count - 1);
            if (_addedClasses.Count == 0)
                NotifyOfPropertyChange(() => DeleteVisible);
        }
        public void Add()
        {
            ClassViewModel cvm = new ClassViewModel(this.m_class,label,_doc,this,false);
            //cvm.setMultiButtonInfo(MultibuttonInfo.MultiType.added, this);
            //int index = _adedClasses.Count;
            //_adedClasses.Add(new ClassViewWithIndex(cvm,index));
            _addedClasses.Add(cvm);
            if (_addedClasses.Count == 1)
                NotifyOfPropertyChange(() => DeleteVisible);
           
        }
        public void removeViews()
        {
            if (this._headerClass != null)
                _headerClass.removeViews();
            if(this._addedClasses!=null)
            {
                foreach(ClassViewModel clas in _addedClasses)
                {
                    clas.removeViews();
                }
            }
        }
    
        public String DeleteVisible { get { if (_addedClasses != null && _addedClasses.Count > 0) return "Visible"; else return "Hidden"; } set { } }

        public override bool validate()
        {
           
            if(HeaderClass!=null)
            {
               
                if (!HeaderClass.validate(false))
                    return false;
                foreach(ClassViewModel cvm in AdedClasses)
                {
                    if (!cvm.validate(false))
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
       

        public override List<XmlNode> getXmlNode()
        {
            
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
                        List<XmlNode> nodos2 = cvm.getXmlNodes();
                        foreach (XmlNode child2 in nodos2)
                            lastChild2.AppendChild(child2);
                        nodes.Add(nodo2);
                    }
                }
            }
            else
            {
                nodes.AddRange(Header.getXmlNode());
                
                if(Aded!=null)
                {
                    foreach(IntegerViewModel ivm in Aded)
                    {
                        nodes.AddRange(ivm.getXmlNode());
                      
                    }
                }
            }
            return nodes;
        }

        public void setAsNull()
        {
            
            if (HeaderClass != null)
                HeaderClass.setAsNull();
            else
                Header.Value = "";
            if(_addedClasses!=null)
            {
                foreach (ClassViewModel cvm in _addedClasses)
                    cvm.setAsNull();
            }
            if (_added != null)
            {
                foreach (IntegerViewModel ivm in _added)
                    ivm.Value = "";
            }
        }

        public void removeViewsFromNew()
        {
            if(HeaderClass!=null)
            {
                HeaderClass.removeFromNewClass();
                if(AdedClasses!=null)
                {
                    foreach (ClassViewModel cvm in AdedClasses)
                        cvm.removeFromNewClass();
                }
            }
        }
    }
   
}
