using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;

namespace SimionSrcParser
{
    
    class SimionSrcParser
    {
        //PRIVATE stuff///////////////////////////////////////////////////
        string m_currentFile = "";
        List<ParameterizedObject> m_parameterizedObjects= new List<ParameterizedObject>();

        public static void getEnclosedBody(string content, int startIndex,string openChar,string closeChar
            , out string definition, out string prefix)
        {
            int contentLength = content.Length;
            int numOpenChars= 0;
            int firstOpenPos = content.IndexOf(openChar, startIndex);
            prefix = content.Substring(startIndex, firstOpenPos-startIndex);
            int pos = firstOpenPos+1;
            string c = content.Substring(pos, 1);
            while (pos<contentLength && (c!=closeChar || numOpenChars!=0))
            {
                if (c ==openChar)
                    numOpenChars++;
                if (c == closeChar)
                    numOpenChars--;
                pos++;
                c = content.Substring(pos, 1);
            }
            definition = content.Substring(firstOpenPos + 1, pos -firstOpenPos- 1);
        }
        public static void addIndentation(ref string definition, int level)
        {
            for (int i = 0; i < level; i++) definition += "  ";
        }

        void parseConstructors(string content)
        {
            string sPattern = @"(\w+)::\1\(\s*CConfigNode\s*\*\s*([^)]+)\)";

            string className, paramName, prefix,definition;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                className = match.Groups[1].Value;
                Console.WriteLine("Found constructor definition: " + className);
                paramName = match.Groups[2].Value;
                getEnclosedBody(content,match.Index + match.Length,"{","}"
                    , out definition, out prefix);
                m_parameterizedObjects.Add(new Constructor(className, paramName, definition, prefix));
            }
        }
        void parseFactories(string content)
        {
            string sPattern = @"(\w+)::getInstance\(\s*CConfigNode\s*\*\s*([^)]+)\)";

            string className, paramName, prefix, definition;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                className = match.Groups[1].Value + "-Factory";
                Console.WriteLine("Found factory definition: " + className);
                paramName = match.Groups[2].Value;
                getEnclosedBody(content, match.Index + match.Length, "{", "}"
                    , out definition, out prefix);
                m_parameterizedObjects.Add(new Factory(className, paramName, definition, prefix));
            }
        }
        void parseEnumerations(string content)
        {
            string sPattern = @"enum class\s*(\w+)\s*{([^}]+)}";

            string className, paramName, prefix, definition;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                className = match.Groups[1].Value + "-Factory";

            }
        //PUBLIC methods//////////////////////////////////////////////////
        public int numCharsProcessed = 0;
        public SimionSrcParser() { }
        public void parseSrcFile(string filename)
        {
            Console.WriteLine("Parsing source file " + filename);
            m_currentFile = filename;
            string fileContents = File.ReadAllText(filename, Encoding.UTF8);
            fileContents = fileContents.Replace('\r', ' ');
            fileContents = fileContents.Replace('\n', ' ');

            numCharsProcessed += fileContents.Length;
            parseConstructors(fileContents);
            parseFactories(fileContents);
            parseEnumerations(fileContents);
        }
        public void parseHeaderFile(string filename)
        {
            Console.WriteLine("Parsing header file " + filename);
            m_currentFile = filename;
            string fileContents = File.ReadAllText(filename, Encoding.UTF8);
            fileContents = fileContents.Replace('\r', ' ');
            fileContents = fileContents.Replace('\n', ' ');

            numCharsProcessed += fileContents.Length;
            parseEnumerations(fileContents);
        }
        public int postProcess()
        {
            return 0;
        }
        public void saveDefinitions(string filename)
        {
            FileStream file = new FileStream(filename,FileMode.Create);
            StreamWriter outputFile = new StreamWriter(file);

            //// encoding=\"utf-8\"
            outputFile.WriteLine("<?xml version=\"1.0\"?>");//\n<DEFINITIONS>\n");
            foreach (ParameterizedObject paramObj in m_parameterizedObjects)
                outputFile.Write(paramObj.outputXML(0));
            //outputFile.Write(@"</DEFINITIONS>");
            outputFile.Close();
        }
    }
}
