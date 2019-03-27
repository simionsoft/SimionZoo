using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    class CppSourceParser: SimionSrcParser
    {
        //PRIVATE stuff///////////////////////////////////////////////////
        string m_currentFile = "";
        static List<ParameterizedObject> m_parameterizedObjects = new List<ParameterizedObject>();
        public static ParameterizedObject GetNamedParamObject(string name)
        {
            return m_parameterizedObjects.Find(obj => obj.name == name);
        }

        public static void GetEnclosedBody(string content, int startIndex, string openChar, string closeChar
            , out string definition, out string prefix)
        {
            int contentLength = content.Length;
            int numOpenChars = 0;
            int firstOpenPos = content.IndexOf(openChar, startIndex);
            prefix = content.Substring(startIndex, firstOpenPos - startIndex);
            int pos = firstOpenPos + 1;
            string c = content.Substring(pos, 1);
            while (pos < contentLength && (c != closeChar || numOpenChars != 0))
            {
                if (c == openChar)
                    numOpenChars++;
                if (c == closeChar)
                    numOpenChars--;
                pos++;
                c = content.Substring(pos, 1);
            }
            definition = content.Substring(firstOpenPos + 1, pos - firstOpenPos - 1);
        }

        void ParseConstructors(string content)
        {
            string sPattern = @"(\w+)::\1\(\s*ConfigNode\s*\*\s*([^)]+)\)";

            string className, paramName;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                className = match.Groups[1].Value;
                //Console.WriteLine("Found constructor definition: " + className);
                paramName = match.Groups[2].Value;
                GetEnclosedBody(content, match.Index + match.Length, "{", "}"
                    , out string definition, out string prefix);
                m_parameterizedObjects.Add(new Constructor(className, paramName, definition, prefix));
            }
        }
        void ParseFactories(string content)
        {
            string sPattern = @"(\w+)::getInstance\(\s*ConfigNode\s*\*\s*([^)]+)\)";

            string className, paramName;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                className = match.Groups[1].Value + "-Factory";
                //Console.WriteLine("Found factory definition: " + className);
                paramName = match.Groups[2].Value;
                GetEnclosedBody(content, match.Index + match.Length, "{", "}"
                    , out string definition, out string prefix);
                m_parameterizedObjects.Add(new Factory(className, paramName, definition, prefix));
            }
        }
        void ParseEnumerations(string content)
        {
            string sPattern = @"enum class\s*(\w+)\s*{([^}]+)}";

            string enumName;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                enumName = match.Groups[1].Value;
                m_parameterizedObjects.Add(new Enumeration(match.Groups[1].Value, match.Groups[2].Value));
            }
        }


        //PUBLIC methods//////////////////////////////////////////////////
        public int numCharsProcessed = 0;
        public CppSourceParser() { }
        public void ParseSrcFile(string filename)
        {
            //Console.WriteLine("Parsing source file " + filename);
            m_currentFile = filename;
            string originalFileContents = File.ReadAllText(filename, Encoding.UTF8);
            string processedFileContents = originalFileContents.Replace('\r', ' ');
            processedFileContents = processedFileContents.Replace('\n', ' ');

            numCharsProcessed += originalFileContents.Length;
            ParseConstructors(processedFileContents);
            ParseFactories(processedFileContents);
            ParseEnumerations(processedFileContents);
        }
        public void ParseHeaderFile(string filename)
        {
            //Console.WriteLine("Parsing header file " + filename);
            m_currentFile = filename;
            string originalFileContent = File.ReadAllText(filename, Encoding.UTF8);
            string processedFileContent = originalFileContent.Replace('\r', ' ');
            processedFileContent = processedFileContent.Replace('\n', ' ');

            numCharsProcessed += originalFileContent.Length;
            ParseEnumerations(processedFileContent);
        }

        public override void ParseSourceFilesInDir(string inputDir)
        {
            //Parse .cpp files for constructor and factory definition
            List<string> sourceFiles = new List<string>(Directory.EnumerateFiles(inputDir, "*.cpp"
                , SearchOption.AllDirectories));
            foreach (var file in sourceFiles)
            {
                ParseSrcFile(file);
            }
            //Parse .h files for enumerated types
            List<string> headerFiles = new List<string>(Directory.EnumerateFiles(inputDir, "*.h"
                , SearchOption.AllDirectories));
            foreach (var file in headerFiles)
            {
                ParseHeaderFile(file);
            }
        }
        public override List<ParameterizedObject> GetParameterizedObjects()
        {
            return m_parameterizedObjects;
        }
        public override int PostProcess()
        {
            return 0;
        }

        public override int GetNumBytesProcessed()
        {
            return numCharsProcessed;
        }
    }
}
