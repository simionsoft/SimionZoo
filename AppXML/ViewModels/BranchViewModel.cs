using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Xml;
namespace AppXML.ViewModels
{
    public class BranchViewModel:PropertyChangedBase
    {
        private bool _isOptional;
        private string _name;
        private ClassViewModel _class;
        private bool _isNull=false;
        private string _comment;
        private XmlDocument _doc;


        public string Comment { get { return _comment; } set { } }
        
        public bool IsNull { get { return _isNull; } set { _isNull = value; NotifyOfPropertyChange(() => IsNull); NotifyOfPropertyChange(() => IsEnabled); } }
        public bool IsEnabled { get { return !_isNull; } set { } }

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
        public BranchViewModel(string name,string clas,string comment,bool isOptional, XmlDocument doc)
        {
            _name = name;
            _comment = comment;
            _class = new ClassViewModel(clas,doc);
            _isOptional = isOptional;
            _doc = doc;
        }
        public string Name{get{return _name;}set{}}
        public ClassViewModel Class { get { return _class; } set { } }
        public void removeViews()
        {
            _class.removeViews();
        }

        public bool validate()
        {
            if (_isOptional && IsNull)
                return true;
            return _class.validate();
        }

        internal XmlNode getXmlNode()
        {
            return Class.getXmlNode();
        }
    }
}
