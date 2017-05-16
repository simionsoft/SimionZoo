using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Badger.Data;
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
        private string m_rename = null;
        public string Rename { get { return m_rename; }} //read-only

        public LoggedPrerequisiteViewModel(XmlNode configNode)
        {
            XmlAttributeCollection attrs = configNode.Attributes;
            if (attrs != null)
            {
                if (attrs.GetNamedItem(XMLConfig.valueAttribute) != null)
                    Value = attrs[XMLConfig.valueAttribute].Value;
                if (attrs.GetNamedItem(XMLConfig.renameAttr) != null)
                    m_rename = attrs[XMLConfig.renameAttr].Value;
            }
        }
    }
}
