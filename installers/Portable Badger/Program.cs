using System;
using System.Reflection;
using System.IO;
using System.IO.Compression;
using System.Collections.Generic;
using System.Diagnostics;

namespace Portable_Badger
{
    class Program
    {
        public static string inBaseRelPath = @"..\..\..\..\";
        public static string outBaseFolder;
        public static void Main()
        {
            List<string> files= new List<string>();
            string version;

            version = getVersion(inBaseRelPath + @"bin\Badger.exe");
            outBaseFolder = @"Badger-" + version + @"\";

            files.Add(inBaseRelPath + @"bin\Badger.exe");

            List<string> dependencyList = new List<string>();
            getDependencies(inBaseRelPath + @"bin\", "Badger.exe", ref dependencyList);
            files.AddRange(dependencyList);
            //for some unknown reason, this dependency is not detected, so we add it manually:
            files.Add(inBaseRelPath + @"bin\GongSolutions.Wpf.DragDrop.dll");

            files.Add(inBaseRelPath + @"bin\RLSimion.exe");
            files.Add(inBaseRelPath + @"bin\RLSimion-x64.exe");
            files.Add(inBaseRelPath + @"bin\FAST_Win32.exe");
            files.Add(inBaseRelPath + @"bin\FAST_x64.exe");
            files.Add(inBaseRelPath + @"bin\TurbSim.exe");
            files.Add(inBaseRelPath + @"bin\TurbSim64.exe");
            files.Add(inBaseRelPath + @"bin\vcruntime140.dll");
            files.Add(inBaseRelPath + @"bin\msvcp140.dll");
            files.Add(inBaseRelPath + @"bin\x64\vcruntime140.dll");
            files.Add(inBaseRelPath + @"bin\x64\msvcp140.dll");
            files.AddRange(getFilesInFolder(inBaseRelPath + @"config\", true));
            files.AddRange(getFilesInFolder(inBaseRelPath + @"experiments\examples", true));

            string outputFile = inBaseRelPath + @"PortableBadger-" + version + ".gz";
            Compress(outputFile, files);
        }

        public static string getVersion(string file)
        {
            FileVersionInfo fvi = FileVersionInfo.GetVersionInfo(file);

            return fvi.FileVersion;
        }

        public static void getDependencies(string inFolder, string module, ref List<string> dependencyList, bool bRecursive= true)
        {
            string depName, modName;

            Assembly assembly= Assembly.LoadFrom(inFolder + module);

            foreach (AssemblyName assemblyName in assembly.GetReferencedAssemblies())
            {
                modName = assemblyName.Name + ".dll";
                depName = inFolder + modName;
                if (File.Exists(depName) && !dependencyList.Exists( name =>  name == depName ))
                {
                    dependencyList.Add(depName);
                    if (bRecursive)
                        getDependencies(inFolder, modName, ref dependencyList, false);
                }
            }
        }

        public static List<string> getFilesInFolder(string folder, bool bRecursive)
        {
            List<string> files = new List<string>();

            if (bRecursive)
                files.AddRange(Directory.EnumerateFiles(folder, "*.*", SearchOption.AllDirectories));
            else
                files.AddRange(Directory.EnumerateFiles(folder));
            return files;
        }

        public static void Compress(string outputFilename,List<string> files)
        {
            uint numFilesAdded = 0;
            using (FileStream zipToOpen = new FileStream(outputFilename, FileMode.Create))
            {
                using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Update))
                {
                    foreach (string file in files)
                    {
                        if (File.Exists(file))
                        {
                            archive.CreateEntryFromFile(file, outBaseFolder + file.Substring(inBaseRelPath.Length));
                            numFilesAdded++;
                        }
                        else Console.WriteLine("Couldn't find file: {0}", file);
                    }
                }
            }
            Console.WriteLine("Added {0} files to {1}", numFilesAdded, outputFilename);
        }
    }
}
