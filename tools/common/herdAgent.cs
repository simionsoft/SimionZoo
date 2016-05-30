using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;

namespace Herd
{
    public class UdpState
    {
        public UdpClient client { get; set; }
        public IPEndPoint ip { get; set; }
    }
    public class TCPState
    {
        public IPEndPoint ip { get; set; }
    }

    public class HerdAgent : CJobDispatcher
    {
        private CancellationTokenSource cts;
        private List<Process> m_spawnedProcesses = new List<Process>();
        private object m_quitExecutionLock = new object();
        public const int m_remotelyCancelledErrorCode = -1;
        public const int m_jobInternalErrorCode = -2;
        public const int m_noErrorCode = 0;

        public HerdAgent(TcpClient tcpClient, NetworkStream netStream, string dirPath, LogMessageHandler logMessageHandler)
            : base(tcpClient, dirPath, logMessageHandler)
        {
            m_netStream = netStream;
            m_xmlStream.resizeBuffer(tcpClient.ReceiveBufferSize);
        }
        
        private void killSpawnedProcesses()
        {
            lock (m_quitExecutionLock)
            {
                foreach (Process p in m_spawnedProcesses)
                {
                    if (!p.HasExited)
                    {
                        try
                        {
                            p.Kill();
                            p.Dispose();
                        }
                        catch
                        {
                            m_logMessageHandler("Exception received while killing process");
                            //do nothing
                        }
                    }
                }
                m_spawnedProcesses.Clear();
            }
        }
        private void addSpawnedProcessToList(Process p)
        {
            lock (m_quitExecutionLock)
            {
                m_spawnedProcesses.Add(p);
            }
        }
        private void removeSpawnedProcessFromList(Process p)
        {
            lock (m_quitExecutionLock)
            {
                m_spawnedProcesses.Remove(p);
            }
        }
        public void Stop()
        {
            if (cts != null)
            {
                try
                { 
                    cts.Cancel(); 
                }
                catch (Exception ex)
                {
                    m_logMessageHandler(ex.ToString());
                }
            }
            killSpawnedProcesses();
        }



        public void SendJobResult()
        {
            SendJobHeader();
            SendExeFiles(false);
            SendInputFiles(false);
            SendOutputFiles(true);
            SendJobFooter();
        }
        public bool ReceiveJobQuery()
        {

            ReceiveJobHeader();
            //ReceiveJobArgs();
            ReceiveExeFiles(true, true);
            ReceiveInputFiles(true, true);
            ReceiveOutputFiles(false, true);
            ReceiveJobFooter();

            return true;//if job query properly received. For now, we will assume it
        }
        //private void runOneProcess(Process p, string pipeName, NamedPipeServerStream server, CancellationToken ct, NetworkStream bridge)
        //{
        //    //not to read 23.232 as 23232
        //    Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

        //    p.Start();

        //    server.WaitForConnection();
        //    XMLStream xmlStream = new XMLStream();

        //    string xmlItem;
        //    while (server.IsConnected)
        //    {
        //        xmlStream.readFromNamedPipeStream(server);
        //        xmlItem = xmlStream.processNextXMLItem();
        //        if (xmlItem != "")
        //        {
        //            //checkConnection(m_tcpClient);
        //            xmlStream.writeMessage(bridge, "<" + pipeName + ">" + xmlItem + "</" + pipeName + ">", false);
        //        }
        //    }

        //    server.Close();

        //}

        public int RunJob(NetworkStream bridgeStream)
        {
            int returnCode = 0;
            cts = new CancellationTokenSource();
            ParallelOptions po = new ParallelOptions();
            po.MaxDegreeOfParallelism = Environment.ProcessorCount;
            po.CancellationToken = cts.Token;
            try
            {
                Parallel.ForEach(m_job.comLineArgs, po, (args) =>
                {
                    //using (cts.Token.Register(Thread.CurrentThread.Abort))
                    {
                        Process myProcess = new Process();
                        string[] arguments = args.Split(' ');
                        NamedPipeServerStream server = new NamedPipeServerStream(arguments[1]);
                        try
                        {
                            myProcess.StartInfo.FileName = getCachedFilename(m_job.exeFile);
                            myProcess.StartInfo.Arguments = args;
                            myProcess.StartInfo.WorkingDirectory = Path.GetDirectoryName(myProcess.StartInfo.FileName);
                            m_logMessageHandler("Running command: " + myProcess.StartInfo.FileName + " " + myProcess.StartInfo.Arguments);

                            //not to read 23.232 as 23232
                            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

                            myProcess.Start();
                            addSpawnedProcessToList(myProcess);//m_spawnedProcesses.Add(myProcess);

                            server.WaitForConnection();
                            XMLStream xmlStream = new XMLStream();

                            string xmlItem;
                            while (server.IsConnected)
                            {
                                //check if we have asked to cancel
                                po.CancellationToken.ThrowIfCancellationRequested();

                                xmlStream.readFromNamedPipeStream(server);
                                xmlItem = xmlStream.processNextXMLItem();
                                if (xmlItem != "")
                                {
                                    //checkConnection(m_tcpClient);
                                    xmlStream.writeMessage(bridgeStream, "<" + arguments[1] + ">" + xmlItem + "</" + arguments[1] + ">", false);
                                }
                            }
                            po.CancellationToken.ThrowIfCancellationRequested();
                            myProcess.WaitForExit();

                            if (myProcess.ExitCode < 0)
                                returnCode = m_jobInternalErrorCode;
                            m_logMessageHandler("Exit code: " + myProcess.ExitCode);
                        }
                        catch (OperationCanceledException)
                        {
                            m_logMessageHandler("Thread finished gracefully");
                            returnCode = m_remotelyCancelledErrorCode;
                        }
                        finally
                        {
                            removeSpawnedProcessFromList(myProcess);
                            server.Close();
                        }
                    }
                });
            }
            catch (OperationCanceledException)
            {
                m_logMessageHandler("Job cancelled gracefully");
            }
            catch (Exception ex)
            {
                m_logMessageHandler(ex.ToString());
                returnCode = m_remotelyCancelledErrorCode;
            }
            finally
            {
                cts.Dispose();
                cts = null;
            }
            return returnCode;
        }

        public void stop()
        {
            if (cts != null)
                cts.Cancel();
        }
    }

}
