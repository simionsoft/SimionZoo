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
using Herd;
using AppXML.Data;

namespace Herd
{
    public class CTask
    {
        public string exe;
        public string arguments;
        public string pipe;
        public string name;
        public CTask()
        {
            name = "";
            exe = "";
            arguments = "";
            pipe = "";
        }
    }
    public class CJob
    {
        public string name;
        public List<CTask> tasks;
        public List<string> inputFiles;
        public List<string> outputFiles;
        //public List<string> pipes;
        //public string exeFile;
        //public List<string> comLineArgs;

        public CJob()
        {
            //comLineArgs = "";
            name = "";
            tasks = new List<CTask>();
            inputFiles = new List<string>();
            outputFiles = new List<string>();
        }
    }



    public class CJobDispatcher
    {
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
        public const string m_cleanCacheMessage = "Clean the cache";
        public const string m_updateMessage = "Update yourself";

        protected NetworkStream m_netStream;
        protected TcpClient m_tcpClient;

        //used for reading
        protected int m_nextFileSize;
        protected string m_nextFileName;

        protected static string m_tempDir;
        protected CJob m_job;

        protected Utility.LogFunction m_logMessageHandler;

        
        public enum FileType { INPUT, OUTPUT };
        public string getFileTypeXMLTag(FileType type)
        {
            if (type == FileType.INPUT)
                return "Input";
            else if (type == FileType.OUTPUT) return "Output";
            logMessage("Can't determine the XML tag of the unkwown file type");
            return "UnkwnownType";
        }
        public CJobDispatcher()
        {
            m_job = new CJob();
            m_xmlStream = new XMLStream();
            m_nextFileSize = 0;
            m_tempDir = "";
            m_logMessageHandler = null;
        }
        
        private bool m_bEnqueueAsyncWrites = false;
        public void startEnqueueingAsyncWriteOps(){m_bEnqueueAsyncWrites= true;}

        private List<Task> m_pendingAsyncWrites = new List<Task>();

        public void setTCPClient(TcpClient client) { m_tcpClient = client; m_netStream = client.GetStream(); }
        public void setDirPath(string dirPath) { m_tempDir = dirPath; }
        public void setLogMessageHandler(Utility.LogFunction logMessageHandler)
        { 
            m_logMessageHandler = logMessageHandler;
            m_xmlStream.setLogMessageHandler(logMessageHandler);
        }
        protected void logMessage(string message) { if (m_logMessageHandler != null) m_logMessageHandler(message); }
        public void writeMessage(string message, bool addDefaultMessageType = false)
        {
            m_xmlStream.writeMessage(m_netStream, message, addDefaultMessageType);
        }
        public async Task writeMessageAsync(string message, CancellationToken cancelToken, bool addDefaultMessageType= false)
        {
            await m_xmlStream.writeMessageAsync(m_netStream, message, cancelToken, addDefaultMessageType);
        }
        public void read()
        {
            ////checkConnection(m_tcpClient);
            m_xmlStream.readFromNetworkStream(m_tcpClient, m_netStream);
        }
        public async Task<int> readAsync(CancellationToken cancelToken)
        {
            int numBytesRead= 0;
            try { numBytesRead = await m_xmlStream.readFromNetworkStreamAsync(m_tcpClient, m_netStream, cancelToken); }
            catch { logMessage("async read operation cancelled"); }
            return numBytesRead;
        }
        public void writeAsync(byte[]buffer, int offset, int length, CancellationToken cancelToken)
        {
            if (!m_bEnqueueAsyncWrites)
            {
                try { m_netStream.WriteAsync(buffer, offset, length, cancelToken); }
                catch (OperationCanceledException) { logMessage("async write operation cancelled"); }
            }
            else
            {
                try { m_pendingAsyncWrites.Add(m_netStream.WriteAsync(buffer, offset, length)); }
                catch (OperationCanceledException) { logMessage("async write operation cancelled"); }
            }
        }
        public void waitAsyncWriteOpsToFinish()
        {
            Task.WhenAll(m_pendingAsyncWrites).Wait();
            m_pendingAsyncWrites.Clear();
            m_bEnqueueAsyncWrites= false;
        }

        //protected void SendExeFiles(bool sendContent) { if (m_job.exeFile != "") SendFile(m_job.exeFile, FileType.EXE, sendContent, false); }
        protected void SendInputFiles(bool sendContent,CancellationToken cancelToken)
        {
            foreach (string file in m_job.inputFiles)
            {
                SendFile(file, FileType.INPUT, sendContent, false,cancelToken);
            }
        }
        protected void SendOutputFiles(bool sendContent,CancellationToken cancelToken)
        {
            foreach (string file in m_job.outputFiles)
            {
                //some output files may not be ther if a task failed
                //we still want to try sending the rest of files
                if (!sendContent || File.Exists(getCachedFilename(file)))
                    SendFile(file, FileType.OUTPUT, sendContent,true, cancelToken);
                else
                    m_job.outputFiles.Remove(file);
            }
        }
        protected void SendTasks(CancellationToken cancelToken)
        {
            foreach (CTask task in m_job.tasks)
                SendTask(task,cancelToken);
        }
        protected void SendTask(CTask task,CancellationToken cancelToken)
        {
            string taskXML = "<Task Name=\"" + task.name + "\"";
            taskXML += " Exe=\"" + task.exe + "\"";
            taskXML += " Arguments=\"" + task.arguments + "\"";
            taskXML += " Pipe=\"" + task.pipe + "\"";
            taskXML += "/>";
            byte[] bytes= Encoding.ASCII.GetBytes(taskXML);

            writeAsync(bytes, 0, bytes.Length,cancelToken);
        }
        public void ReceiveTask()
        {
            CTask task = new CTask();
            Match match;
            match= ReadUntilMatch("<Task Name=\"([^\"]*)\" Exe=\"([^\"]*)\" Arguments=\"([^\"]*)\" Pipe=\"([^\"]*)\"/>");
            task.name = match.Groups[1].Value;
            task.exe = match.Groups[2].Value;
            task.arguments = match.Groups[3].Value;
            task.pipe = match.Groups[4].Value;
            m_job.tasks.Add(task);
        }
 
        protected void SendJobHeader(CancellationToken cancelToken)
        {
            string header = "<Job Name=\"" + m_job.name + "\">";
            byte[] headerbytes = Encoding.ASCII.GetBytes(header);
            //checkConnection(m_tcpClient);
            writeAsync(headerbytes, 0, headerbytes.Length,cancelToken);
        }
        protected void SendJobFooter(CancellationToken cancelToken)
        {
            string footer = "</Job>";
            byte[] footerbytes = Encoding.ASCII.GetBytes(footer);
            //checkConnection(m_tcpClient);
            writeAsync(footerbytes, 0, footerbytes.Length,cancelToken);
        }
        protected void SendFile(string fileName, FileType type, bool sendContent, bool fromCachedDir,CancellationToken cancelToken)
        {
            string fileTypeXMLTag;
            string header;
            byte[] headerBytes;
            string footer = "";

            fileTypeXMLTag = getFileTypeXMLTag(type);
            header = "<" + fileTypeXMLTag + " Name=\"" + fileName + "\"";
            if (sendContent) footer = "</" + fileTypeXMLTag + ">";

            if (sendContent)
            {
                FileStream fileStream= null;
                long fileSize = 0;
                logMessage("Sending file " + fileName);

                if (fromCachedDir)
                    fileName = getCachedFilename(fileName);

                try { fileStream = File.OpenRead(fileName); }
                catch { return; }
                fileSize = fileStream.Length;
                header += " Size=\"" + fileSize + "\">";

                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                writeAsync(headerBytes, 0, headerBytes.Length,cancelToken);


                long readBytes = 0;
                int lastReadBytes;
                
                while (readBytes < fileSize)
                {
                    byte[] buffer = new byte[m_xmlStream.getBufferSize()];
                    lastReadBytes = fileStream.Read(buffer, 0, m_xmlStream.getBufferSize());
                    readBytes += lastReadBytes;

                    try
                    {
                        writeAsync(buffer, 0, (int)lastReadBytes,cancelToken);
                    }
                    catch(Exception ex)
                    {
                        Console.WriteLine(ex.Message + ex.StackTrace);
                    }
                }
                fileStream.Close();

                //Send the footer: </Exe>, </Input> or </Output>
                byte[] footerBytes = Encoding.ASCII.GetBytes(footer);
                //checkConnection(m_tcpClient);
                writeAsync(footerBytes, 0, footerBytes.Length,cancelToken);
                m_netStream.Flush();
            }
            else
            {
                header += "/>";
                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                //checkConnection(m_tcpClient);
                writeAsync(headerBytes, 0, headerBytes.Length,cancelToken);

                m_netStream.Flush();
            }
            //return true;
        }

        public void ReadFromStream()
        {
            m_xmlStream.readFromNetworkStream(m_tcpClient, m_netStream);
        }
        public Match ReadUntilMatch(string pattern)
        {
            Match match;
            string header;

            do
            {
                ReadFromStream();
                header = m_xmlStream.processNextXMLTag();//Encoding.ASCII.GetString(m_buffer);

                match = Regex.Match(header, pattern);
            }
            while (!match.Success);

            return match;
        }
        public void ReceiveJobHeader()
        {
            Match match;
            match= ReadUntilMatch("<Job Name=\"([^\"]*)\">");

            m_job.name = match.Groups[1].Value;

            ReadFromStream(); //we shift the buffer to the left skipping the processed header
        }
   
        protected void ReceiveJobFooter()
        {
            Match match;

            string header;
            do
            {
                ReadFromStream();
                header = m_xmlStream.processNextXMLTag();//Encoding.ASCII.GetString(m_buffer);
                match = Regex.Match(header, "</Job>");
            }
            while (!match.Success);
            // m_bufferOffset += match.Index + match.Length;
        }

        protected void ReceiveFile(FileType type, bool receiveContent, bool inCachedDir)
        {
            ReceiveFileHeader(type, receiveContent,inCachedDir);
            if (receiveContent)
            {
                logMessage("Receiving file: " + m_nextFileName);
                ReceiveFileData(inCachedDir);
                ReceiveFileFooter(type);
            }
        }
        protected void ReceiveFileHeader(FileType type, bool receiveContent, bool inCachedDir)
        {
            Match match;

            if (receiveContent)
                match = ReadUntilMatch("<(Input|Output) Name=\"([^\"]*)\" Size=\"([^\"]*)\">");
            else match = ReadUntilMatch("<(Input|Output) Name=\"([^\"]*)\"/>");

            if ( (match.Groups[1].Value == "Input" && type != FileType.INPUT)
                || (match.Groups[1].Value == "Output" && type != FileType.OUTPUT))
                Debug.Assert(false, "The type of the file received missmatches the expected file type");
           
            if (type == FileType.INPUT) m_job.inputFiles.Add(match.Groups[2].Value);
            else m_job.outputFiles.Add(match.Groups[2].Value);

            m_nextFileName = match.Groups[2].Value;

            if (receiveContent) m_nextFileSize = Int32.Parse(match.Groups[3].Value);
            else m_nextFileSize = 0;

            //We create the necessary directories for the file, be it an exe, an input or an output file
            string outputFilename;
            if (inCachedDir)
                outputFilename = getCachedFilename(m_nextFileName);
            else outputFilename = m_nextFileName;
            string outputDir = Path.GetDirectoryName(outputFilename);
            System.IO.Directory.CreateDirectory(outputDir);

            // m_bufferOffset += match.Index + match.Length;
        }
        protected void ReceiveFileFooter(FileType type)
        {
            Match match;
            match = ReadUntilMatch("</(Input|Output)>");
        }
        protected int SaveBufferToFile(FileStream outputFile, int bytesLeft)
        {
            int bytesToWrite = Math.Min(bytesLeft, m_xmlStream.getBytesInBuffer() - m_xmlStream.getBufferOffset());

            outputFile.Write(m_xmlStream.getBuffer(), m_xmlStream.getBufferOffset(), bytesToWrite);

            m_xmlStream.addProcessedBytes(bytesToWrite);// m_bufferOffset += bytesToWrite;
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

        protected Utility.LogFunction m_logMessageHandler;
        public void setLogMessageHandler(Utility.LogFunction logMessageHandler) { m_logMessageHandler = logMessageHandler; }
        protected void logMessage(string message) { if (m_logMessageHandler != null) m_logMessageHandler(message); }

        public XMLStream()
        {
            m_buffer = new byte[m_maxChunkSize];
        }
        public int getBufferSize() { return m_maxChunkSize; }
        public void resizeBuffer(int newSize)
        {
            m_maxChunkSize = newSize;
            m_buffer= new byte[m_maxChunkSize];
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
        public void writeMessage(NetworkStream stream, string message, bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            stream.Write(msg, 0, msg.Length);
        }
        public async Task writeMessageAsync(NetworkStream stream, string message, CancellationToken cancelToken, bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            try
            {
                await stream.WriteAsync(msg, 0, msg.Length,cancelToken);
            }
            catch(OperationCanceledException)
            {
                logMessage("Write operation cancelled");
            }
            catch(Exception ex)
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
        public async Task writeMessageAsync(NamedPipeServerStream stream, string message, CancellationToken cancelToken,bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            try
            {
                await stream.WriteAsync(msg, 0, msg.Length,cancelToken);
            }
            catch
            {
                logMessage("write operation cancelled");
            }
        }
        public void readFromNetworkStream(TcpClient client, NetworkStream stream)
        {
            discardProcessedData();
            //read if there's something to read and if we have available storage
            do
            {
                //CJobDispatcher.checkConnection(client);
                if (stream.DataAvailable && m_bytesInBuffer < m_maxChunkSize)
                {
                    m_bytesInBuffer += stream.Read(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer);
                }
                if (m_bytesInBuffer == 0) Thread.Sleep(200);
            } while (m_bytesInBuffer == 0);
        }
        public async Task<int> readFromNetworkStreamAsync(TcpClient client, NetworkStream stream,CancellationToken cancelToken)
        {
            int numBytesRead= 0;
            discardProcessedData();
            //read if there's something to read and if we have available storage
            try { numBytesRead = await stream.ReadAsync(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer, cancelToken); }
            catch (OperationCanceledException) { logMessage("async read from network stream cancelled"); }
            m_bytesInBuffer += numBytesRead;
            return numBytesRead;
        }
        public void readFromNamedPipeStream(NamedPipeServerStream stream)
        {
            discardProcessedData();
            //read if there's something to read and if we have available storage
            if (m_bytesInBuffer < m_maxChunkSize)
                m_bytesInBuffer += stream.Read(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer);
        }
        public async Task< int> readFromNamedPipeStreamAsync(NamedPipeServerStream stream,CancellationToken cancelToken)
        {
            int numBytesRead= 0;
            discardProcessedData();
            //read if there's something to read and if we have available storage
            if (m_bytesInBuffer < m_maxChunkSize)
            {
                try
                {
                    numBytesRead = await stream.ReadAsync(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer, cancelToken);
                }
                catch(OperationCanceledException)
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
        public string processNextXMLItem(bool bMarkAsProcessed=true)
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
        public string peekNextXMLTag()
        {
            if (m_bytesInBuffer > 0)
            {
                discardProcessedData();
                m_asciiBuffer = Encoding.ASCII.GetString(m_buffer, 0, m_bytesInBuffer);

                //For "<pipe1><message>kasjdlfj kljasdkljf </message></pipe1>"
                ////this should return the whole message
                m_match = Regex.Match(m_asciiBuffer, @"<([^\s>]*)");

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
            }
            return "";
        }
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
                m_match = Regex.Match(m_lastXMLItem, @"<([^\s\>]*)");
                if (m_match.Success)
                    return m_match.Groups[1].Value;
            }
            return "";
        }
    }
}
