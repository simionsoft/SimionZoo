using System;
using System.ServiceProcess;
using System.Threading;
using System.Net.Sockets;
using System.IO;
using Herd;

//Installing/uninstalling the service: https://msdn.microsoft.com/en-us/library/zt39148a%28v=vs.110%29.aspx?f=255&MSPPError=-2147217396#BK_Install

namespace Herd
{



    public partial class HerdService : ServiceBase
    {
        private HerdAgent m_herdAgent;
        private string m_dirPath;

        private object m_logFileLock = new object();

        public HerdService()
        {

            InitializeComponent();
        }
        public string getLogFilename()
        {
            return m_dirPath + "//log.txt";
        }

        public void cleanLog()
        {
            string logFile = getLogFilename();
            lock (m_logFileLock)
            {
                FileStream file = File.Create(logFile);
                file.Close();
            }
        }
        private const double fileRetirementAgeInDays = 15.0;
        public void cleanDir(string dir)
        {
            //clean child files
            foreach (string file in Directory.GetFiles(dir))
            {
                double filesAge = (DateTime.Now - File.GetLastWriteTime(file)).TotalDays;
                if (filesAge > fileRetirementAgeInDays)
                {
                    logToFile("Deleting temporal file: " + file);
                    File.Delete(file);
                }
            }
            //clean child directories
            foreach (string subDir in Directory.GetDirectories(dir))
            {
                cleanDir(subDir);
                string[] childDirs = Directory.GetDirectories(subDir);
                string[] childFiles = Directory.GetFiles(subDir);
                if (childDirs.Length == 0 && childFiles.Length == 0)
                {
                    logToFile("Deleting temporal directory: " + subDir);
                    Directory.Delete(subDir);
                }
            }
        }
        public void cleanTempDir()
        {
            try
            {
                foreach (string dir in Directory.GetDirectories(m_dirPath))
                {
                    cleanDir(dir);
                    string[] childDirs = Directory.GetDirectories(dir);
                    string[] childFiles = Directory.GetFiles(dir);
                    if (childDirs.Length == 0 && childFiles.Length == 0)
                    {
                        logToFile("Deleting temporal directory: " + dir);
                        Directory.Delete(dir);
                    }
                }
            }
            catch (Exception ex)
            {
                logToFile("Exception cleaning temporal directory");
                logToFile(ex.ToString());
            }
        }
        public void logToFile(string logMessage)
        {
            lock (m_logFileLock)
            {
                if (File.Exists(getLogFilename()))
                {
                    string text = DateTime.Now.ToShortDateString() + " " +
                        DateTime.Now.ToShortTimeString() + ": " + logMessage;
                    StreamWriter w = File.AppendText(getLogFilename());

                    w.WriteLine(text);
                    w.Close();
                    Console.WriteLine(text);
                }
            }
        }

        internal void TestStartupAndStop(string[] args)
        {
            this.OnStart(args);
            Console.ReadLine();
            this.OnStop();
        }

        protected override void OnStart(string[] args)
        {
            DoStart();
        }

        public void DoStart()
        {
            m_herdAgent = new HerdAgent(new CancellationTokenSource());

            m_dirPath = Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "\\temp";
            Directory.CreateDirectory(m_dirPath);
            m_herdAgent.setLogMessageHandler(logToFile);
            cleanLog();
            cleanTempDir();
            logToFile("Herd agent started");

            m_herdAgent.startListening();
        }

        protected override void OnStop()
        {
            DoStop();
        }
        public void DoStop()
        {
            logToFile("Herd Agent stopped");
            m_herdAgent.stopListening();
        }
    }
}
