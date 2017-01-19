
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ReportViewModel : Screen
    {
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; NotifyOfPropertyChange(() => name); } }

        public virtual void updateView() { }
        public virtual void export(string outputFolder) { }

        private ReportsWindowViewModel m_parent = null;
        public ReportsWindowViewModel parent
        {
            get { return m_parent; }
            set { m_parent = value; }
        }
        public void close()
        {
            if (m_parent != null)
                m_parent.close(this);
        }
    }
}
