using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.ViewModels
{
    public abstract  class ValidableAndNodeViewModel:Caliburn.Micro.PropertyChangedBase,IValidable,IGetXml
    {
        public abstract bool validate();
        public abstract List<XmlNode> getXmlNode();
    }
}
