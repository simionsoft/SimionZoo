using System;
using System.Reflection;
using System.IO;
using System.IO.Compression;
using System.Collections.Generic;
using System.Diagnostics;

namespace SimionZooBundler
{
    class Program
    {
        public static string inBaseRelPath = @"../../../../";
        public static string outBaseFolder;

        static void AddFile(ref List<string> fileList, string newFile)
        {
            if (!fileList.Contains(newFile))
                fileList.Add(newFile);
        }

        public static void Main(string [] args)
        {
            bool includeWindowsFiles = true;
            bool includeLinuxFiles = true;
            string osName = "";

            foreach(string arg in args)
            {
                string lowerCaseArg = arg.ToLower();
                if (lowerCaseArg == "-only-windows")
                {
                    includeLinuxFiles = false;
                    osName = "win-";
                }
                else if (lowerCaseArg == "-only-linux")
                {
                    includeWindowsFiles = false;
                    osName = "linux-";
                }
            }
            List<string> files= new List<string>();
            string version;

            version = GetVersion(inBaseRelPath + @"bin/Badger.exe");
            outBaseFolder = @"SimionZoo-" + version + @"/";

            //Herd Agent
            //windows:
            if (includeWindowsFiles)
            {
                AddFile(ref files, inBaseRelPath + @"installers/HerdAgentInstaller.msi");
                AddFile(ref files, inBaseRelPath + @"bin/HerdAgent.exe");
                AddFile(ref files, inBaseRelPath + @"bin/Herd.dll");
            }
            //linux:
            if (includeLinuxFiles)
            {
                AddFile(ref files, inBaseRelPath + @"bin/HerdAgent.exe");
                AddFile(ref files, inBaseRelPath + @"bin/Herd.dll");
                AddFile(ref files, inBaseRelPath + @"installers/herd-agent-installer.sh");
                AddFile(ref files, inBaseRelPath + @"installers/herd-agent.service");
                AddFile(ref files, inBaseRelPath + @"installers/daemon");
            }

            //Badger
            AddFile(ref files, inBaseRelPath + @"bin/BadgerConsole.exe");

            if (includeWindowsFiles)
            {
                AddFile(ref files, inBaseRelPath + @"bin/Badger.exe");
                //Badger - SimionLogViewer
                AddFile(ref files, inBaseRelPath + @"bin/SimionLogViewer.exe");

                List<string> dependencyList = new List<string>();
                GetDependencies(inBaseRelPath + @"bin/", "Badger.exe", ref dependencyList);
                foreach (string dependency in dependencyList)
                {
                    if (!files.Contains(dependency))
                        AddFile(ref files, dependency);
                }
            }

            //RLSimion
            AddFile(ref files, inBaseRelPath + @"bin/RLSimion.exe");
            AddFile(ref files, inBaseRelPath + @"bin/RLSimion-x64.exe");
            AddFile(ref files, inBaseRelPath + @"bin/RLSimion-linux-x64.exe");
            //FAST
            AddFile(ref files, inBaseRelPath + @"bin/openfast_Win32.exe");
            AddFile(ref files, inBaseRelPath + @"bin/TurbSim.exe");
            AddFile(ref files, inBaseRelPath + @"bin/MAP_Win32.dll");
            AddFile(ref files, inBaseRelPath + @"bin/FASTDimensionalPortal.dll");

            //C++ Runtime libraries: x86 and x64 versions
            AddFile(ref files, inBaseRelPath + @"bin/vcruntime140.dll");
            AddFile(ref files, inBaseRelPath + @"bin/msvcp140.dll");
            AddFile(ref files, inBaseRelPath + @"bin/x64/vcruntime140.dll");
            AddFile(ref files, inBaseRelPath + @"bin/x64/msvcp140.dll");
            //CNTK library and dependencies
            //windows:
            AddFile(ref files, inBaseRelPath + @"bin/CNTKWrapper.dll");
            AddFile(ref files, inBaseRelPath + @"bin/Cntk.Composite-2.5.1.dll");
            AddFile(ref files, inBaseRelPath + @"bin/Cntk.Core-2.5.1.dll");
            AddFile(ref files, inBaseRelPath + @"bin/Cntk.Math-2.5.1.dll");
            AddFile(ref files, inBaseRelPath + @"bin/Cntk.PerformanceProfiler-2.5.1.dll");
            AddFile(ref files, inBaseRelPath + @"bin/cublas64_90.dll");
            AddFile(ref files, inBaseRelPath + @"bin/cudart64_90.dll");
            AddFile(ref files, inBaseRelPath + @"bin/cudnn64_7.dll");
            AddFile(ref files, inBaseRelPath + @"bin/libiomp5md.dll");
            AddFile(ref files, inBaseRelPath + @"bin/mklml.dll");
            AddFile(ref files, inBaseRelPath + @"bin/mkldnn.dll");
            AddFile(ref files, inBaseRelPath + @"bin/nvml.dll");
            //linux:
            AddFile(ref files, inBaseRelPath + @"bin/libCNTKWrapper-linux.so");
            AddFile(ref files, inBaseRelPath + @"bin/cntk-linux/libCntk.Core-2.5.1.so");
            AddFile(ref files, inBaseRelPath + @"bin/cntk-linux/libCntk.Math-2.5.1.so");
            AddFile(ref files, inBaseRelPath + @"bin/cntk-linux/libCntk.PerformanceProfiler-2.5.1.so");
            AddFile(ref files, inBaseRelPath + @"bin/cntk-linux/libmklml_intel.so");
            AddFile(ref files, inBaseRelPath + @"bin/cntk-linux/libiomp5.so");

            //Config files and example experiments
            files.AddRange(GetFilesInFolder(inBaseRelPath + @"experiments/examples", true, @"*.simion.exp"));
            files.AddRange(GetFilesInFolder(inBaseRelPath + @"experiments/examples", true, @"*.simion.proj"));

            files.AddRange(GetFilesInFolder(inBaseRelPath + @"config/", true));


            string outputFile = inBaseRelPath + @"SimionZoo-" + osName + version + ".zip";

            Console.WriteLine("Compressing {0} files", files.Count);
            Compress(outputFile, files);
            Console.WriteLine("Finished");
        }

        public static string GetVersion(string file)
        {
            return Herd.Files.Version.SimionZoo;
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
            using (FileStream zipToOpen = new FileStream(outputFilename, FileMode.Create))
            {
                using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Update))
                {
                    foreach (string file in files)
                    {
                        if (System.IO.File.Exists(file))
                        {
                            Console.WriteLine("Adding file to bundle: " + file);
                            archive.CreateEntryFromFile(file, outBaseFolder + file.Substring(inBaseRelPath.Length));
                        }
                        else Console.WriteLine("ERROR: Couldn't find file {0}", file);
                    }
                }
            }
        }
    }
}
