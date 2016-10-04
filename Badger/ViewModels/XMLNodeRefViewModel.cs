using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.ViewModels
{
    public class XMLNodeRefViewModel:ValidableAndNodeViewModel
    {
        private string _label;
        private ObservableCollection<string> _options;
        private string _XMLFile;
        private string _action;
        private string _selectedOption;
        private XmlDocument _doc;
        private string tag;

        public string Tag { get { return tag; } set { } }

        public string SelectedOption { get { return _selectedOption; } 
            set {
                _selectedOption = value; 
                NotifyOfPropertyChange(()=>SelectedOption); 
            } }

        public XMLNodeRefViewModel(string label, string file, string action, XmlDocument doc, string tag,ClassViewModel owner)
        {
            this._label = label;
            this._XMLFile = file;
            this._action = action;
            if (tag == null || tag == "")
                this.tag = label;
            else
                this.tag = tag;
            _doc = doc;
            List<string> names =Badger.Data.Utility.getComboFromXML(file, action);
            _options = new ObservableCollection<string>(names);
            Badger.Models.CApp.addView(this);
            
       }
        public void update()
        {
            List<string> names = Badger.Data.Utility.getComboFromXML(_XMLFile, _action);
            _options = new ObservableCollection<string>(names);
            NotifyOfPropertyChange(() => Options);
        }
        public ObservableCollection<string> Options { get { return _options; } set { } }
        public string Label { get { return _label; } set { } }
        public override bool validate()
        {
            return _selectedOption != null;
        }

        public override List<XmlNode> getXmlNode()
        {
            XmlNode nodo = Badger.Data.Utility.resolveTag(tag, _doc);
            XmlNode lastChild = Badger.Data.Utility.getLastChild(nodo);
            lastChild.InnerText = SelectedOption;
            List<XmlNode> result = new List<XmlNode>();
            result.Add(nodo);
            return result;
        }
    }
}
