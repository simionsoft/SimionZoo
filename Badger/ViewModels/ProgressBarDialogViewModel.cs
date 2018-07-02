using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ProgressBarDialogViewModel : Screen
    {
        private string m_title;
        private string m_message;

        public string Title { get { return m_title; } set { } }
        public string Message { get { return m_message; } set { } }

        public ProgressBarDialogViewModel(string title,string message)
        {
            m_title = title;
            m_message = message;
        }

        private bool m_bCancelled = false;
        public bool Cancelled { get { return m_bCancelled; } }

        public void Cancel()
        {
            m_bCancelled = true;
            TryClose();
        }

        private double m_progress = 0.0;
        public double Progress { get { return m_progress; } set { m_progress = value; NotifyOfPropertyChange(() => Progress); } }

        public void UpdateProgressBar(double value)
        {
            Progress = value;
        }
    }
    
}
