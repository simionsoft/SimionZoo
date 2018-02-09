using Caliburn.Micro;


namespace Badger.ViewModels
{
    public class SelectableTreeItem : PropertyChangedBase
    {
        private bool m_bIsSelected = true;

        public bool IsSelected
        {
            get { return m_bIsSelected; }
            set
            {
                m_bIsSelected = value; NotifyOfPropertyChange(() => IsSelected);
                TraverseAction(false, (element) => { element.IsSelected = value; });
            }
        }

        private bool m_bIsCheckVisible;

        public bool IsCheckVisible
        {
            get { return m_bIsCheckVisible; }
            set
            {
                m_bIsCheckVisible = value; NotifyOfPropertyChange(() => IsCheckVisible);
                TraverseAction(false, (element) => { element.IsCheckVisible = value; });
            }
        }

        public void LocalTraverseAction(System.Action<SelectableTreeItem> action) { action(this); }

        public virtual void TraverseAction(bool doActionLocally, System.Action<SelectableTreeItem> action) { }
    }
}
