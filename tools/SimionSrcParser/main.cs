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
    class main
    {
        static int Main(string[] args)
        {
            //not to read 23.232 as 23232
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            SimionSrcParser sourceProcessor = new SimionSrcParser();
            int numargs = args.Length;
            if (numargs < 3)
            {
                Console.WriteLine("ERROR. Usage: SimionSrcParser <input-directory> <output-config-file (.xml)> <output-docs-file (.md)>");
                return 0;
            }

            string inputDir = args[0];
            string outputConfigFile = args[1];
            string outputDocsFile = args[2];
            string outputDocsFolder = Herd.Utils.GetDirectory(outputDocsFile);
            string projectName = Herd.Utils.RemoveExtension(Herd.Utils.GetFilename(outputDocsFile));

            //Parse .cpp files for constructor and factory definition
            List<string> sourceFiles = new List<string>(Directory.EnumerateFiles(inputDir, "*.cpp", SearchOption.AllDirectories));
            foreach (var file in sourceFiles)
            {
                sourceProcessor.ParseSrcFile(file);
            }
            //Parse .h files for enumerated types
            List<string> headerFiles = new List<string>(Directory.EnumerateFiles(inputDir, "*.h", SearchOption.AllDirectories));
            foreach (var file in headerFiles)
            {
                sourceProcessor.ParseHeaderFile(file);
            }


            int numErrors = sourceProcessor.PostProcess();

            if (numErrors==0)
            {
                Console.WriteLine("Saving GUI Parameters configuration file: " + inputDir + "->" + outputConfigFile);
                sourceProcessor.SaveGUIParameters(outputConfigFile);
                Console.WriteLine("Saving source code documentation as Markdown files: " + inputDir + "->" + outputDocsFile);
                sourceProcessor.SaveDocumentationAsMarkdown(outputDocsFolder, projectName);

                Console.WriteLine("Finished: {0} Kbs of code read.", sourceProcessor.numCharsProcessed / 1000);
            }
            return 0;
        }
    }
}
