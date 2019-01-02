using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace Herd.Network
{
    public class JobTransmitter
    {
        public const string LegacyTaskHeaderRegEx = "<Task Name=\"([^\"]*)\" Exe=\"([^\"]*)\" Arguments=\"([^\"]*)\" Pipe=\"([^\"]*)\"/>";

        public const string TaskHeaderRegEx = "<Task Name=\"([^\"]*)\" Exe=\"([^\"]*)\" Arguments=\"([^\"]*)\" Pipe=\"([^\"]*)\"( AuthenticationToken=\"([^\"]*)\")?/>";
        public const string JobHeaderRegEx = "<Job Name=\"([^\"]*)\">";
        public const string JobFooterRegEx = "</Job>";
        public const string IncomingFileHeaderRegEx = "<(Input|Output) Name=\"([^\"]*)\" Size=\"([^\"]*)\">";
        public const string OutgoingFileHeaderRegEx = "<(Input|Output) Name=\"([^\"]*)\"/>";
        public const string FileFooterRegEx = "</(Input|Output)>";

        public XMLStream m_xmlStream;
        public const int m_discoveryPortHerd = 2333;
        public const int m_discoveryPortServer = 2334;
        public const int m_comPortHerd = 2335;
        public const string m_discoveryMessage = "Slaves, show yourselves!";
        public const string m_discoveryAnswer = "At your command, my Master";
        public const string m_acquireMessage = "You are mine now!";
        public const string m_endMessage = "Job finished correctly";
        public const string m_errorMessage = "Error running job";
        public const string m_quitMessage = "Stop";
        public const string m_freeMessage = "You are free";


        protected NetworkStream m_netStream;
        protected TcpClient m_tcpClient;

        //used for reading
        protected int m_nextFileSize;
        protected string m_nextFileName;

        protected Job m_job;

        protected Action<string> m_logMessageHandler;

        protected virtual string TmpDir()
        {
            return Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "/temp"; ;
        }

        public enum FileType { INPUT, OUTPUT };
        public string getFileTypeXMLTag(FileType type)
        {
            if (type == FileType.INPUT)
                return "Input";
            else if (type == FileType.OUTPUT) return "Output";
            LogMessage("Can't determine the XML tag of the unkwown file type");
            return "UnkwnownType";
        }
        public JobTransmitter()
        {
            m_job = new Job();
            m_xmlStream = new XMLStream();
            m_nextFileSize = 0;
            m_logMessageHandler = null;
        }

        private bool m_bEnqueueAsyncWrites = false;
        public void StartEnqueueingAsyncWriteOps() { m_bEnqueueAsyncWrites = true; }

        private List<Task> m_pendingAsyncWrites = new List<Task>();

        public void SetTCPClient(TcpClient client) { m_tcpClient = client; m_netStream = client.GetStream(); }

        public void SetLogMessageHandler(Action<string> logMessageHandler)
        {
            m_logMessageHandler = logMessageHandler;
            m_xmlStream.SetLogMessageHandler(logMessageHandler);
        }
        protected void LogMessage(string message) { m_logMessageHandler?.Invoke(message); }
        public void WriteMessage(string message, bool addDefaultMessageType = false)
        {
            m_xmlStream.WriteMessage(m_netStream, message, addDefaultMessageType);
        }
        public async Task WriteMessageAsync(string message, CancellationToken cancelToken, bool addDefaultMessageType = false)
        {
            await m_xmlStream.WriteMessageAsync(m_netStream, message, cancelToken, addDefaultMessageType);
        }

        public async Task<int> ReadAsync(CancellationToken cancelToken)
        {
            int numBytesRead = 0;
            try
            {
                numBytesRead = await m_xmlStream.ReadFromNetworkStreamAsync(m_tcpClient, m_netStream, cancelToken);
            }
            catch
            { }// { LogMessage("async read operation cancelled"); } //<- this logged too many messages on cancellation
            return numBytesRead;
        }
        public bool WriteAsync(byte[] buffer, int offset, int length, CancellationToken cancelToken)
        {
            if (!m_bEnqueueAsyncWrites)
            {
                try { m_netStream.WriteAsync(buffer, offset, length, cancelToken); }
                catch (OperationCanceledException) { LogMessage("async write operation cancelled"); }
            }
            else
            {
                try { m_pendingAsyncWrites.Add(m_netStream.WriteAsync(buffer, offset, length)); }
                catch (OperationCanceledException) { LogMessage("async write operation cancelled"); }
            }
            return true;
        }
        public void waitAsyncWriteOpsToFinish()
        {
            Task.WhenAll(m_pendingAsyncWrites).Wait();
            m_pendingAsyncWrites.Clear();
            m_bEnqueueAsyncWrites = false;
        }

        protected void SendInputFiles(bool sendContent, CancellationToken cancelToken)
        {
            foreach (string file in m_job.InputFiles)
            {
                SendFile(file, FileType.INPUT, sendContent, false, cancelToken, m_job.RenamedFilename(file));
            }
        }
        protected void SendOutputFiles(bool sendContent, CancellationToken cancelToken)
        {
            List<string> unknownFiles = new List<string>();
            foreach (string file in m_job.OutputFiles)
            {
                //some output files may not be ther if a task failed
                //we still want to try sending the rest of files
                if (!sendContent || File.Exists(getCachedFilename(file)))
                    SendFile(file, FileType.OUTPUT, sendContent, true, cancelToken, m_job.RenamedFilename(file));
                else
                {
                    unknownFiles.Add(file);
                    LogMessage("File " + file + " not found. Cannot be sent back to the client.");
                }
            }
            foreach (string file in unknownFiles)
                m_job.OutputFiles.Remove(file);
        }
        protected void SendTasks(CancellationToken cancelToken)
        {
            foreach (HerdTask task in m_job.Tasks)
                SendTask(task, cancelToken);
        }
        protected void SendTask(HerdTask task, CancellationToken cancelToken)
        {
            string taskXML = "<Task Name=\"" + task.Name + "\"";
            taskXML += " Exe=\"" + task.Exe + "\"";
            taskXML += " Arguments=\"" + task.Arguments + "\"";
            taskXML += " Pipe=\"" + task.Pipe + "\"";
            //taskXML += " AuthenticationToken=\"" + task.authenticationToken + "\"";
            taskXML += "/>";
            byte[] bytes = Encoding.ASCII.GetBytes(taskXML);

            WriteAsync(bytes, 0, bytes.Length, cancelToken);
        }

        public async Task<bool> ReceiveTask(CancellationToken cancelToken)
        {
            HerdTask task = new HerdTask();
            Match match;
            //This expression was tested and worked fine with and without the authentication token
            match = await ReadUntilMatchAsync(TaskHeaderRegEx, cancelToken);
            task.Name = match.Groups[1].Value;
            task.Exe = match.Groups[2].Value;
            task.Arguments = match.Groups[3].Value;
            task.Pipe = match.Groups[4].Value;
            if (match.Groups.Count>5)
                task.AuthenticationToken = match.Groups[6].Value;
            m_job.Tasks.Add(task);
            return true;
        }

        protected void SendJobHeader(CancellationToken cancelToken)
        {
            string header = "<Job Name=\"" + m_job.Name + "\">";
            byte[] headerbytes = Encoding.ASCII.GetBytes(header);
            WriteAsync(headerbytes, 0, headerbytes.Length, cancelToken);
        }
        protected void SendJobFooter(CancellationToken cancelToken)
        {
            string footer = "</Job>";
            byte[] footerbytes = Encoding.ASCII.GetBytes(footer);
            WriteAsync(footerbytes, 0, footerbytes.Length, cancelToken);
        }
        protected void SendFile(string fileName, FileType type, bool sendContent
            , bool fromCachedDir, CancellationToken cancelToken, string rename = null)
        {
            string fileTypeXMLTag;
            string header;
            byte[] headerBytes;
            string footer = "";

            fileTypeXMLTag = getFileTypeXMLTag(type);

            string sentFilename;
            if (rename != null) sentFilename = rename;
            else sentFilename = fileName;

            header = "<" + fileTypeXMLTag + " Name=\"" + sentFilename + "\"";
            if (sendContent) footer = "</" + fileTypeXMLTag + ">";

            if (sendContent)
            {
                FileStream fileStream = null;
                long fileSize = 0;
                LogMessage("Sending file " + fileName);

                if (fromCachedDir)
                    fileName = getCachedFilename(fileName);

                try { fileStream = File.OpenRead(fileName); }
                catch
                {
                    LogMessage("Could not find input file: " + fileName);
                    return;
                }

                fileSize = fileStream.Length;
                header += " Size=\"" + fileSize + "\">";

                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                WriteAsync(headerBytes, 0, headerBytes.Length, cancelToken);


                long readBytes = 0;
                int lastReadBytes;

                while (readBytes < fileSize)
                {
                    byte[] buffer = new byte[m_xmlStream.getBufferSize()];
                    lastReadBytes = fileStream.Read(buffer, 0, m_xmlStream.getBufferSize());
                    readBytes += lastReadBytes;

                    try
                    {
                        WriteAsync(buffer, 0, (int)lastReadBytes, cancelToken);
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message + ex.StackTrace);
                    }
                }
                fileStream.Close();

                //Send the footer: </Exe>, </Input> or </Output>
                byte[] footerBytes = Encoding.ASCII.GetBytes(footer);
                WriteAsync(footerBytes, 0, footerBytes.Length, cancelToken);
                m_netStream.Flush();
            }
            else
            {
                header += "/>";
                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                WriteAsync(headerBytes, 0, headerBytes.Length, cancelToken);

                m_netStream.Flush();
            }
        }


        public async Task<int> ReadFromStreamAsync(CancellationToken cancelToken)
        {
            int ret = await m_xmlStream.ReadFromNetworkStreamAsync(m_tcpClient, m_netStream, cancelToken);
            return ret;
        }

        public async Task<Match> ReadUntilMatchAsync(string pattern, CancellationToken cancelToken)
        {
            Match match;
            string header;
            int ret;
            do
            {
                header = m_xmlStream.processNextXMLTag();
                while (header == "") //there's nothing in the buffer or incomplete tags
                {
                    ret = await ReadFromStreamAsync(cancelToken);
                    header = m_xmlStream.processNextXMLTag();
                }

                match = Regex.Match(header, pattern);

                if (!match.Success)
                {
                    LogMessage("WARNING: Unexpected XML tag in ReadUntilMatchAsync(): " + header);
                    LogMessage("Buffer contents: " + Encoding.Default.GetString(m_xmlStream.getBuffer()));
                }
            }
            while (!match.Success);

            return match;
        }
        public async Task<int> ReceiveJobHeader(CancellationToken cancelToken)
        {
            Match match;
            match = await ReadUntilMatchAsync(JobHeaderRegEx, cancelToken);

            m_job.Name = match.Groups[1].Value;

            return 1;

            //i think this is just a bug
            //return await ReadFromStreamAsync(cancelToken); //we shift the buffer to the left skipping the processed header
        }

        protected async Task<bool> ReceiveJobFooter(CancellationToken cancelToken)
        {
            Match ret = await ReadUntilMatchAsync(JobFooterRegEx, cancelToken);
            return true;
        }

        protected async Task<bool> ReceiveFile(FileType type, bool receiveContent, bool inCachedDir, CancellationToken cancelToken)
        {
            bool ret = await ReceiveFileHeader(type, receiveContent, inCachedDir, cancelToken);
            if (receiveContent)
            {
                LogMessage("Receiving file data: " + m_nextFileName + " (" + m_nextFileSize + " bytes)");
                ret = await ReceiveFileData(inCachedDir, cancelToken);
                LogMessage("Receiving file footer");
                ret = await ReceiveFileFooter(type, cancelToken);
                LogMessage("File transfer complete");
            }
            return true;
        }
        protected async Task<bool> ReceiveFileHeader(FileType type, bool receiveContent, bool inCachedDir, CancellationToken cancelToken)
        {
            Match match;

            if (receiveContent)
                match = await ReadUntilMatchAsync(IncomingFileHeaderRegEx, cancelToken);
            else match = await ReadUntilMatchAsync(OutgoingFileHeaderRegEx, cancelToken);


            if ((match.Groups[1].Value == "Input" && type != FileType.INPUT)
                || (match.Groups[1].Value == "Output" && type != FileType.OUTPUT))
            {
                LogMessage("Error (CJobDispatcher::ReceiveFileHeader): The type of the file received missmatches the expected file type");
                return false;
            }

            if (type == FileType.INPUT) m_job.InputFiles.Add(match.Groups[2].Value);
            else m_job.OutputFiles.Add(match.Groups[2].Value);

            m_nextFileName = m_job.OriginalFilename(match.Groups[2].Value);

            if (receiveContent) m_nextFileSize = Int32.Parse(match.Groups[3].Value);
            else m_nextFileSize = 0;

            //We create the necessary directories for the file, be it an exe, an input or an output file
            string outputFilename;
            if (inCachedDir)
                outputFilename = getCachedFilename(m_nextFileName);
            else outputFilename = m_nextFileName;
            string outputDir = Path.GetDirectoryName(outputFilename);

            if (!Directory.Exists(outputDir))
                Directory.CreateDirectory(outputDir);

            return true;
        }
        protected async Task<bool> ReceiveFileFooter(FileType type, CancellationToken cancelToken)
        {
            Match match;
            match = await ReadUntilMatchAsync(FileFooterRegEx, cancelToken);
            return true;
        }
        protected int SaveBufferToFile(FileStream outputFile, int bytesLeft, bool bFileOpen = true)
        {
            int bytesToWrite = Math.Min(bytesLeft, m_xmlStream.getBytesInBuffer() - m_xmlStream.getBufferOffset());

            //for convenience, this function may be called even if the file wasn't correctly opened
            if (bFileOpen)
                outputFile.Write(m_xmlStream.getBuffer(), m_xmlStream.getBufferOffset(), bytesToWrite);

            m_xmlStream.addProcessedBytes(bytesToWrite);// m_bufferOffset += bytesToWrite;
            return bytesToWrite;
        }
        protected string getCachedFilename(string originalFilename)
        {
            string outputFilename = TmpDir().TrimEnd('/', '\\') + "/" + originalFilename.TrimStart('.', '/', '\\');
            return outputFilename;
        }
        protected async Task<bool> ReceiveFileData(bool inCachedDir, CancellationToken cancelToken)
        {
            int bytesLeft = m_nextFileSize;

            string outputFilename;
            if (inCachedDir)
                outputFilename = getCachedFilename(m_nextFileName);
            else
                outputFilename = m_nextFileName;

            FileStream outputFile = null;
            bool bFileOpen = true;
            try
            {
                outputFile = File.Open(outputFilename, FileMode.Create);
            }
            catch
            {
                bFileOpen = false;
                LogMessage("Failed to create file " + outputFilename);
            }
            int ret;
            //we may have already in the buffer the data
            if (m_xmlStream.getBytesInBuffer() - m_xmlStream.getBufferOffset() > 0)
                bytesLeft -= SaveBufferToFile(outputFile, bytesLeft, bFileOpen);
            //read if we have to until the file has been read
            while (bytesLeft > 0)
            {
                ret = await ReadFromStreamAsync(cancelToken);
                bytesLeft -= SaveBufferToFile(outputFile, bytesLeft, bFileOpen);
            }
            if (bFileOpen) outputFile.Close();
            LogMessage("Closed file: " + outputFilename);

            return true;
        }
    }
}
