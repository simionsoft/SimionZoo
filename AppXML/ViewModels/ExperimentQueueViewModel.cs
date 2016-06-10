using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Collections.ObjectModel;
using AppXML.Models;
using Caliburn.Micro;
using System.Windows.Forms;
using System.Windows;
using System.IO;
using System.Dynamic;
using System.Threading;
using AppXML.Data;
using Herd;

namespace AppXML.ViewModels
{

    public class ExperimentQueueViewModel : PropertyChangedBase
    {
        
        private WindowViewModel m_parent;

        private string m_name;
        public string name { get { return m_name; } set { m_name = name; } }

        public bool isEmpty() { return m_experimentQueue.Count == 0; }

        private bool m_bModified = false;
        public bool bModified { get { return m_bModified; } set { m_bModified = value; } }

        private BindableCollection<ExperimentViewModel> m_experimentQueue
            = new BindableCollection<ExperimentViewModel>();
        public BindableCollection<ExperimentViewModel> experimentQueue { get { return m_experimentQueue; } set { m_experimentQueue = value; } }

        public void resetState()
        {
            foreach (ExperimentViewModel experiment in m_experimentQueue)
                experiment.resetState();
        }

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

        public void addExperiment(ExperimentViewModel exp)
        {
            m_experimentQueue.Add(exp);
            markModified(true);

            NotifyOfPropertyChange(() => experimentQueue);
        }
        public void addExperiment(string name,XmlDocument expXML)
        {
            ExperimentViewModel newExperiment = new ExperimentViewModel(name, expXML);
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


        public bool save()
        {
            string xmlFileName = null;

            if (experimentQueue.Count == 0) return false;

            //Save dialog -> returns the experiment queue file
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment batch | *.exp-batch";
            sfd.InitialDirectory = "../experiments";
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "../experiments");
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath);

            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                xmlFileName = sfd.FileName;
            }
            else return false;

            //clean output directory if it exists
            xmlFileName = xmlFileName.Split('.')[0];
            xmlFileName = Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), xmlFileName);
            if (Directory.Exists(xmlFileName))
            {
                try
                {
                    Directory.Delete(xmlFileName, true);
                }
                catch
                {
                    DialogViewModel dvm = new DialogViewModel(null, "It has not been possible to remove the directory: " + xmlFileName + ". Make sure that it's not been using for other app.", DialogViewModel.DialogType.Info);
                    dynamic settings = new ExpandoObject();
                    settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
                    settings.ShowInTaskbar = true;
                    settings.Title = "ERROR";

                    new WindowManager().ShowDialog(dvm, null, settings);
                    return false;
                }
            }
            
            XmlDocument experimentXMLDoc = new XmlDocument();
            XmlElement experimentBatchesNode = experimentXMLDoc.CreateElement("Experiments");
            experimentXMLDoc.AppendChild(experimentBatchesNode);

            List<string> names = new List<string>();

            //set the experiment name
            name = xmlFileName;

            foreach (ExperimentViewModel experiment in experimentQueue)
            {
                //avoid duplicated names
                while (names.Contains(experiment.name))
                    experiment.name += "c";
                names.Add(experiment.name);

                XmlDocument docume = experiment.experimentXML;
                string folderPath = xmlFileName + "/" + experiment.name;
                Directory.CreateDirectory(folderPath);
                string filePath = folderPath + "/" + experiment.name + ".experiment";
                docume.Save(filePath);
                //crear carpeta para archivo xml y carpetas para sus hijos
                //añadir el nodo al fichero xml
                XmlElement experimentNode = experimentXMLDoc.CreateElement(experiment.name);
                experimentNode.SetAttribute("Path", filePath);
                experimentBatchesNode.AppendChild(experimentNode);

                experiment.filePath = filePath;
            }

            experimentXMLDoc.Save(xmlFileName + ".exp-batch");
            markModified(false);
            return true;
        }

        void setStatus(string experimentName, string status)
        {
            foreach(ExperimentViewModel experimentVM in m_experimentQueue)
            {
                if (experimentVM.name==experimentName)
                {
                    experimentVM.addStatusInfoLine(status + "\n");
                    break;
                }
            }
        }
    }
}
