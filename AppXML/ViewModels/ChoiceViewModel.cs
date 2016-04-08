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
    public class ChoiceViewModel:ValidableAndNodeViewModel
    {
        private string _label;
        private ObservableCollection<ChoiceElement> _comboValues;
        private ChoiceElement _selectedItem;
        private XmlNode _node;
        private ClassViewModel _Class;
        private string _XML;
        private XmlDocument _doc;
        private string _tag;

        public string Tag { get { return _tag; } }
        public void setSelectedItem(string name)
        {
            foreach(ChoiceElement item in _comboValues)
            {
                if(item.name==name)
                {
                    SelectedItem = item;
                    return;
                }
            }
        }

        public ChoiceViewModel(XmlNode nodo, XmlDocument doc, string tag)
        {
            _doc = doc;
            _label = nodo.Attributes["Name"].Value;
            _node = nodo;
            _comboValues = new ObservableCollection<ChoiceElement>();
            if (tag == null || tag=="")
                _tag = _label;
            else
                _tag = tag;
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
                    if (child.Attributes["XMLTag"] != null)
                        ce.tag = child.Attributes["XMLTag"].Value;
                    else
                        ce.tag = ce.name;
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
        public ClassViewModel Class { get { return _Class; } set { _Class = value; } }
        public ChoiceElement SelectedItem { get { return _selectedItem; } 
            set
            {
                _selectedItem = value; 
                NotifyOfPropertyChange(() => SelectedItem);
                if (_Class != null)
                    _Class.removeViews();
                _Class = new ClassViewModel(SelectedItem.clas,null,_doc);
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

        public override bool validate()
        {
            return _Class.validate(false);
        }

        public override List<XmlNode> getXmlNode()
        {
            XmlNode result = AppXML.Data.Utility.resolveTag(_tag, _doc);
            XmlNode lastChild = AppXML.Data.Utility.getLastChild(result);
            //se asume que el tag para el interior del choice class es el nombre
            XmlNode itemResult = AppXML.Data.Utility.resolveTag(SelectedItem.tag, _doc);
            XmlNode itemLastChild = AppXML.Data.Utility.getLastChild(itemResult);
            foreach (XmlNode child in _Class.getXmlNodes())
                itemLastChild.AppendChild(child);
            lastChild.AppendChild(itemResult);
            List<XmlNode> list = new List<XmlNode>();
            list.Add(result);
            return list;
        }

        public void setAsNull()
        {
            Class.setAsNull();
        }
    }
}
