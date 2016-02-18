using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Models
{
    public class CBranch:CNode
    {
        public List<CIntegerValue> elements { get; set; }
        public string m_comment { get; set; }
        public string klass { get; set; }
        public CBranch(XmlNode node, string n, string c, string k) 
        {
            name = n;
            m_comment = c;
            klass = k;
            this.node = node;
        }
    }
}
