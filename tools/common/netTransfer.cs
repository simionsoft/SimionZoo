using System.Text.RegularExpressions;
using System.IO;
using System.Net.Sockets;
using System.Net;
using System;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;

namespace NetJobTransfer
{
    class CJob
    {
        public string name;
        public List<string> inputFiles;
        public List<string> outputFiles;
        public string exeFile;
        
        public CJob()
        {
            name = "";
            exeFile = "";
            inputFiles= new List<string>();
            outputFiles= new List<string>();
        }
    }
    enum AgentState { BUSY, AVAILABLE, DISCOVERED };
    enum FileType { EXE, INPUT, OUTPUT};
    class CJobDispatcher
    {
        public const int m_discoveryPortShepherd = 2332;
        public const int m_discoveryPortHerd = 2333;
        public const int m_comPortShepherd = 2334;
        public const int m_comPortHerd = 2335;
        public const string m_discoveryMessage = "Slaves, show yourselves!";
        public const string m_discoveryAnswer = "At your command, my Master";
        static int m_maxChunkSize = 1024;

        private byte[] m_buffer;

        NetworkStream m_netStream;

        private int m_bytesInBuffer;
        private int m_bufferOffset;

        //used for reading
        private int m_nextFileSize;
        private string m_nextFileName;

        private string m_tempDir;
        private CJob m_job;
        private int m_numInputFilesRead;
        private int m_numOutputFilesRead;

        public CJobDispatcher()
        {
            m_job = new CJob();
            m_buffer = new byte[m_maxChunkSize];
            m_bytesInBuffer= 0;
            m_bufferOffset= 0;
            m_nextFileSize = 0;
            m_tempDir = "c:\\temp";
        }

        public void SendJobQuery(NetworkStream netStream,CJob job)
        {
            m_netStream = netStream;
            m_job = job;
            SendJobHeader();
            SendExeFiles(true);
            SendInputFiles(true);
            SendOutputFiles(false);
            SendJobFooter();
        }
        public void SendJobResult(NetworkStream netStream, CJob job)
        {
            m_netStream = netStream;
            m_job = job;
            SendJobHeader();
            SendExeFiles(false);
            SendInputFiles(false);
            SendOutputFiles(true);
            SendJobFooter();
        }

        private void SendExeFiles(bool sendContent) { if (m_job.exeFile!="") SendFile(m_job.exeFile,FileType.EXE, sendContent); }
        private void SendInputFiles(bool sendContent) { foreach (string file in m_job.inputFiles) SendFile(file, FileType.INPUT, sendContent); }
        private void SendOutputFiles(bool sendContent) { foreach (string file in m_job.outputFiles) SendFile(file, FileType.OUTPUT, sendContent); }
        private void SendJobHeader()
        {
            string header = "<Job Name=\"" + m_job.name + "\" NumInputFiles=\"" + m_job.inputFiles.Count + "\" NumOutputFiles=\""
                + m_job.outputFiles.Count + "\">";
            byte[] headerbytes= Encoding.ASCII.GetBytes(header);
            m_netStream.Write(headerbytes,0,headerbytes.Length);
        }
        private void SendJobFooter()
        {
            string footer = "</Job>";
            byte[] footerbytes = Encoding.ASCII.GetBytes(footer);
            m_netStream.Write(footerbytes, 0, footerbytes.Length);
        }
        private void SendFile(string fileName, FileType type, bool sendContent)
        {
            string header;
            byte[] headerBytes;
            string footer = "";

            if (type == FileType.EXE)
            {
                header = "<Exe Name=\"" + fileName + "\"";
                footer = "</Exe>";
            }
            else if (type == FileType.INPUT)
            {
                header = "<Input Name=\"" + fileName + "\"";
                footer = "</Input>";
            }
            else //output
            {
                header = "<Output Name=\"" + fileName + "\"";
            }

            if (sendContent)
            {
                FileStream fileStream;
                long fileSize = 0;
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

                //Send the footer: </Exe>, </Input> or </Output>
                byte[] footerBytes = Encoding.ASCII.GetBytes(footer);
                m_netStream.Write(footerBytes, 0, footerBytes.Length);

                m_netStream.Flush();
            }

        }

        public bool ReceiveJobQuery(NetworkStream netStream)
        {
            m_netStream = netStream;
            m_bufferOffset= 0;
            m_bytesInBuffer= 0;

            ReceiveJobHeader();
            ReceiveExeFiles(true);
            ReceiveInputFiles(true);
            ReceiveOutputFiles(false);
            ReceiveJobFooter();

            return true;//if job query properly received. For now, we will assume it
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
        private void ReadFromStream()
        {
            int bytesLeftToProcess= m_bytesInBuffer-m_bufferOffset;
            //something left to process in the buffer?
            if (m_bufferOffset!=0)
            {
                Buffer.BlockCopy(m_buffer,m_bufferOffset,m_buffer,0,m_bytesInBuffer-m_bufferOffset);
                m_bytesInBuffer= m_bytesInBuffer-m_bufferOffset;
                m_bufferOffset= 0;
            }
            if (m_netStream.DataAvailable && m_bytesInBuffer<m_maxChunkSize)
                m_bytesInBuffer+= m_netStream.Read(m_buffer,m_bytesInBuffer, m_maxChunkSize-m_bytesInBuffer);
        }
        private void ReceiveJobHeader()
        {
            Match match;
            
            do
            {
                ReadFromStream();
                string header = Encoding.ASCII.GetString(m_buffer);
                match = Regex.Match(header, "<Job Name=\"([^\"]*)\" NumInputFiles=\"([^\"]*)\" NumOutputFiles=\"([^\"]*)\">");
            }
            while (!match.Success);

            m_job.name = match.Groups[1].Value;
            m_numInputFilesRead = Int32.Parse(match.Groups[2].Value);
            m_numOutputFilesRead = Int32.Parse(match.Groups[3].Value);
            m_bufferOffset+= match.Index + match.Length;
            ReadFromStream(); //we shift the buffer to the left skipping the processed header
        }
        private void ReceiveJobFooter()
        {
            Match match;

            string header = Encoding.ASCII.GetString(m_buffer);
            do
            {
                ReadFromStream();
                match = Regex.Match(header, "</Job>");
            }
            while (!match.Success);
        }
        private void ReceiveExeFiles(bool receiveContent) {ReceiveFile(FileType.EXE,receiveContent);}
        private void ReceiveInputFiles(bool receiveContent)
        {
            for (int i= 0; i<m_numInputFilesRead; i++)
                ReceiveFile(FileType.INPUT,receiveContent);
        }
       private void ReceiveOutputFiles(bool receiveContent)
        {
            for (int i= 0; i<m_numOutputFilesRead; i++)
                ReceiveFile(FileType.OUTPUT,receiveContent);
        }

        private void ReceiveFile(FileType type,bool receiveContent)
        {
                ReceiveFileHeader(type,receiveContent);
                if (receiveContent) ReceiveFileData();
                ReceiveFileFooter(type);
        }
        public void ReceiveFileHeader(FileType type,bool receiveContent)
        {
            Match match;

            
            do
            {
                ReadFromStream();
                string header = Encoding.ASCII.GetString(m_buffer);

                if (receiveContent)
                    match = Regex.Match(header, "<(Exe|Input|Output) Name=\"([^\"]*)\" Size=\"([^\"]*)\">");
                else match = Regex.Match(header, "<(Exe|Input|Output) Name=\"([^\"]*)\">");
            }
            while (!match.Success);


            if ((match.Groups[1].Value == "Exe" && type != FileType.EXE)
                || (match.Groups[1].Value == "Input" && type != FileType.INPUT)
                || (match.Groups[1].Value == "Output" && type != FileType.OUTPUT))
                Debug.Assert(false,"The type of the file received missmatches the expected file type");


            m_nextFileName = match.Groups[2].Value;

            if (receiveContent) m_nextFileSize = Int32.Parse(match.Groups[3].Value);
            else m_nextFileSize = 0;

            m_bufferOffset += match.Index+match.Length;
        }
        public void ReceiveFileFooter(FileType type)
        {
            Match match;

            do
            {
                ReadFromStream();
                string header = Encoding.ASCII.GetString(m_buffer);

                if (type == FileType.EXE) match = Regex.Match(header, "</Exe>");
                else if (type == FileType.INPUT) match = Regex.Match(header, "</Input>");
                else match= Regex.Match(header,"</Output>");
            }
            while (!match.Success);

            m_bufferOffset += match.Length + match.Index;
        }
        private int SaveBufferToFile(FileStream outputFile,int bytesLeft)
        {
            int bytesToWrite = Math.Min(bytesLeft, m_bytesInBuffer - m_bufferOffset);

            outputFile.Write(m_buffer, m_bufferOffset, bytesToWrite);
          
            m_bufferOffset+= bytesToWrite;
            return bytesToWrite;
        }
        private string getCachedFilename(string originalFilename)
        {
            string outputFilename = m_tempDir.TrimEnd('/', '\\') + "\\" + originalFilename.TrimStart('.', '/', '\\');
            return outputFilename;
        }
        public void ReceiveFileData()
        {
            int bytesLeft = m_nextFileSize;


            string outputFilename = getCachedFilename(m_nextFileName);
            string outputDir = Path.GetDirectoryName(outputFilename);
            System.IO.Directory.CreateDirectory(outputDir);
            FileStream outputFile = File.Open(outputFilename, FileMode.Create);

            do
            {
                ReadFromStream();
                bytesLeft -= SaveBufferToFile(outputFile,bytesLeft);
            } while (bytesLeft > 0);
            outputFile.Close();

        }
        public void RunJob()
        {

        }

    }
}