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
        public void startEnqueueingAsyncWriteOps() { m_bEnqueueAsyncWrites = true; }

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

        public async Task<int> readAsync(CancellationToken cancelToken)
        {
            int numBytesRead = 0;
            try { numBytesRead = await m_xmlStream.ReadFromNetworkStreamAsync(m_tcpClient, m_netStream, cancelToken); }
            catch { LogMessage("async read operation cancelled"); }
            return numBytesRead;
        }
        public bool writeAsync(byte[] buffer, int offset, int length, CancellationToken cancelToken)
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

            writeAsync(bytes, 0, bytes.Length, cancelToken);
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
            writeAsync(headerbytes, 0, headerbytes.Length, cancelToken);
        }
        protected void SendJobFooter(CancellationToken cancelToken)
        {
            string footer = "</Job>";
            byte[] footerbytes = Encoding.ASCII.GetBytes(footer);
            writeAsync(footerbytes, 0, footerbytes.Length, cancelToken);
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
                writeAsync(headerBytes, 0, headerBytes.Length, cancelToken);


                long readBytes = 0;
                int lastReadBytes;

                while (readBytes < fileSize)
                {
                    byte[] buffer = new byte[m_xmlStream.getBufferSize()];
                    lastReadBytes = fileStream.Read(buffer, 0, m_xmlStream.getBufferSize());
                    readBytes += lastReadBytes;

                    try
                    {
                        writeAsync(buffer, 0, (int)lastReadBytes, cancelToken);
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message + ex.StackTrace);
                    }
                }
                fileStream.Close();

                //Send the footer: </Exe>, </Input> or </Output>
                byte[] footerBytes = Encoding.ASCII.GetBytes(footer);
                writeAsync(footerBytes, 0, footerBytes.Length, cancelToken);
                m_netStream.Flush();
            }
            else
            {
                header += "/>";
                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                writeAsync(headerBytes, 0, headerBytes.Length, cancelToken);

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


    public class XMLStream
    {
        private int m_bufferOffset;
        private int m_bytesInBuffer;
        private byte[] m_buffer;
        private Match m_match;
        private int m_maxChunkSize = 65536;
        private string m_asciiBuffer;
        private string m_lastXMLItem;
        public const string m_defaultMessageType = "Internal";

        protected Action<string> m_logMessageHandler;
        public void SetLogMessageHandler(Action<string> logMessageHandler) { m_logMessageHandler = logMessageHandler; }
        protected void logMessage(string message) { m_logMessageHandler?.Invoke(message); }

        public XMLStream()
        {
            m_buffer = new byte[m_maxChunkSize];
        }
        public int getBufferSize() { return m_maxChunkSize; }
        public void resizeBuffer(int newSize)
        {
            m_maxChunkSize = newSize;
            m_buffer = new byte[m_maxChunkSize];
        }
        public int getBytesInBuffer() { return m_bytesInBuffer; }
        public int getBufferOffset() { return m_bufferOffset; }
        public byte[] getBuffer() { return m_buffer; }
        public void addBytesRead(int bytesReadInBuffer) { m_bytesInBuffer += bytesReadInBuffer; }
        public void addProcessedBytes(int processedBytes) { m_bufferOffset += processedBytes; }
        public void discardProcessedData()
        {
            //shift left unprocessed bytes to discard processed data
            if (m_bufferOffset != 0)
            {
                Buffer.BlockCopy(m_buffer, m_bufferOffset, m_buffer, 0, m_bytesInBuffer - m_bufferOffset);
                m_bytesInBuffer = m_bytesInBuffer - m_bufferOffset;
                m_bufferOffset = 0;
            }
        }
        public void WriteMessage(NetworkStream stream, string message, bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            stream.Write(msg, 0, msg.Length);
        }
        public async Task WriteMessageAsync(NetworkStream stream, string message, CancellationToken cancelToken, bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            try
            {
                await stream.WriteAsync(msg, 0, msg.Length, cancelToken);
            }
            catch (OperationCanceledException)
            {
                logMessage("Write operation cancelled");
            }
            catch (Exception ex)
            {
                logMessage("Unhandled exception in writeMessageAsync");
                logMessage(ex.ToString());
            }
        }
        public void writeMessage(NamedPipeServerStream stream, string message, bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            stream.Write(msg, 0, msg.Length);
        }
        public async Task writeMessageAsync(NamedPipeServerStream stream, string message, CancellationToken cancelToken, bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            try
            {
                await stream.WriteAsync(msg, 0, msg.Length, cancelToken);
            }
            catch
            {
                logMessage("write operation cancelled");
            }
        }

        public async Task<int> ReadFromNetworkStreamAsync(TcpClient client, NetworkStream stream, CancellationToken cancelToken)
        {
            int numBytesRead = 0;
            discardProcessedData();
            //read if there's something to read and if we have available storage
            try
            {
                numBytesRead = await
                    stream.ReadAsync(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer, cancelToken);
            }
            catch (OperationCanceledException)
            { logMessage("async read from network stream cancelled"); }
            m_bytesInBuffer += numBytesRead;
            return numBytesRead;
        }

        public async Task<int> readFromNamedPipeStreamAsync(NamedPipeServerStream stream, CancellationToken cancelToken)
        {
            int numBytesRead = 0;
            discardProcessedData();
            //read if there's something to read and if we have available storage
            if (m_bytesInBuffer < m_maxChunkSize)
            {
                try
                {
                    numBytesRead = await stream.ReadAsync(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer, cancelToken);
                }
                catch (OperationCanceledException)
                {
                    logMessage("async read from pipe opeartion cancelled");
                }
                m_bytesInBuffer += numBytesRead;
            }
            return numBytesRead;
        }
        public string peekNextXMLItem()
        {
            return processNextXMLItem(false);
        }
        //returns the next complete xml element (NO ATTRIBUTES!!) in the stream
        //empty string if there was none
        public string processNextXMLItem(bool bMarkAsProcessed = true)
        {
            if (m_bytesInBuffer > 0)
            {
                discardProcessedData();
                m_asciiBuffer = Encoding.ASCII.GetString(m_buffer, 0, m_bytesInBuffer);

                //For "<pipe1><message>kasjdlfj kljasdkljf </message></pipe1>"
                ////this should return the whole message
                m_match = Regex.Match(m_asciiBuffer, @"<([^>]*)>.*?</(\1)>");

                if (m_match.Success)
                {
                    if (bMarkAsProcessed) m_bufferOffset += m_match.Index + m_match.Length;
                    m_lastXMLItem = m_match.Value;
                    return m_match.Value;
                }
            }
            return "";
        }
        //If message "<pipe1><message>kasjdlfj kljasdkljf </message></pipe1>" is received
        ////this method should return "pipe1", not marking those bytes as processed
        public string peekNextXMLTag()
        {
            if (m_bytesInBuffer > 0)
            {
                discardProcessedData();
                m_asciiBuffer = Encoding.ASCII.GetString(m_buffer, 0, m_bytesInBuffer);

                m_match = Regex.Match(m_asciiBuffer, @"<([^\s>]*)[^>]*>");

                if (m_match.Success)
                {
                    return m_match.Groups[1].Value;
                }
            }
            return "";
        }
        //returns the next complete xml element (NO ATTRIBUTES!!) in the stream
        //empty string if there was none
        public string processNextXMLTag()
        {
            if (m_bytesInBuffer > 0)
            {
                discardProcessedData();
                m_asciiBuffer = Encoding.ASCII.GetString(m_buffer, 0, m_bytesInBuffer);

                //For "<pipe1><message>kasjdlfj kljasdkljf </message></pipe1>"
                ////this should return the whole message
                m_match = Regex.Match(m_asciiBuffer, @"<([^>]*)>");

                if (m_match.Success)
                {
                    m_bufferOffset += m_match.Index + m_match.Length;
                    m_lastXMLItem = m_match.Value;
                    return m_match.Value;
                }
                else logMessage("WARNING: Couldn't match input in processNextXMLTag(): " + m_asciiBuffer);
            }
            return "";
        }
        //instead of parsing pending info in the buffer, it parses m_lastXMLItem
        //, which is set after a call to processNextXMLTag()
        public string getLastXMLItemContent()
        {
            if (m_lastXMLItem != "")
            {
                m_match = Regex.Match(m_lastXMLItem, @"<([^>]*?)>(.*?)</(\1)>"); //@"<[^>]*?>([^<]*?)<");
                if (m_match.Success)
                {
                    m_lastXMLItem = m_match.Groups[2].Value;
                    return m_match.Groups[2].Value;
                }
            }
            return "";
        }
        public string getLastXMLItemTagAndAttributes()
        {
            if (m_lastXMLItem != "")
            {
                m_match = Regex.Match(m_lastXMLItem, @"<([^\s]*)[^\>]*>");//@"<([^>]*)>[^<]*?<");
                if (m_match.Success)
                    return m_match.Groups[1].Value;
            }
            return "";
        }
        public string getLastXMLItemTag()
        {
            if (m_lastXMLItem != "")
            {
                m_match = Regex.Match(m_lastXMLItem, @"<([^\s>]*)[^>]*>");
                if (m_match.Success)
                    return m_match.Groups[1].Value;
            }
            return "";
        }
    }
}
