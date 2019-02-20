using System.Text;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Security.Cryptography;
using System.Xml;
using Herd.Files;

namespace Badger.Data
{
    public static class Utility
    {
        public static byte[] GetHash(string inputString)
        {
            HashAlgorithm algorithm = MD5.Create();  //or use SHA256.Create();
            return algorithm.ComputeHash(Encoding.UTF8.GetBytes(inputString));
        }

        public static string GetHashString(string inputString)
        {
            StringBuilder sb = new StringBuilder();
            foreach (byte b in GetHash(inputString))
                sb.Append(b.ToString("X2"));

            return sb.ToString();
        }


        //this function is called from several tasks and needs to be synchronized
        [MethodImpl(MethodImplOptions.Synchronized)]
        public static RunTimeRequirements GetRunTimeRequirements(string exe, string args)
        {
            object o = new object();
            Monitor.Enter(o);
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = exe,
                    Arguments = args + " -requirements",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };

            string processOutput = "";
            process.Start();
            while (!process.StandardOutput.EndOfStream)
            {
                processOutput += process.StandardOutput.ReadLine();
            }

            //Parse the output
            RunTimeRequirements runTimeRequirements = null;
            int startPos = processOutput.IndexOf("<" + Herd.Network.XmlTags.Requirements + ">");
            int endPos = processOutput.IndexOf("</" + Herd.Network.XmlTags.Requirements + ">");
            if (startPos >= 0 && endPos > 0)
            {
                string xml = processOutput.Substring(startPos, endPos - startPos + ("</" + Herd.Network.XmlTags.Requirements + ">").Length);

                XmlDocument doc = new XmlDocument();
                doc.LoadXml(xml);

                runTimeRequirements = new RunTimeRequirements(doc.FirstChild);

            }
            Monitor.Exit(o);
            return runTimeRequirements;
        }

    }
}
