using Caliburn.Micro;

using System;
using System.IO;
using System.Text;

namespace Badger.ViewModels
{
    /// <summary>
    /// 
    /// </summary>
    public class MainWindowViewModel : PropertyChangedBase
    {
        static public MainWindowViewModel Instance { get; set; }

        public MonitorWindowViewModel MonitorWindowVM { get; }

        public EditorWindowViewModel EditorWindowVM { get; }

        public ReportsWindowViewModel ReportWindowVM { get; }

        private int m_selectedScreenIndex = 0;
        public int SelectedScreenIndex
        {
            get { return m_selectedScreenIndex; }
            set { m_selectedScreenIndex = value; NotifyOfPropertyChange(() => SelectedScreenIndex); }
        }

        /// <summary>
        ///     Class constructor.
        /// </summary>
        public MainWindowViewModel()
        {
            //Save the instance
            Instance = this;

            EditorWindowVM = new EditorWindowViewModel();
            MonitorWindowVM = new MonitorWindowViewModel();
            ReportWindowVM = new ReportsWindowViewModel();

            //set culture as invariant to write numbers as in english
            System.Threading.Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

        }


        /// <summary>
        /// Show the report viewer tab
        /// </summary>
        public void ShowReportWindow()
        {
            SelectedScreenIndex = 2;
        }

        /// <summary>
        /// Shows the experiment monitor tab
        /// </summary>
        public void ShowExperimentMonitor()
        {
            SelectedScreenIndex= 1;
        }

        /// <summary>
        ///     Shows the editor tab
        /// </summary>
        public void ShowEditorWindow()
        {
            SelectedScreenIndex= 0;
        }

        //logging functions used from all the screens in the tab control
        private object m_logFileLock = new object();

        public const string logFilename = Files.badgerLogFile;

        private bool m_bFirstLog = true;

        public void LogToFile(string logMessage)
        {
            lock (m_logFileLock)
            {
                string text = DateTime.Now.ToShortDateString() + " "
                    + DateTime.Now.ToShortTimeString() + ": " + logMessage + "\n";
                FileStream file;

                if (m_bFirstLog)
                {
                    file = File.Create(logFilename);
                    m_bFirstLog = false;
                }
                else
                    file = File.Open(logFilename, FileMode.Append);

                file.Write(Encoding.ASCII.GetBytes(text), 0, Encoding.ASCII.GetByteCount(text));
                file.Close();

                Console.WriteLine(text);
            }
        }
    }
}
