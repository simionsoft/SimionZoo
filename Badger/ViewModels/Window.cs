using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class Window : Conductor<Screen>.Collection.OneActive
    {
        private bool m_bCanGenerateReports = false;
        public bool bCanGenerateReports
        {
            get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => bCanGenerateReports); }
        }

        private BindableCollection<string> m_inGroupSelectionVariables = new BindableCollection<string>();
        public BindableCollection<string> inGroupSelectionVariables
        {
            get { return m_inGroupSelectionVariables; }
            set { m_inGroupSelectionVariables = value; NotifyOfPropertyChange(() => inGroupSelectionVariables); }
        }

        protected string m_selectedInGroupSelectionVariable = "";
        public string selectedInGroupSelectionVariable
        {
            get { return m_selectedInGroupSelectionVariable; }
            set { m_selectedInGroupSelectionVariable = value; validateQuery(); NotifyOfPropertyChange(() => selectedInGroupSelectionVariable); }
        }

        protected bool m_bGroupsEnabled = false; //no groups by default
        public bool bGroupsEnabled
        {
            get { return m_bGroupsEnabled; }
            set { m_bGroupsEnabled = value; NotifyOfPropertyChange(() => bGroupsEnabled); }
        }

        private BindableCollection<string> m_orderByVariables = new BindableCollection<string>();
        public BindableCollection<string> orderByVariables
        {
            get { return m_orderByVariables; }
            set { m_orderByVariables = value; NotifyOfPropertyChange(() => orderByVariables); }
        }

        //Group By
        protected BindableCollection<string> m_groupByForks = new BindableCollection<string>();
        public string groupBy
        {
            get
            {
                string s = "";
                for (int i = 0; i < m_groupByForks.Count - 1; i++) s += m_groupByForks[i] + ",";
                if (m_groupByForks.Count > 0) s += m_groupByForks[m_groupByForks.Count - 1];
                return s;
            }
        }

        public void addGroupBy(string forkName)
        {
            m_groupByForks.Add(forkName);
            validateQuery();
            NotifyOfPropertyChange(() => groupBy);
            bGroupsEnabled = true;
        }

        //Variables
        private BindableCollection<LoggedVariableViewModel> m_variables
            = new BindableCollection<LoggedVariableViewModel>();
        public BindableCollection<LoggedVariableViewModel> variables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => variables); }
        }
        //to be called by children experimental units after reading their log file descriptor
        public void addVariable(string variable)
        {
            bool bVarExists = false;
            foreach (LoggedVariableViewModel var in variables) if (var.name == variable) bVarExists = true;
            if (!bVarExists)
            {
                variables.Add(new LoggedVariableViewModel(variable, this));
                inGroupSelectionVariables.Add(variable);
                orderByVariables.Add(variable);
            }
        }


        public void validateQuery()
        {
            //validate the current query
            int numSelectedVars = 0;
            foreach (LoggedVariableViewModel variable in variables) if (variable.bIsSelected) ++numSelectedVars;
            if (numSelectedVars == 0 || selectedInGroupSelectionVariable == "")
                bCanGenerateReports = false;
            else bCanGenerateReports = true;

            //update the "enabled" property of the variable used to select a group
            bGroupsEnabled = m_groupByForks.Count > 0;
        }
    }
}
