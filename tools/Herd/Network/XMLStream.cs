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

using System;
using System.IO.Pipes;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace Herd.Network
{
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
