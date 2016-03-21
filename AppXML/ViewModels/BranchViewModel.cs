using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Xml;
namespace AppXML.ViewModels
{
    public class BranchViewModel:ValidableAndNodeViewModel
    {
        private bool _isOptional;
        private string _name;
        private ClassViewModel _class;
        private bool _isNull=false;
        private string _comment;
        private XmlDocument _doc;
        private string _tag;
        public string Tag { get { return _tag; } set { } }
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
        public BranchViewModel(string name,string clas,string comment,bool isOptional, XmlDocument doc, string tag)
        {
            _name = name;
            _comment = comment;
            _class = new ClassViewModel(clas,doc);
            _isOptional = isOptional;
            _doc = doc;
            if (tag == null || tag == "")
                _tag = name;
            else
                _tag = tag;

            
        }
        public string Name{get{return _name;}set{}}
        public ClassViewModel Class { get { return _class; } set { } }
        public void removeViews()
        {
            _class.removeViews();
        }

        public override bool validate()
        {
            if (_isOptional && IsNull)
                return true;
            return _class.validate();
        }

        public override  List<XmlNode> getXmlNode()
        {
            if(IsNull)
                return null;
            XmlNode result = AppXML.Data.Utility.resolveTag(_tag,_doc);
            XmlNode lastChild = AppXML.Data.Utility.getLastChild(result); 
            foreach (XmlNode child in Class.getXmlNodes())
            {
                if(child!=null)
                    lastChild.AppendChild(child);
            }
            //si al ultimo nodo no se le ha añadido ningun hijo no hay que añadir la rama ya que esta vacia.
            /*
            if (!lastChild.HasChildNodes)
                return null;*/
            List<XmlNode> list = new List<XmlNode>();
            list.Add(result);
            return list;
        }

        private XmlNode getLastChild(XmlNode result)
        {
            if (result.HasChildNodes)
                return getLastChild(result.ChildNodes[0]);
            else 
                return result;
        }

        private XmlNode resolveTag(string _tag)
        {
            string[] tags = _tag.Split('/');
            if(tags.Count()>0)
            {
                XmlNode result = _doc.CreateElement(tags[0]);
                if(tags.Count()>1)
                {
                    XmlNode father = result;
                    for(int i=1;i<tags.Count();i++)
                    {
                        XmlNode nodo = _doc.CreateElement(tags[i]);
                        father.AppendChild(nodo);
                        father = nodo;
                    }

                }
                return result;
            }

            return null;
           
            
        }
    }
}
