using System;
using System.Reflection;
using System.IO;
using System.IO.Compression;
using System.Collections.Generic;
using System.Diagnostics;
using IWshRuntimeLibrary;

namespace Portable_Badger
{
    class Program
    {
        public static string inBaseRelPath = @"../../../../";
        public static string outBaseFolder;
        public static void Main()
        {
            List<string> files= new List<string>();
            string version;

            version = GetVersion(inBaseRelPath + @"bin/Badger.exe");
            outBaseFolder = @"SimionZoo-" + version + @"/";

            //Herd Agent
            files.Add(inBaseRelPath + @"bin/HerdAgentInstaller.msi");

            //Badger
            files.Add(inBaseRelPath + @"bin/Badger.exe");

            //Badger - SimionLogViewer
            files.Add(inBaseRelPath + @"bin/SimionLogViewer.exe");

            List<string> dependencyList = new List<string>();
            GetDependencies(inBaseRelPath + @"bin/", "Badger.exe", ref dependencyList);
            files.AddRange(dependencyList);

            //RLSimion
            files.Add(inBaseRelPath + @"bin/RLSimion.exe");
            files.Add(inBaseRelPath + @"bin/RLSimion-x64.exe");
            //FAST
            files.Add(inBaseRelPath + @"bin/openfast_Win32.exe");
            files.Add(inBaseRelPath + @"bin/TurbSim.exe");
            files.Add(inBaseRelPath + @"bin/MAP_Win32.dll");
            files.Add(inBaseRelPath + @"bin/FASTDimensionalPortal.dll");

            //C++ Runtime libraries: x86 and x64 versions
            files.Add(inBaseRelPath + @"bin/vcruntime140.dll");
            files.Add(inBaseRelPath + @"bin/msvcp140.dll");
            files.Add(inBaseRelPath + @"bin/x64\vcruntime140.dll");
            files.Add(inBaseRelPath + @"bin/x64\msvcp140.dll");
            //CNTK library and dependencies
            files.Add(inBaseRelPath + @"bin/CNTKWrapper.dll");
            files.Add(inBaseRelPath + @"bin/Cntk.Composite-2.5.1.dll");
            files.Add(inBaseRelPath + @"bin/Cntk.Core-2.5.1.dll");
            files.Add(inBaseRelPath + @"bin/Cntk.Math-2.5.1.dll");
            files.Add(inBaseRelPath + @"bin/Cntk.PerformanceProfiler-2.5.1.dll");
            files.Add(inBaseRelPath + @"bin/cublas64_90.dll");
            files.Add(inBaseRelPath + @"bin/cudart64_90.dll");
            files.Add(inBaseRelPath + @"bin/cudnn64_7.dll");
            files.Add(inBaseRelPath + @"bin/libiomp5md.dll");
            files.Add(inBaseRelPath + @"bin/mklml.dll");
            files.Add(inBaseRelPath + @"bin/mkl_cntk_p.dll");
            files.Add(inBaseRelPath + @"bin/mkldnn.dll");
            files.Add(inBaseRelPath + @"bin/nvml.dll");

            //Config files and example experiments
            files.AddRange(GetFilesInFolder(inBaseRelPath + @"experiments/examples", true, @"*.simion.exp"));
            files.AddRange(GetFilesInFolder(inBaseRelPath + @"experiments/examples", true, @"*.simion.proj"));

            files.AddRange(GetFilesInFolder(inBaseRelPath + @"config/", true));


            string outputFile = inBaseRelPath + @"SimionZoo-" + version + ".gz";

            Console.WriteLine("Compressing files");
            Compress(outputFile, files);
            Console.WriteLine("Finished");
        }

        public static string GetVersion(string file)
        {
            FileVersionInfo fvi = FileVersionInfo.GetVersionInfo(file);

            return fvi.FileVersion;
        }

        public static void GetDependencies(string inFolder, string module, ref List<string> dependencyList, bool bRecursive= true)
        {
            string depName, modName;

            Assembly assembly= Assembly.LoadFrom(inFolder + module);

            foreach (AssemblyName assemblyName in assembly.GetReferencedAssemblies())
            {
                modName = assemblyName.Name + ".dll";
                depName = inFolder + modName;
                if (System.IO.File.Exists(depName) && !dependencyList.Exists( name =>  name == depName ))
                {
                    dependencyList.Add(depName);
                    if (bRecursive)
                        GetDependencies(inFolder, modName, ref dependencyList, false);
                }
            }
        }

        public static List<string> GetFilesInFolder(string folder, bool bRecursive, string filter = "*.*")
        {
            List<string> files = new List<string>();

            if (bRecursive)
                files.AddRange(Directory.EnumerateFiles(folder, filter, SearchOption.AllDirectories));
            else
                files.AddRange(Directory.EnumerateFiles(folder));
            return files;
        }

        public static void Compress(string outputFilename,List<string> files)
        {
            uint numFilesAdded = 0;
            double totalNumFiles = (double) files.Count;
            using (FileStream zipToOpen = new FileStream(outputFilename, FileMode.Create))
            {
                using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Update))
                {
                    foreach (string file in files)
                    {
                        if (System.IO.File.Exists(file))
                        {
                            archive.CreateEntryFromFile(file, outBaseFolder + file.Substring(inBaseRelPath.Length));
                            numFilesAdded++;
                        }
                        else Console.WriteLine("Couldn't find file: {0}", file);

                        Console.Write("\rProgress: {0:F2}%", 100.0*((double)numFilesAdded)/ totalNumFiles);
                    }
                    Console.WriteLine("\nSaving {0} files in  {1}", numFilesAdded, Path.GetFullPath( outputFilename) );
                }
            }
        }
    }
}
