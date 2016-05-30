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

namespace Herd
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

    public enum FileType { EXE, INPUT, OUTPUT };
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
        public TcpClient getTcpClient() { return m_tcpClient; }
        public NetworkStream getNetworkStream() { return m_netStream; }

        //used for reading
        protected int m_nextFileSize;
        protected string m_nextFileName;

        protected static string m_tempDir;
        protected CJob m_job;
        protected int m_numInputFilesRead;
        protected int m_numOutputFilesRead;
        protected int m_numTasksRead;

        public delegate void LogMessageHandler(string logMessage);
        protected LogMessageHandler m_logMessageHandler;

        public CJobDispatcher()
        {
            m_job = new CJob();
            m_xmlStream = new XMLStream();
            m_nextFileSize = 0;
            m_numInputFilesRead = 0;
            m_numOutputFilesRead = 0;
            m_numTasksRead = 0;
            m_tempDir = "";
            m_logMessageHandler = null;
        }

        public void setTCPClient(TcpClient client) { m_tcpClient = client; m_netStream = client.GetStream(); }
        public void setDirPath(string dirPath) { m_tempDir = dirPath; }
        public void setLogMessageHandler(LogMessageHandler logMessageHandler) { m_logMessageHandler = logMessageHandler; }
        protected void logMessage(string message) { if (m_logMessageHandler != null) m_logMessageHandler(message); }
        public void writeMessage(string message, bool addDefaultMessageType = false)
        {
            m_xmlStream.writeMessage(m_netStream, message, addDefaultMessageType);
        }
        public void read()
        {
            ////checkConnection(m_tcpClient);
            m_xmlStream.readFromNetworkStream(m_tcpClient, m_netStream);
        }
        public string processNextXMLItem()
        {
            return m_xmlStream.processNextXMLItem();
        }
        public string processNextXMLTag()
        {
            return m_xmlStream.processNextXMLTag();
        }
        public string getLastXMLItemContent()
        {
            return m_xmlStream.getLastXMLItemContent();
        }

        protected void SendExeFiles(bool sendContent) { if (m_job.exeFile != "") SendFile(m_job.exeFile, FileType.EXE, sendContent, false); }
        protected void SendInputFiles(bool sendContent)
        {
            foreach (string file in m_job.inputFiles)
            {
                SendFile(file, FileType.INPUT, sendContent, false);
            }
        }
        protected void SendOutputFiles(bool sendContent)
        {
            foreach (string file in m_job.outputFiles)
            {
                SendFile(file, FileType.OUTPUT, sendContent, true);
            }
        }
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
            header += args;
            byte[] headerbytes = Encoding.ASCII.GetBytes(header);
            //checkConnection(m_tcpClient);
            m_netStream.Write(headerbytes, 0, headerbytes.Length);
        }
        protected void SendJobFooter()
        {
            string footer = "</Job>";
            byte[] footerbytes = Encoding.ASCII.GetBytes(footer);
            //checkConnection(m_tcpClient);
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
                logMessage("Sending file " + fileName);

                if (fromCachedDir)
                    fileName = getCachedFilename(fileName);

                fileStream = File.OpenRead(fileName);
                fileSize = fileStream.Length;
                header += " Size=\"" + fileSize + "\">";

                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                //checkConnection(m_tcpClient);
                m_netStream.Write(headerBytes, 0, headerBytes.Length);

                //send the content if it has to be sent
                if (sendContent)
                {
                    long readBytes = 0;
                    int lastReadBytes;
                    byte[] buffer = new byte[m_xmlStream.getBufferSize()];
                    while (readBytes < fileSize)
                    {
                        lastReadBytes = fileStream.Read(buffer, 0, m_xmlStream.getBufferSize());
                        readBytes += lastReadBytes;
                        //checkConnection(m_tcpClient);
                        try
                        {
                            m_netStream.Write(buffer, 0, (int)lastReadBytes);
                        }
                        catch(Exception ex)
                        {
                            Console.WriteLine(ex.Message + ex.StackTrace);
                        }
                    }
                    fileStream.Close();
                }

                //Send the footer: </Exe>, </Input> or </Output>
                byte[] footerBytes = Encoding.ASCII.GetBytes(footer);
                //checkConnection(m_tcpClient);
                m_netStream.Write(footerBytes, 0, footerBytes.Length);

                m_netStream.Flush();
            }
            else
            {
                header += "/>";
                //send the header
                headerBytes = Encoding.ASCII.GetBytes(header);
                //checkConnection(m_tcpClient);
                m_netStream.Write(headerBytes, 0, headerBytes.Length);


                m_netStream.Flush();
            }

        }

        public void ReadFromStream()
        {
            //checkConnection(m_tcpClient);
            m_xmlStream.readFromNetworkStream(m_tcpClient, m_netStream);
        }
        public void ReceiveJobHeader()
        {
            Match match;
            string header;

            do
            {
                ReadFromStream();
                header = m_xmlStream.processNextXMLTag();//Encoding.ASCII.GetString(m_buffer);

                match = Regex.Match(header, "<Job Name=\"([^\"]*)\" NumTasks=\"([^\"]*)\" NumInputFiles=\"([^\"]*)\" NumOutputFiles=\"([^\"]*)\">");
            }
            while (!match.Success);

            m_job.name = match.Groups[1].Value;
            m_numTasksRead = Int32.Parse(match.Groups[2].Value);
            m_numInputFilesRead = Int32.Parse(match.Groups[3].Value);
            m_numOutputFilesRead = Int32.Parse(match.Groups[4].Value);

            //read arguments
            for (int i = 0; i < m_numTasksRead; i++)
            {
                ReadFromStream();
                header = m_xmlStream.processNextXMLItem();//Encoding.ASCII.GetString(m_buffer);

                match = Regex.Match(header, "<Arg>([^<]*)</Arg>");

                m_job.comLineArgs.Add(match.Groups[1].Value);
                //m_bufferOffset += match.Index + match.Length;
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
                header = m_xmlStream.processNextXMLTag();//Encoding.ASCII.GetString(m_buffer);
                match = Regex.Match(header, "</Job>");
            }
            while (!match.Success);
            // m_bufferOffset += match.Index + match.Length;
        }
        protected void ReceiveExeFiles(bool receiveContent,bool inCachedDir) { ReceiveFile(FileType.EXE, receiveContent, inCachedDir); }
        protected void ReceiveInputFiles(bool receiveContent, bool inCachedDir)
        {
            for (int i = 0; i < m_numInputFilesRead; i++)
            {
                ReceiveFile(FileType.INPUT, receiveContent, inCachedDir);
            }
        }
        protected void ReceiveOutputFiles(bool receiveContent, bool inCachedDir)
        {
            for (int i = 0; i < m_numOutputFilesRead; i++)
                ReceiveFile(FileType.OUTPUT, receiveContent, inCachedDir);
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

            do
            {
                ReadFromStream();
                string header = m_xmlStream.processNextXMLTag();// Encoding.ASCII.GetString(m_buffer);

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

            do
            {
                ReadFromStream();
                string header = m_xmlStream.processNextXMLTag();// Encoding.ASCII.GetString(m_buffer);

                if (type == FileType.EXE) match = Regex.Match(header, "</Exe>");
                else if (type == FileType.INPUT) match = Regex.Match(header, "</Input>");
                else match = Regex.Match(header, "</Output>");
            }
            while (!match.Success);

            //m_bufferOffset += match.Length + match.Index;
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
        private int m_maxChunkSize = 1024;
        private string m_asciiBuffer;
        private string m_lastXMLItem;
        public const string m_defaultMessageType = "Internal";
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
        public void writeMessage(NamedPipeServerStream stream, string message, bool addDefaultMessageType = false)
        {
            byte[] msg;
            if (addDefaultMessageType)
                msg = Encoding.ASCII.GetBytes("<" + m_defaultMessageType + ">" + message + "</" + m_defaultMessageType + ">");
            else
                msg = Encoding.ASCII.GetBytes(message);
            stream.Write(msg, 0, msg.Length);
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
        public void readFromNamedPipeStream(NamedPipeServerStream stream)
        {
            discardProcessedData();
            //read if there's something to read and if we have available storage
            if (m_bytesInBuffer < m_maxChunkSize)
                m_bytesInBuffer += stream.Read(m_buffer, m_bytesInBuffer, m_maxChunkSize - m_bytesInBuffer);
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
            return processNextXMLTag(false);
        }
        //returns the next complete xml element (NO ATTRIBUTES!!) in the stream
        //empty string if there was none
        public string processNextXMLTag(bool bMarkAsProcessed=true)
        {
            if (m_bytesInBuffer > 0)
            {
                m_asciiBuffer = Encoding.ASCII.GetString(m_buffer, 0, m_bytesInBuffer);

                //For "<pipe1><message>kasjdlfj kljasdkljf </message></pipe1>"
                ////this should return the whole message
                m_match = Regex.Match(m_asciiBuffer, @"<([^>]*)>");

                if (m_match.Success)
                {
                    if (bMarkAsProcessed) m_bufferOffset += m_match.Index + m_match.Length;
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
                m_match = Regex.Match(m_lastXMLItem, @"<[^>]*>([^<]*?)<");
                if (m_match.Success)
                    return m_match.Groups[1].Value;
            }
            return "";
        }
        public string getLastXMLItemTag()
        {
            if (m_lastXMLItem != "")
            {
                m_match = Regex.Match(m_lastXMLItem, @"<([^>]*)>[^<]*?<");
                if (m_match.Success)
                    return m_match.Groups[1].Value;
            }
            return "";
        }
    }
}
