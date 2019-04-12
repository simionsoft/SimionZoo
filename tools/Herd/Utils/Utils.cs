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
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Herd.Files;
using System.Diagnostics;

namespace Herd
{
    public class Utils
    {
        /// <summary>
        /// Returns whether the file exists and is not empty.
        /// </summary>
        /// <param name="fileName">Name of the file.</param>
        public static bool FileExistsAndNotEmpty(string fileName)
        {
            if (File.Exists(fileName))
            {
                FileInfo fileInfo = new FileInfo(fileName);
                if (fileInfo.Length > 0)
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Sets the execution permission of a file. Needed in the Linux version of the herd agent after receiving
        /// an executable
        /// </summary>
        /// <param name="filename">The path to the executable file</param>
        public static void SetExecutionPermission(string filename)
        {
            Process.Start("chmod", "770 " + filename);
        }

        /// <summary>
        /// Gets the log file path that will be created from an experimental unit file
        /// </summary>
        /// <param name="experimentFilePath">The experimental unit file path</param>
        /// <param name="descriptor">If true, the path to the descriptor will be returned. Otherwise, the path to the binary file</param>
        /// <returns>The path to the log file</returns>
        public static string GetLogFilePath(string experimentFilePath, bool descriptor = true)
        {
            if (experimentFilePath != "")
            {
                if (descriptor)
                    return RemoveExtension(experimentFilePath, 2) + Extensions.LogDescriptor;
                else
                    return RemoveExtension(experimentFilePath, 2) + Extensions.LogBinaryFile;
            }

            return "";
        }

        /// <summary>
        /// Calculates the relative path of an absolute path with respect to a second path
        /// </summary>
        /// <param name="absPath">The abssolute path</param>
        /// <param name="relTo">The path to which we want the relative path</param>
        /// <returns>The relative path</returns>
        public static string GetRelativePathTo(string absPath, string relTo)
        {
            string[] absDirs = absPath.Split('/');
            string[] relDirs = relTo.Split('/');
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
                if (absDirs[index].Length > 0) relativePath.Append("../");
            }
            // Add on the folders 
            if (absDirs.Contains(relDirs[lastCommonRoot]) && (len - 1 == lastCommonRoot))
            {
                relativePath.Append("../");
            }
            for (index = lastCommonRoot + 1; index < relDirs.Length - 1; index++)
            {
                relativePath.Append(relDirs[index] + "/");
            }
            relativePath.Append(relDirs[relDirs.Length - 1]);
            relativePath.Replace('\\', '/');
            return relativePath.ToString();
        }

        /// <summary>
        /// Given the path (relative or absolute) of a file, this function removes all but "allowedLevels" levels of directories
        /// For example:
        // -removeDirectories("c:\jander\clander\more.txt",2) -> "jander\clander\more.txt"
        // -removeDirectories("c:\jander\clander\more.txt",0) -> "more.txt"
        ///<param name="fileName">path to a file from which directories must be removed</param>
        ///<param name="allowedLevels">number of directory levels to be left in the returned path</param>
        /// </summary>
        public static string RemoveDirectories(string fileName, uint allowedLevels = 0)
        {
            if (fileName != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                int startPos = fileName.Length - 1;

                lastPos1 = fileName.LastIndexOf("/", startPos);
                lastPos2 = fileName.LastIndexOf("\\", startPos);
                lastPos = Math.Max(lastPos1, lastPos2);

                while (lastPos >= 0 && allowedLevels > 0)
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

        /// Returns the number of parts in a string, using separator as delimiter of the parts.
        /// For example, NumParts("harl.demore.pitikan",'.') should return 3
        /// For simplicity, we assume the separator is neither the first nor the last char in the string
        /// </summary>
        /// <param name="filename">The filename</param>
        /// <param name="separator">The separator character</param>
        /// <returns>The number of parts</returns>
        public static int NumParts(string filename, char separator)
        {
            int numSeparators = filename.Count(c => c == separator);

            if (filename[0] == separator) //to avoid returning 3 parts with ".simion.batch"
                return numSeparators;
            return numSeparators + 1;
        }


        /// <summary>
        /// Given the path to a file, this function returns [numExtensions] extensions of the file.
        /// If not enough extensions are found, nothing is changed from the input path
        /// </summary>
        /// <param name="filename">The filename.</param>
        /// <param name="numExtensions">The number extensions.</param>
        /// <returns>The file's extension</returns>
        public static string GetExtension(string filename, uint numExtensions = 1)
        {
            string result = filename;
            int lastBar;
            lastBar = Math.Max(filename.LastIndexOf('/'), filename.LastIndexOf('\\'));
            int numExtensionsFound = 0;
            int index = filename.Length - 1;
            while (index > 0 && numExtensionsFound < numExtensions && index > lastBar)
            {
                index--;
                if (filename[index] == '.')
                    numExtensionsFound++;
            }
            if (numExtensionsFound == numExtensions)
                return filename.Substring(index);
            return filename;
        }


        /// <summary>
        /// Given the path to a file, this function removes [numExtensions] extensions of the file.
        /// If not enough extensions are found, nothing is changed from the input path
        /// </summary>
        /// <param name="filename">The filename.</param>
        /// <param name="numExtensions">The number extensions.</param>
        /// <returns>The filename without the extensions</returns>
        public static string RemoveExtension(string filename, int numExtensions = 1)
        {
            string result = filename;
            int lastExt, lastBar;
            lastBar = Math.Max(filename.LastIndexOf('/'), filename.LastIndexOf('\\'));
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
        public static string LimitLength(string s, uint maxLength, char[] delim = null)
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
            return "..." + s.Substring((int)(s.Length - maxLength + 3));
        }

        /// <summary>
        /// This function returns a string->string Dicionary as a single string with the format:
        /// key1=value1\nkey2=value2\n...
        /// </summary>
        /// <param name="dictionary"></param>
        /// <returns></returns>
        public static string DictionaryAsString(Dictionary<string, string> dictionary)
        {
            string res = "";
            foreach (string key in dictionary.Keys)
            {
                res += key + "= " + dictionary[key] + "\n";
            }
            res = res.TrimEnd('\n');
            return res;
        }

        /// <summary>
        /// This function returns a list of strings as a single string with the format:
        /// element1-<paramref name="delimiter"/>-element2-<paramref name="delimiter"/>-...
        /// </summary>
        /// <param name="list"></param>
        /// <param name="delimiter"/>
        /// <returns></returns>
        public static string ListAsString(List<string> list, char delimiter = '\n')
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
            if (s == null) return null;
            //Subscripts
            //E_p => E_{p}
            //E_int_omega_r = E_{int_omega_r}
            //E_p,w_r => E_{p},w_{r}
            int firstUnderscore, nextDelimiter, nextComma, nextEqual, nextSemicolon, charCount;
            string underText;

            firstUnderscore = s.IndexOf('_');
            while (firstUnderscore >= 0)
            {
                nextComma = s.IndexOf(',', firstUnderscore + 1);
                nextEqual = nextDelimiter = s.IndexOf('=', firstUnderscore + 1);
                nextSemicolon = nextDelimiter = s.IndexOf(';', firstUnderscore + 1);
                nextDelimiter = s.Length;
                if (nextComma >= 0) nextDelimiter = nextComma;
                if (nextEqual >= 0) nextDelimiter = Math.Min(nextDelimiter, nextEqual);
                if (nextSemicolon >= 0) nextDelimiter = Math.Min(nextDelimiter, nextSemicolon);

                charCount = nextDelimiter - firstUnderscore - 1;
                underText = s.Substring(firstUnderscore + 1, charCount);
                s = s.Substring(0, firstUnderscore + 1) + "{" + underText + "}"
                    + s.Substring(firstUnderscore + 1 + charCount);
                //we skip the last delimiter, the undertext, and the "{" we added
                firstUnderscore = s.IndexOf('_', firstUnderscore + underText.Length + 2);
            }

            //Greek characters
            //α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω
            //Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ Σ Τ Υ Φ Χ Ψ Ω
            s = s.Replace("alpha", "α");
            s = s.Replace("Alpha", "α");
            s = s.Replace("beta", "β");
            s = s.Replace("Beta", "β");
            s = s.Replace("gamma", "γ");
            s = s.Replace("Gamma", "γ");
            s = s.Replace("delta", "δ");
            s = s.Replace("Delta", "Δ");
            s = s.Replace("epsilon", "ε");
            s = s.Replace("Epsilon", "Ε");
            s = s.Replace("theta", "θ");
            s = s.Replace("Theta", "Θ");
            s = s.Replace("lambda", "λ");
            s = s.Replace("Lambda", "λ");
            s = s.Replace("mu", "μ");
            s = s.Replace("Mu", "μ");
            s = s.Replace("sigma", "σ");
            s = s.Replace("Sigma", "Σ");
            s = s.Replace("omega", "ω");
            s = s.Replace("Omega", "Ω");

            return s;
        }
    }
}
