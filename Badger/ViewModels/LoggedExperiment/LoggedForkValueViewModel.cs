using Caliburn.Micro;
using System.Xml;

namespace Badger.ViewModels
{

    public class LoggedForkValueViewModel: PropertyChangedBase
    {
        private string m_value = "";
        public string value { get { return m_value; } set { m_value = value; } }
        public LoggedForkValueViewModel(XmlNode configNode)
        {
            value = configNode.InnerText;
        }
    }
}
