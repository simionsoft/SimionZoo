using System.Xml;
using System.Collections.Generic;

using Caliburn.Micro;


namespace Badger.ViewModels
{

    public class LoggedForkValueViewModel: SelectableTreeItem
    {
        private string m_value = "";
        public string Value { get { return m_value; } set { m_value = value; } }

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
        public List<LoggedExperimentalUnitViewModel> expUnits { get; }
            = new List<LoggedExperimentalUnitViewModel>();

        public LoggedForkValueViewModel(XmlNode configNode, LoggedForkViewModel _parent)
        {
            //FIXXXXXXXXXXXXXXXXXXXXXX
            //
            //
            //
            //


            //hide the area used to display children forks?
            HasChildrenForks = Forks.Count != 0;
        }

        public override void TraverseAction(bool doActionLocally,System.Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in Forks)
                fork.TraverseAction(doActionLocally, action);
            foreach (LoggedExperimentalUnitViewModel expUnit in expUnits)
                expUnit.LocalTraverseAction(action);
        }
    }
}
