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
        public MultiSimpleViewModel(string label,string clas,string def,string comment, bool isOptional,XmlDocument doc,string tag):base(label,clas,comment,isOptional,doc,tag)
        {
            if (base.Header.ComboBox != null)
                base.Header.ComboBox[0].SelectedComboValue = def;
            else if (base.Header.TextBox != null)
                base.Header.TextBox[0].Default = def;
            else if (base.Header.TextBoxFile!=null)
                base.Header.TextBoxFile[0].Default=def;
        }
    }
}
