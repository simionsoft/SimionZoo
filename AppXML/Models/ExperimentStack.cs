using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;


namespace AppXML.Models
{
    public class Experiment:Caliburn.Micro.PropertyChangedBase
    {
        private string m_tags;
        public string tags { get { return m_tags; } set { m_tags = value; } }
        private string m_name;
        public string name { get { return m_name; }
            set { m_name = value; } }

        private XmlDocument m_experimentXML;
        public XmlDocument experimentXML{get{return m_experimentXML;} set{m_experimentXML=value;}}

        public Experiment(string name, XmlDocument experimentXML)
        {
            m_name = name;
            m_experimentXML = experimentXML;
        }
    }
    //public class ExperimentQueue : Caliburn.Micro.PropertyChangedBase
    //{
    //    private List<Experiment> m_experiments;
    //    public List<Experiment> experiments{get{return m_experiments;}set{m_experiments=value;}}
       

    //    public ExperimentQueue()
    //    {
    //        m_experiments= new List<Experiment>();
    //    }

    //    public void addExperiment(Experiment experiment)
    //    {
    //        m_experiments.Add(experiment);
    //        NotifyOfPropertyChange(() => experiments);
    //    }
    //    public void clean()
    //    {
    //        m_experiments.Clear();
    //    }
    //    public void removeElement(Experiment experiment)
    //    {
    //        //first we must know if child has childNodes or not
    //        if (m_experiments.Contains(experiment))
    //        {
    //            m_experiments.Remove(experiment);
    //        }
    //    }

    //}
}