
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ReportViewModel : PropertyChangedBase
    {
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; NotifyOfPropertyChange(() => name); } }

        public virtual void updateView() { }
        public virtual void export(string outputFolder) { }
    }
}
