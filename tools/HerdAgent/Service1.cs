using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Net;
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

        private object m_logFileLock= new object();

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
        public void Log(string logMessage)
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
        public class TCPCallbackInfo
        {
            public HerdAgent herdAgent { get; set; }
            public NetworkStream netStream { get; set; }
            public XMLStream xmlStream { get; set; }
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
            m_herdAgent.setDirPath(m_dirPath);
            m_herdAgent.setLogMessageHandler(Log);
            cleanLog();
            Log("Herd agent started");

            m_herdAgent.startListening();
        }

        protected override void OnStop()
        {
            DoStop();
        }
        public void DoStop()
        {
            Log("Herd Agent stopped");
            m_herdAgent.stopListening();
        }
    }
}
