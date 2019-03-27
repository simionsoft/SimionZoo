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
        const string argSourceDirPrefix = "source-dir=";
        const string argOutDefsPrefix = "out-defs=";

        static List<string> sourceDirectories = new List<string>();
        static string outDefinitionsFile = null;

        static bool ParseArguments(string [] args)
        {
            //lang=[cpp|csharp] source-dir=<dir> [out-defs=<path-to-xml>] [out-docs=<path-to-md>]
            foreach (string arg in args)
            {
                if (arg.StartsWith(argSourceDirPrefix)) sourceDirectories.Add(arg.Substring(argSourceDirPrefix.Length));
                else if (arg.StartsWith(argOutDefsPrefix)) outDefinitionsFile = arg.Substring(argOutDefsPrefix.Length);

            }
            if (sourceDirectories.Count == 0 || outDefinitionsFile == null)
                return false;   //all required arguments were not provided

            return true;        //all required arguments were provided
        }
        static int Main(string[] args)
        {
            //not to read 23.232 as 23232
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            if (!ParseArguments(args))
            {
                Console.WriteLine("ERROR. Usage: SimionSrcParser lang=[cpp|csharp] source-dir=<dir> [out-defs=<path-to-xml>]");
                return 0;
            }

            //Create the appropriate parser
            SimionSrcParser parser = null;
            parser = new CppSourceParser();

            foreach(string sourceDir in sourceDirectories)
                parser.ParseSourceFilesInDir(sourceDir);

            int numErrors = parser.PostProcess();

            if (numErrors==0)
            {
                string inputSourceDirectories = "{ ";
                foreach (string sourceDir in sourceDirectories) inputSourceDirectories += sourceDir + "; ";
                inputSourceDirectories += "}";

                if (outDefinitionsFile != null && parser.GetParameterizedObjects() != null)
                {
                    Console.WriteLine("Saving object definitions for Badger: " + inputSourceDirectories + " ->" + outDefinitionsFile);
                    DocumentationExporter.ExportGUIParameters(outDefinitionsFile, parser.GetParameterizedObjects());
                }

                Console.WriteLine("Finished: {0} Kbs of code read.", parser.GetNumBytesProcessed() / 1000);
            }
            return 0;
        }
    }
}
