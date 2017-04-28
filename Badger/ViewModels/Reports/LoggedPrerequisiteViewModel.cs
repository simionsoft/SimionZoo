using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Badger.Simion;

namespace Badger.ViewModels.Reports
{
    public class LoggedPrerequisiteViewModel
    {
        private string m_value;

        public string Value
        {
            get { return m_value; }
            set { m_value = value; }
        }

        public LoggedPrerequisiteViewModel(XmlNode configNode, Window parent)
        {
            XmlAttributeCollection attrs = configNode.Attributes;
            if (attrs != null)
            {
                if (attrs.GetNamedItem(XMLConfig.valueAttribute) != null)
                    Value = attrs[XMLConfig.valueAttribute].Value;
            }
        }
    }
}
