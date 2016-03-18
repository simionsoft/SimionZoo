using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.ViewModels
{
    public class MultiXmlNodeRefViewModel:ValidableAndNodeViewModel
    {
        private XMLNodeRefViewModel _header;
        private ObservableCollection<XMLNodeRefViewModel> _addedXml;

        public XMLNodeRefViewModel Header
        {
            get { return _header; }
            set
            {
                _header = value;
                NotifyOfPropertyChange(() => Header);
            }
        }
        public ObservableCollection<XMLNodeRefViewModel> AddedXml
        {
            get { return _addedXml; }
            set
            {
                _addedXml = value;
                NotifyOfPropertyChange(() => AddedXml);
            }
        }

        public string Label{get;set;}

        private string file;
        private string action;
        private XmlDocument doc;
        private string tag;
        

        public MultiXmlNodeRefViewModel(string label,string file,string action,XmlDocument doc, string tag)
        {
            Label = label;
            this.file = file;
            this.action = action;
            this.doc = doc;
            this.tag = tag;
            _header = new XMLNodeRefViewModel(label, file, action, doc, tag);
        }
        public void Add()
        {
            if (_addedXml == null)
                _addedXml = new ObservableCollection<XMLNodeRefViewModel>();
            XMLNodeRefViewModel tmp = new XMLNodeRefViewModel(Label, file, action, doc, tag);
            if (_header.SelectedOption != null)
                tmp.SelectedOption = _header.SelectedOption;
            _addedXml.Add(tmp);
            NotifyOfPropertyChange(() => AddedXml);
        }
        public void Remove(object sender)
        {
            if(sender is XMLNodeRefViewModel)
            {
                _addedXml.Remove(sender as XMLNodeRefViewModel);
                Models.CApp.removeView(sender as XMLNodeRefViewModel);
                NotifyOfPropertyChange(() => AddedXml);
            }
        }

        public override bool validate()
        {
            if (!_header.validate())
                return false;
            if(_addedXml !=null)
            {
                foreach(XMLNodeRefViewModel item in _addedXml)
                {
                    if (!item.validate())
                        return false;
                }
            }
            return true;
        }
        public override List<XmlNode> getXmlNode()
        {
            List<XmlNode> list = new List<XmlNode>();
            list.AddRange(_header.getXmlNode());
            if(_addedXml!=null)
            {
                foreach (XMLNodeRefViewModel item in _addedXml)
                    list.AddRange(item.getXmlNode());
            }
            return list;
        }
        public void removeViews()
        {
            Models.CApp.removeView(this._header);
            if (_addedXml != null)
                Models.CApp.removeViews(_addedXml.ToList());
        }
    }
}
