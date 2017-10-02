using Caliburn.Micro;


namespace Badger.ViewModels
{
    public class SelectableTreeItem : PropertyChangedBase
    {
        private bool m_bIsSelected = true;

        public bool bIsSelected
        {
            get { return m_bIsSelected; }
            set
            {
                m_bIsSelected = value; NotifyOfPropertyChange(() => bIsSelected);
                TraverseAction(false, (element) => { element.bIsSelected = value; });
            }
        }

        private bool m_bIsCheckVisible;

        public bool IsCheckVisible
        {
            get { return m_bIsCheckVisible; }
            set
            {
                m_bIsCheckVisible = value;
                NotifyOfPropertyChange(() => IsCheckVisible);
                TraverseAction(false, (element) => { element.IsCheckVisible = value; });
            }
        }

        private bool m_bVisible = true;

        public bool bVisible
        {
            get { return m_bVisible; }
            set
            {
                m_bVisible = value;
                NotifyOfPropertyChange(() => bVisible);
            }
        }

        public void LocalTraverseAction(System.Action<SelectableTreeItem> action) { action(this); }

        public virtual void TraverseAction(bool doActionLocally, System.Action<SelectableTreeItem> action) { }
    }
}
