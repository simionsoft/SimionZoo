using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class SelectableTreeItem: PropertyChangedBase
    {
        private bool m_bIsSelected = true;
        public bool bIsSelected
        {
            get { return m_bIsSelected; }
            set { m_bIsSelected = value; OnSelectedChange(value); NotifyOfPropertyChange(() => bIsSelected); }
        }

        private bool m_bVisible = true;
        public bool bVisible
        {
            get { return m_bVisible; }
            set { m_bVisible = value;  NotifyOfPropertyChange(() => bVisible); }
        }

        public virtual void OnSelectedChange(bool bSelected)
        { }

        protected ReportsWindowViewModel m_parentWindow;
    }
}
