using System.Collections.Generic;
using Caliburn.Micro;


namespace Badger.ViewModels
{
    public class LoggedVariableViewModel : PropertyChangedBase
    {
        private bool m_bIsSelected;

        public bool bIsSelected
        {
            get { return m_bIsSelected; }
            set
            {
                m_bIsSelected = value;
                NotifyOfPropertyChange(() => bIsSelected);

                if (m_parent != null)
                    m_parent.ValidateQuery();
            }
        }

        private bool m_bCheckIsVisible = true;

        public bool bCheckIsVisible
        {
            get { return m_bCheckIsVisible; }
            set
            {
                m_bCheckIsVisible = value;
                NotifyOfPropertyChange(() => bCheckIsVisible);
            }
        }

        private string m_name;

        public string name
        {
            get { return m_name; }
            set
            {
                m_name = value;
                NotifyOfPropertyChange(() => name);
            }
        }

        public enum PlotType { LastEvaluation, AllEvaluations, Both };

        private MultiStateButtonViewModel m_stateButton =
            new MultiStateButtonViewModel(new List<string>() { PlotType.LastEvaluation.ToString(),
                PlotType.AllEvaluations.ToString(), PlotType.Both.ToString()});

        public MultiStateButtonViewModel stateButton
        {
            get { return m_stateButton; }
        }

        public LoggedVariableViewModel(string name)
        {
            m_name = name;
        }

        ReportsWindowViewModel m_parent;

        public void setParent(ReportsWindowViewModel parent) { m_parent = parent; }
    }
}