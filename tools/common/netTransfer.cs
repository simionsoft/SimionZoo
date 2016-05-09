using System.Text.RegularExpressions;
using System.IO;
using System.Net.Sockets;
using System.Net;
using System;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Xml.Linq;
using System.Threading.Tasks;
using System.Threading;
using System.IO.Pipes;
using System.Xml;

namespace NetJobTransfer
{
    public class CJob
    {
        public string name;
        public List<string> inputFiles;
        public List<string> outputFiles;
        public string exeFile;
        public List<string> comLineArgs;

        public CJob()
        {
            //comLineArgs = "";
            name = "";
            exeFile = "";
            comLineArgs = new List<string>();
            inputFiles = new List<string>();
            outputFiles = new List<string>();
        }
    }
    public enum AgentState { BUSY, AVAILABLE, DISCOVERED,CANCELING };
    public enum FileType { EXE, INPUT, OUTPUT };
    public class CJobDispatcher
    {
        public const int m_discoveryPortShepherd = 2332;
        public const int m_discoveryPortHerd = 2333;
        public const int m_comPortShepherd = 2334;
        public const int m_comPortHerd = 2335;
        public const string m_discoveryMessage = "Slaves, show yourselves!";
        public const string m_discoveryAnswer = "At your command, my Master";
        static int m_maxChunkSize = 1024;

        protected byte[] m_buffer;

        protected NetworkStream m_netStream;

        protected int m_bytesInBuffer;
        protected int m_bufferOffset;

        //used for reading
        protected int m_nextFileSize;
        protected string m_nextFileName;

        protected string m_tempDir;
        protected CJob m_job;
        protected int m_numInputFilesRead;
        protected int m_numOutputFilesRead;
        protected int m_numTasksRead;

        public CJobDispatcher()
        {
            m_job = new CJob();
            m_buffer = new byte[m_maxChunkSize];
            m_bytesInBuffer = 0;
            m_bufferOffset = 0;
            m_nextFileSize = 0;
            m_numInputFilesRead = 0;
            m_numOutputFilesRead = 0;
            m_numTasksRead = 0;
            m_tempDir = "c:\\temp";
        }


        protected void SendExeFiles(bool sendContent) { if (m_job.exeFile != "") SendFile(m_job.exeFile, FileType.EXE, sendContent, false); }
        protected void SendInputFiles(bool sendContent) { foreach (string file in m_job.inputFiles) SendFile(file, FileType.INPUT, sendContent, false); }
        protected void SendOutputFiles(bool sendContent) { foreach (string file in m_job.outputFiles) SendFile(file, FileType.OUTPUT, sendContent, true); }
        protected void SendJobHeader()
        {
            string header = "<Job Name=\"" + m_job.name + "\" NumTasks=\"" 
                + m_job.comLineArgs.Count + "\" NumInputFiles=\"" + m_job.inputFiles.Count + "\" NumOutputFiles=\""
                + m_job.outputFiles.Count + "\">";
            string args = "";// "<Args>";
            foreach (string arg in m_job.comLineArgs)
            {
                args += "<Arg>" + arg + "</Arg>";
            }
           // args += "</Args>";
            header += args;
            byte[] headerbytes = Encoding.ASCII.GetBytes(header);
            m_netStream.Write(headerbytes, 0, headerbytes.Length);
        }
        protected void SendJobFooter()
        {
            string footer = "</Job>";
            byte[] footerbytes = Encoding.ASCII.GetBytes(footer);
            m_netStream.Write(footerbytes, 0, footerbytes.Length);
        }
        protected void SendFile(string fileName, FileType type, bool sendContent, bool fromCachedDir)
        {
            string header;
            byte[] headerBytes;
            string footer = "";

            if (type == FileType.EXE)
            {
                header = "<Exe Name=\"" + fileName + "\" ComLineArgs=\"" + m_job.comLineArgs + "\"";
                if (sendContent) footer = "</Exe>";
            }
            else if (type == FileType.INPUT)
            {
                header = "<Input Name=\"" + fileName + "\"";
                if (sendContent) footer = "</Input>";
            }
            else //output
            {
                header = "<Output Name=\"" + fileName + "\"";
                if (sendContent) footer = "</Output>";
            }

            if (sendContent)
            {
                FileStream fileStream;
                long fileSize = 0;

                if (fromCachedDir)
                    fileName = getCachedFilename(fileName);

                fileStream = File.Open(fileName, FileMode.Open);
                fileSize = fileStream.Length;
                header += " Size=\"" + fileSize + "\">";

                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                m_netStream.Write(headerBytes, 0, headerBytes.Length);

                //send the content if it has to be sent
                if (sendContent)
                {
                    long readBytes = 0;
                    int lastReadBytes;
                    byte[] buffer = new byte[m_maxChunkSize];
                    while (readBytes < fileSize)
                    {
                        lastReadBytes = fileStream.Read(buffer, 0, m_maxChunkSize);
                        readBytes += lastReadBytes;
                        m_netStream.Write(buffer, 0, (int)lastReadBytes);
                    }
                    fileStream.Close();
                }

                //Send the footer: </Exe>, </Input> or </Output>
                byte[] footerBytes = Encoding.ASCII.GetBytes(footer);
                m_netStream.Write(footerBytes, 0, footerBytes.Length);

                m_netStream.Flush();
            }
            else
            {
                header += "/>";
                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                m_netStream.Write(headerBytes, 0, headerBytes.Length);

                ////Send the footer: </Exe>, </Input> or </Output>
                //byte[] footerBytes = Encoding.ASCII.GetBytes(footer);
                //m_netStream.Write(footerBytes, 0, footerBytes.Length);

                m_netStream.Flush();
            }

        }
        public string GetBufferAsString()
        {
            return Encoding.ASCII.GetString(m_buffer);
        }
        public void markBytesAsProcessed(int numBytesProcessed)
        {
            m_bufferOffset += numBytesProcessed;
        }
        public void ReadFromStream()
        {
            int bytesLeftToProcess = m_bytesInBuffer - m_bufferOffset;
            //something left to process in the buffer?
            if (m_bufferOffset != 0)
            {
                Buffer.BlockCopy(m_buffer, m_bufferOffset, m_buffer, 0, m_bytesInBuffer - m_bufferOffset);
                m_bytesInBuffer = m_bytesInBuffer - m_bufferOffset;
                m_bufferOffset = 0;
            }
            while (m_bytesInBuffer == 0)
            {
                if (m_netStream.DataAvailable && m_bytesInBuffer < m_maxChunkSize)
                    m_bytesInBuffer += m_netStream.Read(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer);

                if (m_bytesInBuffer == 0) Thread.Sleep(100);
            }
        }
        protected void ReceiveJobHeader()
        {
            Match match;
            string header;

            do
            {
                ReadFromStream();
                header = Encoding.ASCII.GetString(m_buffer);
                //string[] headerAndArgs = header.Split(new string[] { "<Args>", "</Args>" }, StringSplitOptions.None);
                //header = headerAndArgs[0];
                //string args = null;
                //if (headerAndArgs.Length > 1)
                //{

                //    XElement[] argsE = XDocument.Parse("<args>" + headerAndArgs[1] + "</args>")
                //    .Descendants()
                //    .Where(e => e.Name == "Arg")
                //    .ToArray();
                //    foreach (XElement arg in argsE)
                //    {
                //        m_job.comLineArgs.Add(arg.Value);
                //    }
                //}
                //Console.WriteLine(args);
                match = Regex.Match(header, "<Job Name=\"([^\"]*)\" NumTasks=\"([^\"]*)\" NumInputFiles=\"([^\"]*)\" NumOutputFiles=\"([^\"]*)\">");
            }
            while (!match.Success);




            m_job.name = match.Groups[1].Value;
            m_numTasksRead = Int32.Parse(match.Groups[2].Value);
            m_numInputFilesRead = Int32.Parse(match.Groups[3].Value);
            m_numOutputFilesRead = Int32.Parse(match.Groups[4].Value);
            m_bufferOffset += match.Index + match.Length;

            //read arguments
            for (int i = 0; i < m_numTasksRead; i++)
            {
                ReadFromStream();
                header = Encoding.ASCII.GetString(m_buffer);

                match = Regex.Match(header, "<Arg>([^<]*)</Arg>");

                m_job.comLineArgs.Add(match.Groups[1].Value);
                m_bufferOffset += match.Index + match.Length;
            }

            ReadFromStream(); //we shift the buffer to the left skipping the processed header
        }

        protected void ReceiveJobFooter()
        {
            Match match;

            string header;
            do
            {
                ReadFromStream();
                header= Encoding.ASCII.GetString(m_buffer);
                match = Regex.Match(header, "</Job>");
            }
            while (!match.Success);
            m_bufferOffset += match.Index + match.Length;
        }
        protected void ReceiveExeFiles(bool receiveContent) { ReceiveFile(FileType.EXE, receiveContent, true); }
        protected void ReceiveInputFiles(bool receiveContent)
        {
            for (int i = 0; i < m_numInputFilesRead; i++)
                ReceiveFile(FileType.INPUT, receiveContent, true);
        }
        protected void ReceiveOutputFiles(bool receiveContent)
        {
            for (int i = 0; i < m_numOutputFilesRead; i++)
                ReceiveFile(FileType.OUTPUT, receiveContent, false);
        }

        protected void ReceiveFile(FileType type, bool receiveContent, bool inCachedDir)
        {
            ReceiveFileHeader(type, receiveContent);
            if (receiveContent)
            {
                ReceiveFileData(inCachedDir);
                ReceiveFileFooter(type);
            }
        }
        protected void ReceiveFileHeader(FileType type, bool receiveContent)
        {
            Match match;

            do
            {
                ReadFromStream();
                string header = Encoding.ASCII.GetString(m_buffer);

                if (type == FileType.EXE)
                {
                    if (receiveContent)
                        match = Regex.Match(header, "<(Exe) Name=\"([^\"]*)\" ComLineArgs=\"([^\"]*)\" Size=\"([^\"]*)\">");
                    else match = Regex.Match(header, "<(Exe) Name=\"([^\"]*)\" ComLineArgs=\"([^\"]*)\"/>");
                }
                else
                {
                    if (receiveContent)
                        match = Regex.Match(header, "<(Input|Output) Name=\"([^\"]*)\" Size=\"([^\"]*)\">");
                    else match = Regex.Match(header, "<(Input|Output) Name=\"([^\"]*)\"/>");
                }
            }
            while (!match.Success);


            if ((match.Groups[1].Value == "Exe" && type != FileType.EXE)
                || (match.Groups[1].Value == "Input" && type != FileType.INPUT)
                || (match.Groups[1].Value == "Output" && type != FileType.OUTPUT))
                Debug.Assert(false, "The type of the file received missmatches the expected file type");

            if (type == FileType.EXE)
            {

                m_job.exeFile = match.Groups[2].Value;
                m_nextFileName = match.Groups[2].Value;
                if (receiveContent) m_nextFileSize = Int32.Parse(match.Groups[4].Value);
                else m_nextFileSize = 0;
            }
            else
            {
                if (type == FileType.INPUT) m_job.inputFiles.Add(match.Groups[2].Value);
                else m_job.outputFiles.Add(match.Groups[2].Value);

                m_nextFileName = match.Groups[2].Value;

                if (receiveContent) m_nextFileSize = Int32.Parse(match.Groups[3].Value);
                else m_nextFileSize = 0;
            }

            //We create the necessary directories for the file, be it an exe, an input or an output file
            string outputFilename = getCachedFilename(m_nextFileName);
            string outputDir = Path.GetDirectoryName(outputFilename);
            System.IO.Directory.CreateDirectory(outputDir);

            m_bufferOffset += match.Index + match.Length;
        }
        protected void ReceiveFileFooter(FileType type)
        {
            Match match;

            do
            {
                ReadFromStream();
                string header = Encoding.ASCII.GetString(m_buffer);

                if (type == FileType.EXE) match = Regex.Match(header, "</Exe>");
                else if (type == FileType.INPUT) match = Regex.Match(header, "</Input>");
                else match = Regex.Match(header, "</Output>");
            }
            while (!match.Success);

            m_bufferOffset += match.Length + match.Index;
        }
        protected int SaveBufferToFile(FileStream outputFile, int bytesLeft)
        {
            int bytesToWrite = Math.Min(bytesLeft, m_bytesInBuffer - m_bufferOffset);

            outputFile.Write(m_buffer, m_bufferOffset, bytesToWrite);

            m_bufferOffset += bytesToWrite;
            return bytesToWrite;
        }
        protected string getCachedFilename(string originalFilename)
        {
            string outputFilename = m_tempDir.TrimEnd('/', '\\') + "\\" + originalFilename.TrimStart('.', '/', '\\');
            return outputFilename;
        }
        protected void ReceiveFileData(bool inCachedDir)
        {
            int bytesLeft = m_nextFileSize;

            string outputFilename;
            if (inCachedDir)
                outputFilename = getCachedFilename(m_nextFileName);
            else
                outputFilename = m_nextFileName;

            FileStream outputFile = File.Open(outputFilename, FileMode.Create);

            do
            {
                ReadFromStream();
                bytesLeft -= SaveBufferToFile(outputFile, bytesLeft);
            } while (bytesLeft > 0);
            outputFile.Close();

            ReadFromStream(); //discard processed data
        }
    }
    public class Shepherd : CJobDispatcher
    {
        public void SendJobQuery(NetworkStream netStream, CJob job)
        {
            m_netStream = netStream;
            m_job = job;
            SendJobHeader();
            SendExeFiles(true);
            SendInputFiles(true);
            SendOutputFiles(false);
            SendJobFooter();
        }
        public bool ReceiveJobResult(NetworkStream netStream)
        {
            m_netStream = netStream;
            m_bufferOffset = 0;
            m_bytesInBuffer = 0;

            ReceiveJobHeader();
            ReceiveExeFiles(false);
            ReceiveInputFiles(false);
            ReceiveOutputFiles(true);
            ReceiveJobFooter();

            return true;//if job result properly received. For now, we will assume it}
        }


    }
    public class HerdAgent : CJobDispatcher
    {
        private CancellationTokenSource cts;
        private IPEndPoint master;

        public void CancelRunningProcesses()
        {
            if (cts != null)
                cts.Cancel();
        }
        public void SendJobResult(NetworkStream netStream)
        {
            m_netStream = netStream;
            SendJobHeader();
            SendExeFiles(false);
            SendInputFiles(false);
            SendOutputFiles(true);
            SendJobFooter();
        }
        public bool ReceiveJobQuery(NetworkStream netStream)
        {
            m_netStream = netStream;
            m_bufferOffset = 0;
            m_bytesInBuffer = 0;

            ReceiveJobHeader();
            //ReceiveJobArgs();
            ReceiveExeFiles(true);
            ReceiveInputFiles(true);
            ReceiveOutputFiles(false);
            ReceiveJobFooter();

            return true;//if job query properly received. For now, we will assume it
        }
        private void runOneProcess(Process p,string pipeName, NamedPipeServerStream server, CancellationToken ct,NetworkStream bridge)
        {

            ProcessStartInfo startInfo = new ProcessStartInfo();
            

            //not to read 23.232 as 23232
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            //Task.Delay(1000).Wait();


          
            p.Start();
            //Process.Start(startInfo);
            server.WaitForConnection();
            StreamReader reader = new StreamReader(server);

            while (server.IsConnected)
            {
                string sms = reader.ReadLine();
                if (sms != null)
                {
                    //we have to add a header or new root to this sms, otherwise the master will not be able to know who is the reciever
                    byte[] message = Encoding.ASCII.GetBytes("<" + pipeName + ">" + sms + "</" + pipeName + ">");
                    byte[] fixedSMS = new byte[256];
                    for (int i = 0; i < fixedSMS.Length; i++)
                    {
                        fixedSMS[i] = 32;
                    }
                    if(message.Length>256)
                    {
                        int bytesToRemove = message.Length-256;
                        
                    }
                    else
                    {
                        Array.Copy(message, fixedSMS, message.Length);
                        bridge.Write(fixedSMS, 0, fixedSMS.Length);
                    }
                    
                    

                }
            }
            //ids.Remove(p);
            reader.Close();
            //readers.Remove(reader);
            server.Close();
            //readers.Remove(server);
        }
       
        public void RunJob(NetworkStream bridgeStream)
        {
            cts = new CancellationTokenSource();
            ParallelOptions po = new ParallelOptions();
            po.CancellationToken = cts.Token;
            //var t = Task.Factory.StartNew(() =>
            //{
                Parallel.ForEach(m_job.comLineArgs, po, (args) =>
                {
                    using (cts.Token.Register(Thread.CurrentThread.Abort))
                    {
                        string[] arguments = args.Split(' ');
                        NamedPipeServerStream server = new NamedPipeServerStream(arguments[1]);
                        Process myProcess = new Process();
                       
                        try
                        {
                              
                            myProcess.StartInfo.FileName = getCachedFilename(m_job.exeFile);
                            myProcess.StartInfo.Arguments = args;
                            myProcess.StartInfo.WorkingDirectory = Path.GetDirectoryName(myProcess.StartInfo.FileName);
                            Console.WriteLine("Running command: " + myProcess.StartInfo.FileName + " " + myProcess.StartInfo.Arguments);
                            runOneProcess(myProcess, arguments[1],server, cts.Token,bridgeStream);
                            Console.WriteLine("Exit code: " + myProcess.ExitCode);
                        }
                        catch (Exception ex)
                        {
                            if (myProcess != null && !myProcess.HasExited)
                            {
                                myProcess.Kill();
                                myProcess.Dispose();
                            }
                        }

                    }
                });

           // }, cts.Token);          
          //  t.Wait();
            }

        public void stop()
        {
            if(cts!=null)
                cts.Cancel();
        }
    }
        public class UdpState
        {
            public IPEndPoint e { get; set; }
            public UdpClient u { get; set; }
            public HerdAgent c { get; set; }
            public AgentState st { get; set; }
        }
        public class PipesBridgeClient
        {
            private HerdAgent myHerdAgent;

            public PipesBridgeClient(IPAddress server, HerdAgent agent)
            {
                myHerdAgent = agent;
                server = IPAddress.Any;
                IPEndPoint e = new IPEndPoint(server, 8888);
                UdpClient u = new UdpClient(e);
                UdpState s = new UdpState();
                s.e = e;
                s.u = u;
                Console.WriteLine("listening for messages");
                u.BeginReceive(new AsyncCallback(ReceiveCallback), s);
            }
            private void ReceiveCallback(IAsyncResult ar)
            {
                Task.Factory.StartNew(() =>
                {
                    UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
                    IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

                    Byte[] receiveBytes = u.EndReceive(ar, ref e);
                    string receiveString = Encoding.ASCII.GetString(receiveBytes);
                    if (receiveString.Equals("QUIT"))
                    {
                        myHerdAgent.CancelRunningProcesses();
                    }
                });
            }
        }
    }

    
