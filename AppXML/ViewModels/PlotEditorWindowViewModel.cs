using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class PlotEditorWindowViewModel : Screen
    {
        private List<ExperimentViewModel> m_experimentList;
        public List<ExperimentViewModel> experimentList { get { return m_experimentList; } set { } }

        public PlotEditorWindowViewModel(List<ExperimentViewModel> experimentList)
        {
            m_experimentList = experimentList;
        }
    }
    
}
