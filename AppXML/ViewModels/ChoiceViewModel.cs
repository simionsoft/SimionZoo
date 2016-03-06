using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using AppXML.Models;
using System.Collections.ObjectModel;
using System.Xml;
using System.Windows.Controls;


namespace AppXML.ViewModels
{
    public class ChoiceViewModel:PropertyChangedBase
    {
        private string _label;
        private ObservableCollection<ChoiceElement> _comboValues;
        private ChoiceElement _selectedItem;
        private XmlNode _node;
        private string _clas;
        private ClassViewModel _Class;
        private string _XML;
        private XmlDocument _doc;
        public ChoiceViewModel(XmlNode nodo, XmlDocument doc)
        {
            _doc = doc;
            _label = nodo.Attributes["Name"].Value;
            _node = nodo;
            _comboValues = new ObservableCollection<ChoiceElement>();
            bool XML = false;
            if (nodo.Attributes["LoadXML"] != null)
            {
                XML = true;
                _XML = nodo.Attributes["LoadXML"].Value;
            }
            foreach(XmlNode child in nodo.ChildNodes)
            {
                if(child.Name=="CHOICE-ELEMENT")
                {
                    ChoiceElement ce = new ChoiceElement(child.Attributes["Name"].Value, child.Attributes["Class"].Value);
                    if (XML && child.Attributes["XML"] != null)
                        ce.XML = child.Attributes["XML"].Value;
                    _comboValues.Add(ce);
                }
                
            }
           
            if(XML)
            {
                string key = _XML;
                string value = _comboValues[0].XML;
                CNode.XML.Add(key, value);
            }
            SelectedItem = _comboValues[0];
                
        }
       
       
        public string Label { get { return _label; } set { } }
        public ObservableCollection<ChoiceElement> Combo { get { return _comboValues; } set { } }
        public string Clas { get { return _clas; } set { } }
        public ClassViewModel Class { get { return _Class; } set { _Class = value; } }
        public ChoiceElement SelectedItem { get { return _selectedItem; } 
            set
            {
                _selectedItem = value; 
                NotifyOfPropertyChange(() => SelectedItem);
                if (_Class != null)
                    _Class.removeViews();
                _Class = new ClassViewModel(SelectedItem.clas,_doc);
                if (SelectedItem.XML != null)
                {
                    CNode.XML[_XML] = SelectedItem.XML;
                    CApp.updateViews();
                }
                NotifyOfPropertyChange(() => Class);
            } 
        }
        public void removeViews()
        {
            _Class.removeViews();
        }

        public bool validate()
        {
            return _Class.validate();
        }

        internal XmlNode getXmlNode()
        {
            return _Class.getXmlNode();
        }
    }
}
