using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Models
{
    public class CApp:CNode
    {
        public CApp(XmlNode node, string n) { name = n; }
    }
}
