using System.Xml;
using System.Collections.Generic;
using System;

using Herd.Files;

namespace Badger.ViewModels
{
    public class LoggedForkViewModel : SelectableTreeItem
    {
        Fork m_model;

        public string Name
        {
            get { return m_model.Name; }
        }

        private List<LoggedForkValueViewModel> m_values = new List<LoggedForkValueViewModel>();

        public List<LoggedForkValueViewModel> Values
        {
            get { return m_values; }
        }

        private bool m_bGroupByThis;

        public bool IsGroupedByThisFork
        {
            get { return m_bGroupByThis; }
            set
            {
                m_bGroupByThis = value;
                NotifyOfPropertyChange(() => IsGroupedByThisFork);
            }
        }

        private List<LoggedForkViewModel> m_forks = new List<LoggedForkViewModel>();

        public List<LoggedForkViewModel> Forks
        {
            get { return m_forks; }
            set { m_forks = value; }
        }

        //this is used to hide the space given to display children forks in case there is none
        private bool m_bHasChildrenForks;
        public bool HasChildrenForks
        {
            get { return m_bHasChildrenForks; }
            set { m_bHasChildrenForks = value; NotifyOfPropertyChange(() => HasChildrenForks); }
        }
        private bool m_bHasChildrenValues;
        public bool HasChildrenValues
        {
            get { return m_bHasChildrenValues; }
            set { m_bHasChildrenValues = value; NotifyOfPropertyChange(() => HasChildrenValues); }
        }

        public LoggedForkViewModel(Fork model)
        {
            m_model = model;

            //Build View models from the models
            foreach (Fork fork in model.Forks)
            {
                LoggedForkViewModel forkVM = new LoggedForkViewModel(fork);
                Forks.Add(forkVM);
            }
            HasChildrenForks = Forks.Count != 0;
            foreach (ForkValue forkValue in model.Values)
            {
                LoggedForkValueViewModel forkValueVM = new LoggedForkValueViewModel(forkValue, this);
                Values.Add(forkValueVM);
            }
            HasChildrenValues = Values.Count != 0;
        }

        public override void TraverseAction(bool doActionLocally, Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkValueViewModel value in m_values) value.TraverseAction(true, action);
            foreach (LoggedForkViewModel fork in m_forks) fork.TraverseAction(true, action);
        }

        public void ToggleIsExpanded()
        {
            TraverseAction(false, (SelectableTreeItem item) => item.IsVisible = !item.IsVisible);
        }

        /// <summary>
        ///     Method is called from the context menu informs the parent window that results should be
        ///     grouped by this fork.
        /// </summary>
        public void GroupByThisFork() { IsGroupedByThisFork = true; }
    }
}
