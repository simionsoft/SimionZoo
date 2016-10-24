using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Runtime.InteropServices;
using Herd;
using System.Threading;
using System.Runtime.CompilerServices;


namespace Badger.Data
{
    public static class Utility
    {
        [DllImport(@"./RLSimionInterfaceDLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int getIOFiles(string xmlFilename, StringBuilder pBuffer, int bufferSize);
        
        
        //used to avoid readings of worl-denitions xml
        private static Dictionary<string, List<string>> xmlDic = new Dictionary<string, List<string>>();
        
        
        [MethodImpl(MethodImplOptions.Synchronized)]
        public static void getInputsAndOutputs(string path, ref CJob job)
        {
            StringBuilder myResult = new StringBuilder(204800);
            object o = myResult;
            Monitor.Enter(o);
            
           
            //StringBuilder myResult = new StringBuilder(204800);
            int error = getIOFiles(path, myResult, 204800);
            if (error == -1)
            {
                return;
            }
            else
            {
                XDocument doc = XDocument.Parse(myResult.ToString());
                XElement[] inputFiles = doc
                .Descendants()
                .Where(e => e.Name == "Input")
                .ToArray();
                XElement[] outputFiles = doc
                .Descendants()
                .Where(e => e.Name == "Output")
                .ToArray();
                foreach (XElement e in inputFiles)
                {
                    if(!job.inputFiles.Contains(e.Value))
                        job.inputFiles.Add(e.Value);
                }
                foreach (XElement e in outputFiles)
                {
                    if(!job.outputFiles.Contains(e.Value))
                        job.outputFiles.Add(e.Value);
                }
            }
            Monitor.Exit(o);
        }

        public static List<string> getInputs(string path)
        {
             StringBuilder myResult = new StringBuilder(204800);
            int error = getIOFiles(path, myResult, 204800);
            if (error == -1)
            {
                return null;
            }
            else
            {
                XDocument doc = XDocument.Parse(myResult.ToString());
                XElement[] inputFiles = doc
                .Descendants()
                .Where(e => e.Name == "Input")
                .ToArray();
                List<string> returnList = new List<string>();
                foreach(XElement e in inputFiles)
                {
                    returnList.Add(e.Value);
                }
                return returnList;
            }
                   
        }

        public static Dictionary<string, List<string>> findIOProblems(List<string> pahts)
        {
           
            Dictionary<string, List<string>> result = new Dictionary<string, List<string>>();
            Dictionary<string, string> myDic = new Dictionary<string, string>();
            foreach(string path in pahts)
            {
                StringBuilder myResult = new StringBuilder(204800);
                int error = getIOFiles(path, myResult, 204800);
                if(error==-1)
                {
                    //mostrar error
                }
                else
                {
                    XDocument doc = XDocument.Parse(myResult.ToString());
                    /*XElement[] inputFiles = doc
                    .Descendants()
                    .Where(e => e.Name=="Input")
                    .ToArray();*/
                    XElement[] outputFiles = doc
                    .Descendants()
                    .Where(e => e.Name == "Output")
                    .ToArray();
                   /* foreach(XElement e in inputFiles)
                    {
                        Console.WriteLine(e.Value);
                    }*/
                    foreach (XElement e in outputFiles)
                    {
                        if(!myDic.ContainsKey(e.Value))
                            myDic.Add(e.Value, path);
                        else if(!myDic[e.Value].Equals(path))
                        {
                            if(result.ContainsKey(e.Value))
                            {
                                List<string> tmp =result[e.Value];
                                if(!tmp.Contains(path))
                                {
                                    tmp.Add(path);
                                    result[e.Value] = tmp;
                                }
                            }
                            else
                            result.Add(e.Value, new List<string> { myDic[e.Value], path });
                        }
                   
                    }
                }
                myResult.Clear();
            }      
            return result;
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

            return string.Join("/", Enumerable.Reverse(nodes));
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
        public static string getFileName(string Default,bool removeExtension=false)
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
            int index = filename.LastIndexOf('.');
            filename = filename.Remove(index);
            return filename;
        }
    }
}
