using System.Xml;
using System.Collections.Generic;

using Caliburn.Micro;

using Herd.Files;

namespace Badger.ViewModels
{

    public class LoggedForkValueViewModel: SelectableTreeItem
    {
        ForkValue m_model;

        public string Value { get { return m_model.Value; } set { m_model.Value = value; } }

        private BindableCollection<LoggedForkViewModel> m_forks = new BindableCollection<LoggedForkViewModel>();
        public BindableCollection<LoggedForkViewModel> Forks
        {
            get { return m_forks; }
            set { m_forks = value; NotifyOfPropertyChange(() => Forks); }
        }

        //this is used to hide the space given to display children forks in case there is none
        private bool m_bHasForks = false;
        public bool HasChildrenForks
        {
            get { return m_bHasForks; }
            set { m_bHasForks = value; NotifyOfPropertyChange(() => HasChildrenForks); }
        }

        public LoggedForkViewModel Parent { get; set; }
        public List<LoggedExperimentalUnitViewModel> ExpUnits { get; }
            = new List<LoggedExperimentalUnitViewModel>();

        public LoggedForkValueViewModel(ForkValue model, LoggedForkViewModel parent)
        {
            m_model = model;
            Parent = parent;

            //hide the area used to display children forks?
            HasChildrenForks = Forks.Count != 0;
        }

        public override void TraverseAction(bool doActionLocally,System.Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in Forks)
                fork.TraverseAction(doActionLocally, action);
            foreach (LoggedExperimentalUnitViewModel expUnit in ExpUnits)
                expUnit.LocalTraverseAction(action);
        }
    }
}
