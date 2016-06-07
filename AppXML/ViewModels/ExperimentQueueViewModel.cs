using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Collections.ObjectModel;
using AppXML.Models;
using Caliburn.Micro;

namespace AppXML.ViewModels
{

    public class ExperimentQueueViewModel : PropertyChangedBase
    {
        
        private WindowViewModel m_parent;

        public bool isEmpty() { return m_experimentQueue.Count == 0; }

        private bool m_bModified = false;
        public bool bModified { get { return m_bModified; } set { m_bModified = value; } }

        private BindableCollection<Experiment> m_experimentQueue= new BindableCollection<Experiment>();
        public BindableCollection<Experiment> experimentQueue { get { return m_experimentQueue; } set { m_experimentQueue = value; } }

        private int m_selectedIndex= -1;
        public int selectedIndex
        {
            get { return m_selectedIndex; }
            set { m_selectedIndex = value;
            if (m_selectedIndex >= 0)
            {
                m_parent.loadExperimentInEditor(m_experimentQueue[m_selectedIndex].experimentXML);
                m_parent.bIsExperimentQueueNotEmpty = true;
            }
            NotifyOfPropertyChange(()=>selectedIndex);}
        }

        public void markModified(bool modified)
        {
            m_bModified = modified;
            NotifyOfPropertyChange(() => bModified);
        }
        public void clear()
        {
            m_experimentQueue.Clear();
            NotifyOfPropertyChange(()=>experimentQueue);
        }

        public ExperimentQueueViewModel()
        {
            m_selectedIndex = -1;

        }
        public void setParent(WindowViewModel parent)
        {
            m_parent= parent;
        }

        public void addExperiment(Experiment exp)
        {
            m_experimentQueue.Add(exp);
            markModified(true);

            NotifyOfPropertyChange(() => experimentQueue);
        }
        public void addExperiment(string name,XmlDocument expXML)
        {
            Experiment newExperiment = new Experiment(name, expXML);
            m_experimentQueue.Add(newExperiment);
            markModified(true);

            NotifyOfPropertyChange(() => experimentQueue);
        }

        public void removeSelectedExperiments()
        {
            if (m_selectedIndex>=0)
            {
                markModified(true);

                m_experimentQueue.RemoveAt(m_selectedIndex);
                NotifyOfPropertyChange(() => experimentQueue);
            }
        }
        public void modifySelectedExperiment(XmlDocument modifiedExperimentXML)
        {
            if(m_selectedIndex>=0)
            {
                markModified(true);

                m_experimentQueue[m_selectedIndex].experimentXML = modifiedExperimentXML;
            }
        }
    }
}
