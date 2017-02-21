using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Herd;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Threading;

namespace Badger.Data
{
    public static class Utility
    {
        //this function is called from several tasks and needs to be synchronized
        [MethodImpl(MethodImplOptions.Synchronized)]
        public static void getInputsAndOutputs(string exe,string args, ref CJob job)
        {
            object o = new object();
            Monitor.Enter(o);
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = exe,
                    Arguments = args + " -printIOfiles",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };

            string processOutput= "";
            process.Start();
            while(!process.StandardOutput.EndOfStream)
            {
                processOutput += process.StandardOutput.ReadLine();
            }
            int startPos = processOutput.IndexOf("<Files>");
            int endPos = processOutput.IndexOf("</Files>");
            if (startPos>0 && endPos>0)
            {
                string xml = processOutput.Substring(startPos, endPos - startPos + ("</Files>").Length);

                XDocument doc = XDocument.Parse(xml);
                XElement[] inputFiles = doc.Descendants()
                                        .Where(e => e.Name == "Input")
                                        .ToArray();
                XElement[] outputFiles = doc.Descendants()
                                            .Where(e => e.Name == "Output")
                                            .ToArray();
                foreach (XElement e in inputFiles)
                {
                    if (!job.inputFiles.Contains(e.Value))
                        job.inputFiles.Add(e.Value);
                }
                foreach (XElement e in outputFiles)
                {
                    if (!job.outputFiles.Contains(e.Value))
                        job.outputFiles.Add(e.Value);
                }
            }
            Monitor.Exit(o);
        }
     
        private static string GetPath(XElement element)
        {
            var nodes = new List<string>();
            var node = element;
            while (node != null)
            {
                nodes.Add(node.Name.ToString());
                node = node.Parent;
            }

            return string.Join("\\", Enumerable.Reverse(nodes));
        }


        public static string GetRelativePathTo(string absPath, string relTo)
        {
            string[] absDirs = absPath.Split('\\');
            string[] relDirs = relTo.Split('\\');
            // Get the shortest of the two paths 
            int len = absDirs.Length < relDirs.Length ? absDirs.Length : relDirs.Length;
            // Use to determine where in the loop we exited 
            int lastCommonRoot = -1; int index;
            // Find common root 
            for (index = 0; index < len; index++)
            {
                if (absDirs[index] == relDirs[index])
                    lastCommonRoot = index;
                else break;
            }
            // If we didn't find a common prefix then throw 
            if (lastCommonRoot == -1)
            {
                return relTo;
            }
            // Build up the relative path 
            StringBuilder relativePath = new StringBuilder();
            // Add on the .. 
            for (index = lastCommonRoot + 1; index < absDirs.Length; index++)
            {
                if (absDirs[index].Length > 0) relativePath.Append("..\\");
            }
            // Add on the folders 
            if(absDirs.Contains(relDirs[lastCommonRoot])&&(len-1==lastCommonRoot))
            {
                relativePath.Append("..\\");
            }
            for (index = lastCommonRoot + 1; index < relDirs.Length - 1; index++)
            {
                relativePath.Append(relDirs[index] + "\\");
            }
            relativePath.Append(relDirs[relDirs.Length - 1]);
            relativePath.Replace('\\','/');
            return relativePath.ToString();
        }

        //returns the file's name from the full path
        //if removeExtension==true: c:\jander\clander.txt ->clander.txt
        //else: c:\jander\clander.txt ->clander
        public static string getFileName(string Default,bool removeExtension=false, int numExtensionDots=1)
        {
            string filename;
            if (Default == null)
                return null;
            char[] separators = {'/','\\'};
            string[] tmp = Default.Split(separators);
            filename = tmp[tmp.Length - 1];

            if (!removeExtension)
                //we return the filename untouched
                return filename;

            //we have to remove the extension
            for (int i = 0; i < numExtensionDots; i++)
            {
                int index = filename.LastIndexOf('.');
                filename = filename.Remove(index);
            }
            return filename;
        }
    }
}
