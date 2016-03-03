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
        private XmlNode _node;
        private string _clas;
        private ClassViewModel _Class;
        private string _XML;
        public ChoiceViewModel(XmlNode nodo)
        {
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
            _Class = new ClassViewModel(_comboValues[0].clas);
            if(XML)
            {
                string key = _XML;
                string value = _comboValues[0].XML;
                CNode.XML.Add(key, value);
            }
                
        }
        /*public ChoiceViewModel()
        {
            CNode root = AppXML.Data.Utility.getRootNode("../config/RLSimion.xml");
            XmlNode nodo = CNode.definitions["Single-Output-VFA-Policy-Learner"].ChildNodes[0];
            _label = nodo.Attributes["Name"].Value;
            _node = nodo;
            _comboValues = new ObservableCollection<ChoiceElement>();
            foreach (XmlNode child in nodo.ChildNodes)
            {
                ChoiceElement ce = new ChoiceElement(child.Attributes["Name"].Value, child.Attributes["Class"].Value);
                _comboValues.Add(ce);
            }


            
        }*/
        public void ComboChanged(object sender)
        {
            ChoiceElement ce = sender as ChoiceElement;
            _Class.removeViews();
            _Class = new ClassViewModel(ce.clas);
            if(ce.XML!=null)
            {
                CNode.XML[_XML] = (sender as ChoiceElement).XML;
                CApp.updateViews();
            }
            NotifyOfPropertyChange(() => Class);
        }
        public string Label { get { return _label; } set { } }
        public ObservableCollection<ChoiceElement> Combo { get { return _comboValues; } set { } }
        public string Clas { get { return _clas; } set { } }
        public ClassViewModel Class { get { return _Class; } set { } }
        public void removeViews()
        {
            _Class.removeViews();
        }

        public bool validate()
        {
            return _Class.validate();
        }
    }
}
