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
using System.Threading;

namespace SimionSrcParser
{
    class MainApp
    {
        const string argSourcePrefix = "lang=";
        const string argSourceDirPrefix = "source-dir=";
        const string argOutDefsPrefix = "out-defs=";
        const string argOutDocsPrefix = "out-docs=";

        static string sourcesLanguage = "cpp"; //default value
        static string sourcesDir = null;
        static string outDefinitionsFile = null;
        static string outDocumentationFile = null;

        static bool ParseArguments(string [] args)
        {
            //lang=[cpp|csharp] source-dir=<dir> [out-defs=<path-to-xml>] [out-docs=<path-to-md>]
            foreach (string arg in args)
            {
                if (arg.StartsWith(argSourcePrefix)) sourcesLanguage = arg.Substring(argSourcePrefix.Length);
                else if (arg.StartsWith(argSourceDirPrefix)) sourcesDir = arg.Substring(argSourceDirPrefix.Length);
                else if (arg.StartsWith(argOutDefsPrefix)) outDefinitionsFile = arg.Substring(argOutDefsPrefix.Length);
                else if (arg.StartsWith(argOutDocsPrefix)) outDocumentationFile = arg.Substring(argOutDocsPrefix.Length);
            }
            if (sourcesDir == null || (outDefinitionsFile == null && outDocumentationFile == null))
                return false;   //all required arguments were not provided
            return true;        //all required arguments were provided
        }
        static int Main(string[] args)
        {
            //not to read 23.232 as 23232
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            if (!ParseArguments(args))
            {
                Console.WriteLine("ERROR. Usage: SimionSrcParser lang=[cpp|csharp] source-dir=<dir> [out-defs=<path-to-xml>] [out-docs=<path-to-md>]");
                return 0;
            }

            string outputDocsFolder = Herd.Utils.GetDirectory(outDocumentationFile);
            string projectName = Herd.Utils.RemoveExtension(Herd.Utils.GetFilename(outDocumentationFile));

            //if (sourcesLanguage == "cpp")
                SimionSrcParser parser = new CppSourceParser();

            parser.ParseSourceFilesInDir(sourcesDir);
            int numErrors = parser.PostProcess();

            if (numErrors==0)
            {
                if (parser.GetParameterizedObjects() != null)
                {
                    Console.WriteLine("Saving object definitions for Badger: " + sourcesDir + "->" + outDefinitionsFile);
                    DocumentationExporter.ExportGUIParameters(outDefinitionsFile, parser.GetParameterizedObjects());

                    HtmlExporter htmlExporter = new HtmlExporter();
                    string outputConfigFileForHumans = Herd.Utils.RemoveExtension(outDefinitionsFile) + htmlExporter.FormatExtension();
                    Console.WriteLine("Saving object definitions as Html : " + sourcesDir + "->" + outputConfigFileForHumans);
                    DocumentationExporter.ExportGUIParametersForHumans(outputConfigFileForHumans, htmlExporter, parser.GetParameterizedObjects());
                }
                if (parser.GetObjectClasses() != null)
                {
                    //Save documentation as markdown
                    MarkdownExporter markdownExporter = new MarkdownExporter();
                    Console.WriteLine("Saving source code documentation as Markdown: " + sourcesDir + "->"
                        + projectName + markdownExporter.FormatExtension());
                    DocumentationExporter.ExportDocumentation(outputDocsFolder, projectName, markdownExporter, parser.GetObjectClasses());

                    //Save documentation as html
                    HtmlExporter htmlExporter = new HtmlExporter();
                    Console.WriteLine("Saving source code documentation as Html: " + sourcesDir + "->"
                        + projectName + htmlExporter.FormatExtension());
                    DocumentationExporter.ExportDocumentation(outputDocsFolder, projectName, htmlExporter, parser.GetObjectClasses());
                }

                Console.WriteLine("Finished: {0} Kbs of code read.", parser.GetNumBytesProcessed() / 1000);
            }
            return 0;
        }
    }
}
