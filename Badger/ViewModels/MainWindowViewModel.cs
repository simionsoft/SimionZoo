/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using Caliburn.Micro;

using System;
using System.IO;
using System.Text;
using Herd.Files;

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

        public const string logFilename = Folders.badgerLogFile;

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
