
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ReportViewModel : PropertyChangedBase
    {
        public virtual void updateView() { }
        public virtual void export(string outputFolder) { }
    }
}
