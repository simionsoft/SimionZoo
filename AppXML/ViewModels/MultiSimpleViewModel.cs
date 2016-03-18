using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.ViewModels
{
    public class MultiSimpleViewModel:MultiValuedViewModel
    {
        public MultiSimpleViewModel(string label,string clas,string comment, bool isOptional,XmlDocument doc,string tag):base(label,clas,comment,isOptional,doc,tag)
        {

        }
    }
}
