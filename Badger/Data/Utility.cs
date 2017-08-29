using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Herd;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Threading;
using System.IO;
using System;

namespace Badger.Data
{
    public static class Utility
    {
        //this function is called from several tasks and needs to be synchronized
        [MethodImpl(MethodImplOptions.Synchronized)]
        public static void GetInputsAndOutputs(string exe, string args, ref CJob job)
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

            string processOutput = "";
            process.Start();
            while (!process.StandardOutput.EndOfStream)
            {
                processOutput += process.StandardOutput.ReadLine();
            }
            int startPos = processOutput.IndexOf("<Files>");
            int endPos = processOutput.IndexOf("</Files>");
            if (startPos >= 0 && endPos > 0)
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
            if (absDirs.Contains(relDirs[lastCommonRoot]) && (len - 1 == lastCommonRoot))
            {
                relativePath.Append("..\\");
            }
            for (index = lastCommonRoot + 1; index < relDirs.Length - 1; index++)
            {
                relativePath.Append(relDirs[index] + "\\");
            }
            relativePath.Append(relDirs[relDirs.Length - 1]);
            relativePath.Replace('\\', '/');
            return relativePath.ToString();
        }

        /// <summary>
        /// Given the path (relative or absolute) of a file, this function removes all but "allowedLevels" levels of directories
        ///<param name="fileName">path to a file from which directories must be removed</param>
        ///<param name="allowedLevels">number of directory levels to be left in the returned path</param>
        /// </summary>
        //For example:
        //-removeDirectories("c:\jander\clander\more.txt",2) -> "jander\clander\more.txt"
        //-removeDirectories("c:\jander\clander\more.txt",0) -> "more.txt"
        public static string RemoveDirectories(string fileName,uint allowedLevels=0)
        {
            if (fileName != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                int startPos = fileName.Length - 1;

                lastPos1 = fileName.LastIndexOf("/",startPos);
                lastPos2 = fileName.LastIndexOf("\\",startPos);
                lastPos = Math.Max(lastPos1, lastPos2);

                while (lastPos>=0 && allowedLevels>0)
                {
                    allowedLevels--;
                    startPos = lastPos - 1;

                    lastPos1 = fileName.LastIndexOf("/", startPos);
                    lastPos2 = fileName.LastIndexOf("\\", startPos);
                    lastPos = Math.Max(lastPos1, lastPos2);
                }
                if (lastPos > 0)
                    return fileName.Substring(lastPos + 1);
            }
            return fileName;
        }

        //given the path to a file, this function removes [numExtensions] extensions of the file
        //if not enough extensions are found, nothing is changed from the input path
        public static string RemoveExtension(string filename, uint numExtensions= 1)
        {
            string result = filename;
            int lastExt, lastBar;
            lastBar = Math.Max(filename.LastIndexOf('/'),filename.LastIndexOf('\\'));
            lastExt = result.LastIndexOf('.');
            int numRemovedExtensions = 0;
            while (lastExt > 0 && numRemovedExtensions < numExtensions && lastExt > lastBar)
            {
                result = result.Substring(0, lastExt);
                ++numRemovedExtensions;

                lastExt = result.LastIndexOf('.');
            }
            if (numRemovedExtensions == numExtensions)
                return result;
            return filename;
        }

        //given the path (relative or absolute) of a file, this function returns the path without the
        //file's name
        //For example:
        //-"c:\jander\clander.txt" -> "c:\jander\"
        //-"clander.txt" -> ""
        public static string GetDirectory(string fileName)
        {
            if (fileName != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                lastPos1 = fileName.LastIndexOf("/");
                lastPos2 = fileName.LastIndexOf("\\");

                lastPos = Math.Max(lastPos1, lastPos2);
                if (lastPos > 0)
                    return fileName.Substring(0, lastPos + 1);
            }
            return "";
        }

        //returns the file's name from the full path
        //if removeExtension==true: c:\jander\clander.txt ->clander.txt
        //else: c:\jander\clander.txt ->clander
        public static string GetFilename(string Default, bool removeExtension = false, int numExtensionDots = 1)
        {
            string filename;
            if (Default == null)
                return null;
            char[] separators = { '/', '\\' };
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
        public static string RemoveSpecialCharacters(string filename)
        {
            string invalid = new string(Path.GetInvalidFileNameChars()) + new string(Path.GetInvalidPathChars());

            foreach (char c in invalid)
            {
                filename = filename.Replace(c.ToString(), "");
            }
            return filename;
        }

        public static string LimitLength(string s, uint maxLength, char[] delim= null)
        {
            //make sure we can prepend "..." in case we have to
            maxLength = Math.Max(4, maxLength);

            if (s.Length < maxLength) return s;
            if (delim == null) return "";

            //look for any delimiter: if there's one, just remove everything before it
            //if s="../experiment/config/jarl.txt" and delim='/', it would return "jarl.txt"
            int pos = s.LastIndexOfAny(delim);
            if (pos >= 0)
                return s.Substring(pos + 1);

            //if no delimiter is found, just take the last maxLength-3 characters and prepend "..."
            return "..." + s.Substring((int)( s.Length - maxLength + 3));
        }

        public static string DictionaryAsString(Dictionary<string,string> dictionary)
        {
            string res = "";
            foreach (string key in dictionary.Keys)
            {
                res += key + "= " + dictionary[key] + "\n";
            }
            res.TrimEnd('\n');
            return res;
        }
    }
}
