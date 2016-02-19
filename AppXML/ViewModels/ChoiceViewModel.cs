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
        public ChoiceViewModel(XmlNode nodo)
        {
            _label = nodo.Attributes["Name"].Value;
            _node = nodo;
            _comboValues = new ObservableCollection<ChoiceElement>();
            foreach(XmlNode child in nodo.ChildNodes)
            {
                ChoiceElement ce = new ChoiceElement(child.Attributes["Name"].Value, child.Attributes["Class"].Value);
                _comboValues.Add(ce);
            }
            _Class = new ClassViewModel(_comboValues[0].clas);
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
            _Class = new ClassViewModel((sender as ChoiceElement).clas);
            NotifyOfPropertyChange(() => Class);
        }
        public string Label { get { return _label; } set { } }
        public ObservableCollection<ChoiceElement> Combo { get { return _comboValues; } set { } }
        public string Clas { get { return _clas; } set { } }
        public ClassViewModel Class { get { return _Class; } set { } }
    }
}
