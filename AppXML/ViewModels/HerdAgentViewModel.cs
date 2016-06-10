using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Xml;
using System.Xml.Linq;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class HerdAgentViewModel : PropertyChangedBase
    {
        private IPEndPoint m_ipAddress;
        public IPEndPoint ipAddress { get { return m_ipAddress; } set { m_ipAddress = value; } }
        public string ipAddressString { get { return m_ipAddress.Address.ToString(); } set { } }

        private DateTime m_lastACK;
        public DateTime lastACK { get { return m_lastACK; } set { m_lastACK = value; } }

        //not sure i'll use this
        //private bool m_bWorkingForMe = false;
        //public bool bWorkingForMe { get { return m_bWorkingForMe; } set { m_bWorkingForMe = value; NotifyOfPropertyChange(() => bWorkingForMe); } }

        public string state { get { return getProperty(Herd.HerdAgent.m_stateXMLTag); } set { } }
        public string version { get { return getProperty(Herd.HerdAgent.m_versionXMLTag); } set { } }
        public int numProcessors { get { return Int32.Parse(getProperty(Herd.HerdAgent.m_numProcessorsXMLTag)); } set { } }

        private Dictionary<string, string> m_properties;
        public HerdAgentViewModel()
        {
            m_properties = new Dictionary<string, string>();
        }
        public void addProperty(string name, string value)
        {
            m_properties.Add(name, value);
        }
        public string getProperty(string name)
        {
            if (m_properties.ContainsKey(name))
                return m_properties[name];
            else return "n/a";
        }
        public void parse(XElement xmlDescription)
        {
            if (xmlDescription.Name.ToString() == "HerdAgent")
            {
                m_properties.Clear();
                foreach (XElement child in xmlDescription.Elements())
                {
                    addProperty(child.Name.ToString(), child.Value);
                }
            }
        }
        public override string ToString()
        {
            string res = "";
            foreach (var property in m_properties)
            {
                res += property.Key + "=\"" + property.Value + "\";";
            }
            return res;
        }
        private string m_status;
        public string status {
            get { return m_status; }
            set { m_status = value; NotifyOfPropertyChange(() => status); }
        }
    }
}
