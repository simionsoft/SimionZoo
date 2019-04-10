using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Threading.Tasks;
using Herd.Files;

namespace SimionLogToOfflineTraining
{
    class Program
    {
        public class Tuple
        {
            public double[] s { get; } = null;
            public double[] a { get; } = null;
            public double[] s_p { get; } = null;
            public double[] r { get; } = null;

            public Tuple(Log.Descriptor descriptor)
            {
                s = new double[descriptor.StateVariables.Count];
                a = new double[descriptor.ActionVariables.Count];
                s_p = new double[descriptor.StateVariables.Count];
                r = new double[descriptor.RewardVariables.Count];
            }

            public void DrawRandomSample(Log.Data logData)
            {

            }

            public void SaveToFile(BinaryWriter writer)
            {

            }
        }
        static void Main(string[] args)
        {
            //Parse the input arguments
            const string inputFolderArg = "dir=";
            const string numTuplesArg = "num-tuples=";
            const string outputFileArg = "output=";

            string inputFolder = null;
            int numTuples = 0;
            string outputFile = null;

            foreach(string arg in args)
            {
                if (arg.StartsWith(inputFolderArg)) inputFolder = arg.Substring(inputFolderArg.Length);
                if (arg.StartsWith(numTuplesArg)) numTuples = int.Parse(arg.Substring(numTuplesArg.Length));
            }

            if (inputFolder == null || numTuples == 0)
            {
                Console.WriteLine("ERROR. Usage: SimionLogToOfflineTraining " + inputFolderArg + "<dir> " + numTuplesArg + "<numTuples>");
                return;
            }

            //Traverse all the log files
            string[] logDescriptorFiles = Directory.GetFiles(inputFolder, "*" + Herd.Files.Extensions.LogDescriptorExtension, SearchOption.AllDirectories);
            int numFiles = logDescriptorFiles.Length;

            if (numFiles == 0)
            {
                Console.WriteLine("ERROR. No log files found in the provided directory: " + inputFolder);
                return;
            }

            int numTuplesPerFile = numTuples / numFiles;

            //We use the first descriptor as the common descriptor used for tuples. We are assuming they all have the same variables. BEWARE!!
            Log.Descriptor commonDescriptor = new Log.Descriptor(logDescriptorFiles[0]);
            Tuple sampledTuple = new Tuple(commonDescriptor);



            using ()
            foreach (string logDescriptorFile in logDescriptorFiles)
            {
                Log.Descriptor logDescriptor = new Log.Descriptor(logDescriptorFile);
                string folder = Path.GetDirectoryName(logDescriptorFile);

                Log.Data data = new Log.Data();
                data.Load(folder + "\\" + logDescriptor.BinaryLogFile);
                if (data.SuccessfulLoad)
                {
                    if (sampledTuple.s.Length + sampledTuple.a.Length + sampledTuple.r.Length ==
                        logDescriptor.StateVariables.Count + logDescriptor.ActionVariables.Count + logDescriptor.RewardVariables.Count)
                    {
                        Console.WriteLine(logDescriptorFile + ": " + data.TotalNumEpisodes + " episodes");
                        for (int sample = 0; sample < numTuplesPerFile; sample++)
                        {
                            sampledTuple.DrawRandomSample(data);
                            sampledTuple.SaveToFile(outputTrainingFile);
                        }
                    }
                }
            }
        }
    }
}