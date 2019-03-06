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
        static List<ParameterizedObject> m_parameterizedObjects = new List<ParameterizedObject>();
        public static ParameterizedObject getNamedParamObject(string name)
        {
            return m_parameterizedObjects.Find(obj => obj.name == name);
        }

        public static void getEnclosedBody(string content, int startIndex, string openChar, string closeChar
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
        public static void addIndentation(ref string definition, int level)
        {
            for (int i = 0; i < level; i++) definition += "  ";
        }

        void ParseConstructors(string content)
        {
            string sPattern = @"(\w+)::\1\(\s*ConfigNode\s*\*\s*([^)]+)\)";

            string className, paramName, prefix, definition;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                className = match.Groups[1].Value;
                //Console.WriteLine("Found constructor definition: " + className);
                paramName = match.Groups[2].Value;
                getEnclosedBody(content, match.Index + match.Length, "{", "}"
                    , out definition, out prefix);
                m_parameterizedObjects.Add(new Constructor(className, paramName, definition, prefix));
            }
        }
        void ParseFactories(string content)
        {
            string sPattern = @"(\w+)::getInstance\(\s*ConfigNode\s*\*\s*([^)]+)\)";

            string className, paramName, prefix, definition;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                className = match.Groups[1].Value + "-Factory";
                //Console.WriteLine("Found factory definition: " + className);
                paramName = match.Groups[2].Value;
                getEnclosedBody(content, match.Index + match.Length, "{", "}"
                    , out definition, out prefix);
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

        List<ObjectClass> Classes = new List<ObjectClass>();

        void ParseAllMethodsAndComments(string filename, string content)
        {
            //We only process comments starting with ///
            string sPattern = @"(///[^\r\n]+\r\n)+(\w+)\s+(\w+)::(\w+)\(([^\)]*)\)";
            string className, methodName, returnType, arguments;
            CaptureCollection comments;
            foreach (Match match in Regex.Matches(content, sPattern))
            {
                comments = match.Groups[1].Captures;
                returnType = match.Groups[2].Value;
                className = match.Groups[3].Value;
                methodName = match.Groups[4].Value;
                arguments = match.Groups[5].Value;

                ObjectClass objClass = Classes.Find(c => c.Name == className);
                if (objClass == null)
                {
                    objClass = new ObjectClass(filename, className);
                    Classes.Add(objClass);
                }
                objClass.AddMethod(new ClassMethod(methodName, comments, arguments, returnType, ClassMethod.MethodType.Regular));
            }
        }
        //PUBLIC methods//////////////////////////////////////////////////
        public int numCharsProcessed = 0;
        public SimionSrcParser() { }
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

            ParseAllMethodsAndComments(Herd.Utils.GetFilename(filename), originalFileContents);
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
        public int PostProcess()
        {
            return 0;
        }
        public void SaveGUIParameters(string filename)
        {
            FileStream file = new FileStream(filename, FileMode.Create);
            StreamWriter outputFile = new StreamWriter(file);

            //// encoding=\"utf-8\"
            outputFile.WriteLine("<?xml version=\"1.0\"?>\n<DEFINITIONS>");
            foreach (ParameterizedObject paramObj in m_parameterizedObjects)
                outputFile.Write(paramObj.outputXML(1));
            outputFile.Write(@"</DEFINITIONS>");
            outputFile.Close();
        }
        void ExportMethod(StreamWriter writer, ClassMethod method, ReferenceDocExporter.Format format)
        {
            ReferenceDocExporter.Title3(writer, ReferenceDocExporter.InlineCode(method.ReturnType + " " + method.Name + "(" + method.Arguments + ")", format), format);

            ReferenceDocExporter.OpenList(writer, format);
            if (method.MethodSummary != null)
            {
                ReferenceDocExporter.ListItem(writer, ReferenceDocExporter.InlineBold("Summary", format), format);
                ReferenceDocExporter.PlainText(writer, method.MethodSummary, format);
            }
            //export inputs and their descriptions
            if (method.ArgumentDescriptions.Keys.Count > 0)
            {
                ReferenceDocExporter.ListItem(writer, ReferenceDocExporter.InlineBold("Parameters", format), format);
                ReferenceDocExporter.OpenList(writer, format);
                foreach (string argument in method.ArgumentDescriptions.Keys)
                {
                    ReferenceDocExporter.Tab(writer, ReferenceDocExporter.InlineListItem(
                        ReferenceDocExporter.InlineItalic(argument, format) + ": " + method.ArgumentDescriptions[argument], format), format);
                }
                ReferenceDocExporter.CloseList(writer, format);
            }
            //export output
            if (method.ReturnValueDescription != null)
            {
                ReferenceDocExporter.ListItem(writer, ReferenceDocExporter.InlineBold("Return Value", format), format);
                ReferenceDocExporter.PlainText(writer, method.ReturnValueDescription, format);
            }
            ReferenceDocExporter.CloseList(writer, format);
        }


        public void SaveDocumentation(string outputDir, string projectName, ReferenceDocExporter.Format format= ReferenceDocExporter.Format.Markdown)
        {
            if (!outputDir.EndsWith("\\") && !outputDir.EndsWith("/"))
                outputDir += "/";

            string extension= ReferenceDocExporter.FormatExtension(format);

            string outputIndexFile = outputDir + projectName + extension;

            Classes = Classes.OrderBy(x => x.Name).ToList();

            Directory.CreateDirectory(outputDir + projectName);
            using (StreamWriter indexWriter = File.CreateText(outputIndexFile))
            {
                ReferenceDocExporter.OpeningSection(indexWriter, format);
                ReferenceDocExporter.Title1(indexWriter, "Project: " + projectName, format);
                ReferenceDocExporter.Comment(indexWriter, "This file has been automatically generated. Please do not edit it", format);
                ReferenceDocExporter.Title2(indexWriter, "API Reference", format);
                ReferenceDocExporter.OpenList(indexWriter, format);
                foreach (ObjectClass objClass in Classes)
                {
                    ReferenceDocExporter.ListItem(indexWriter
                        , ReferenceDocExporter.InlineLink(objClass.Name,  projectName + "/" + objClass.Name + extension
                        , format), format);
                    string outputMdFile = outputDir + projectName + "/" + objClass.Name + extension;
                    using (StreamWriter classWriter = File.CreateText(outputMdFile))
                    {
                        ReferenceDocExporter.OpeningSection(classWriter, format);
                        ReferenceDocExporter.Title1(classWriter, "Class " + objClass.Name, format);
                        ReferenceDocExporter.Comment(classWriter, "Source: " + objClass.SrcFileName, format);

                        if (objClass.Constructors.Count > 0)
                        {
                            ReferenceDocExporter.Title2(classWriter, "Constructors", format);
                            foreach (ClassMethod method in objClass.Constructors)
                                ExportMethod(classWriter, method, format);
                        }
                        if (objClass.Destructors.Count > 0)
                        {
                            ReferenceDocExporter.Title2(classWriter, "Destructors", format);
                            foreach (ClassMethod method in objClass.Destructors)
                                ExportMethod(classWriter, method, format);
                        }
                        if (objClass.Methods.Count > 0)
                        {
                            ReferenceDocExporter.Title2(classWriter, "Methods", format);
                            foreach (ClassMethod method in objClass.Methods)
                                ExportMethod(classWriter, method, format);
                        }
                        ReferenceDocExporter.ClosingSection(classWriter, format);
                    }
                }
                ReferenceDocExporter.CloseList(indexWriter, format);
                ReferenceDocExporter.ClosingSection(indexWriter, format);
            }
        }
    }
}
