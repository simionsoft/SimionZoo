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
using System.Security.Cryptography;

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

        /// <summary>
        /// Returns a string with a shortened copy of the input parameter <paramref name="s"/> that has
        /// a maximum length <paramref name="maxLength"/>. In case any of the characters in <paramref name="delim"/>
        /// is found, everything before the last index of it will be removed.
        /// </summary>
        /// <param name="s"></param>
        /// <param name="maxLength"></param>
        /// <param name="delim"></param>
        /// <returns></returns>
        public static string LimitLength(string s, uint maxLength, char[] delim= null)
        {
            //make sure we can prepend "..." in case we have to
            maxLength = Math.Max(4, maxLength);

            if (s.Length < maxLength) return s;
            if (delim != null)
            {

                //look for any delimiter: if there's one, just remove everything before it
                //if s="../experiment/config/jarl.txt" and delim='/', it would return "jarl.txt"
                int pos = s.LastIndexOfAny(delim);
                if (pos >= 0)
                    return s.Substring(pos + 1);
            }
            //if no delimiter is found, just take the last maxLength-3 characters and prepend "..."
            return "..." + s.Substring((int)( s.Length - maxLength + 3));
        }

        /// <summary>
        /// This function returns a string->string Dicionary as a single string with the format:
        /// key1=value1\nkey2=value2\n...
        /// </summary>
        /// <param name="dictionary"></param>
        /// <returns></returns>
        public static string DictionaryAsString(Dictionary<string,string> dictionary)
        {
            string res = "";
            foreach (string key in dictionary.Keys)
            {
                res += key + "= " + dictionary[key] + "\n";
            }
            res= res.TrimEnd('\n');
            return res;
        }

        /// <summary>
        /// This function returns a list of strings as a single string with the format:
        /// element1-<paramref name="delimiter"/>-element2-<paramref name="delimiter"/>-...
        /// </summary>
        /// <param name="list"></param>
        /// <param name="delimiter"/>
        /// <returns></returns>
        public static string ListAsString(List<string> list, char delimiter='\n')
        {
            string res = "";
            foreach (string element in list)
            {
                res += element + delimiter;
            }
            res = res.TrimEnd(delimiter);
            return res;
        }

        /// <summary>
        /// This function does some "intelligent" conversions in the input string to try to automatically
        /// adapt it to Oxyplot's math notation
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        public static string OxyPlotMathNotation(string s)
        {
            //Subscripts
            //E_p => E_{p}
            //E_int_omega_r = E_{int_omega_r}
            //E_p,w_r => E_{p},w_{r}
            int firstUnderscore, nextDelimiter, nextComma, nextEqual, nextSemicolon, charCount;
            string underText;
            
            firstUnderscore= s.IndexOf('_');
            while (firstUnderscore >= 0)
            {
                nextComma = s.IndexOf(',', firstUnderscore + 1);
                nextEqual = nextDelimiter = s.IndexOf('=', firstUnderscore + 1);
                nextSemicolon = nextDelimiter = s.IndexOf(';', firstUnderscore + 1);
                nextDelimiter = s.Length;
                if (nextComma >= 0) nextDelimiter= nextComma;
                if (nextEqual >= 0) nextDelimiter = Math.Min(nextDelimiter,nextEqual);
                if (nextSemicolon >= 0) nextDelimiter = Math.Min(nextDelimiter, nextSemicolon);

                charCount= nextDelimiter - firstUnderscore - 1;
                underText = s.Substring(firstUnderscore + 1, charCount);
                s = s.Substring(0, firstUnderscore + 1) + "{" + underText + "}"
                    + s.Substring(firstUnderscore+1+charCount);
                //we skip the last delimiter, the undertext, and the "{" we added
                firstUnderscore = s.IndexOf('_', firstUnderscore + underText.Length + 2); 
            }

            //Greek characters
            //α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω
            //Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ Σ Τ Υ Φ Χ Ψ Ω
            s= s.Replace("alpha", "α");
            s = s.Replace("Alpha", "α");
            s= s.Replace("beta", "β");
            s= s.Replace("Beta", "β");
            s= s.Replace("gamma", "γ");
            s= s.Replace("Gamma", "γ");
            s= s.Replace("delta", "δ");
            s= s.Replace("Delta", "Δ");
            s= s.Replace("epsilon", "ε");
            s= s.Replace("Epsilon", "Ε");
            s= s.Replace("theta", "θ");
            s= s.Replace("Theta", "Θ");
            s= s.Replace("lambda", "λ");
            s= s.Replace("Lambda", "λ");
            s= s.Replace("mu", "μ");
            s= s.Replace("Mu", "μ");
            s= s.Replace("sigma", "σ");
            s= s.Replace("Sigma", "Σ");
            s= s.Replace("omega", "ω");
            s= s.Replace("Omega", "Ω");

            return s;
        }
    }
}
