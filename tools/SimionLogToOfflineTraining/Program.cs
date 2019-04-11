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
            public double[] State { get; } = null;
            public double[] Action { get; } = null;
            public double[] State_p { get; } = null;
            public double[] Reward { get; } = null;

            public Tuple(Log.Descriptor descriptor)
            {
                State = new double[descriptor.StateVariables.Count];
                Action = new double[descriptor.ActionVariables.Count];
                State_p = new double[descriptor.StateVariables.Count];
                Reward = new double[descriptor.RewardVariables.Count];
            }

            public bool DrawRandomSample(Log.Data logData)
            {
                Log.Episode episode;
                Random randomGenerator = new Random();

                int episodeIndex = randomGenerator.Next() % logData.TotalNumEpisodes;
                if (episodeIndex < logData.EvaluationEpisodes.Count)
                    episode = logData.EvaluationEpisodes[episodeIndex];
                else
                    episode = logData.TrainingEpisodes[episodeIndex - logData.TrainingEpisodes.Count];

                if (episode.steps.Count <= 1) return false;

                int stepIndex = randomGenerator.Next() % (episode.steps.Count - 1);
                Log.Step step = episode.steps[stepIndex]; //%(numSteps-1) because we need s and s_p
                int dataOffset = 0;
                //copy s
                for (int i= 0; i<State.Length; i++) { State[i] = step.data[i]; }
                //copy a
                dataOffset = State.Length;
                for (int i = 0; i < Action.Length; i++) { Action[i] = step.data[dataOffset + i]; }
                //copy r
                dataOffset += Action.Length;
                for (int i = 0; i < Reward.Length; i++) { Reward[i] = step.data[dataOffset + i]; }

                //copy s_p from next step
                step = episode.steps[stepIndex + 1];
                dataOffset = 0;
                for (int i = 0; i < State_p.Length; i++) { State_p[i] = step.data[i]; }

                return true;
            }

            public void SaveToFile(BinaryWriter writer)
            {
                int numElementsPerTuple = 2 * State.Length + Action.Length + Reward.Length;
                byte[] buffer = new byte[sizeof(double) * numElementsPerTuple];
                Buffer.BlockCopy(State, 0, buffer, 0, sizeof(double) * State.Length);
                Buffer.BlockCopy(Action, 0, buffer, 0, sizeof(double) * Action.Length);
                Buffer.BlockCopy(State_p, 0, buffer, 0, sizeof(double) * State_p.Length);
                Buffer.BlockCopy(Reward, 0, buffer, 0, sizeof(double) * Reward.Length);
                writer.Write(buffer, 0, buffer.Length);
            }
        }

        static int GetTotalNumSamples(Log.Data data)
        {
            int totalNumSamples = 0;
            foreach(Log.Episode episode in data.Episodes)
            {
                totalNumSamples += episode.steps.Count - 1;
            }
            return totalNumSamples;
        }

        static void Main(string[] args)
        {
            //Parse the input arguments
            const string inputFolderArg = "dir=";
            const string numSamplesArg = "num-samples=";
            const string outputFileArg = "output-file=";

            string inputFolder = null;
            int numSamples = 0;
            string outputFilename = null;

            foreach(string arg in args)
            {
                if (arg.StartsWith(inputFolderArg)) inputFolder = arg.Substring(inputFolderArg.Length);
                if (arg.StartsWith(numSamplesArg)) numSamples = int.Parse(arg.Substring(numSamplesArg.Length));
                if (arg.StartsWith(outputFileArg)) outputFilename = arg.Substring(outputFileArg.Length);
            }

            if (inputFolder == null || numSamples == 0 || outputFilename == null)
            {
                Console.WriteLine("ERROR. Usage: SimionLogToOfflineTraining " + inputFolderArg + "<dir> " + numSamplesArg + "<numTuples> " + outputFileArg + "<outputFile>");
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

            int numDesiredSamplesPerFile = numSamples / numFiles;

            //We use the first descriptor as the common descriptor used for tuples. We are assuming they all have the same variables. BEWARE!!
            Log.Descriptor commonDescriptor = new Log.Descriptor(logDescriptorFiles[0]);
            Tuple sample = new Tuple(commonDescriptor);

            using (FileStream outputStream = File.Create(outputFilename))
            {
                BinaryWriter writer = new BinaryWriter(outputStream);

                Console.WriteLine("STARTED: Drawing " + numSamples + " samples from log files in folder " + inputFolder);
                foreach (string logDescriptorFilename in logDescriptorFiles)
                {
                    Log.Descriptor logDescriptor = new Log.Descriptor(logDescriptorFilename);
                    string folder = Path.GetDirectoryName(logDescriptorFilename);

                    Log.Data data = new Log.Data();
                    data.Load(folder + "\\" + logDescriptor.BinaryLogFile);
                    if (data.SuccessfulLoad)
                    {
                        if (sample.State.Length + sample.Action.Length + sample.Reward.Length ==
                            logDescriptor.StateVariables.Count + logDescriptor.ActionVariables.Count + logDescriptor.RewardVariables.Count)
                        {
                            int totalNumSamples = GetTotalNumSamples(data);
                            int actualNumSamples = Math.Min(totalNumSamples - 1, numDesiredSamplesPerFile);

                            double percentSampled = 100 * ((double)actualNumSamples / (double)totalNumSamples);
                            Console.Write("Log file " + Path.GetFileName(logDescriptorFilename) + ":");
                            for (int i = 0; i < actualNumSamples; i++)
                            {
                                if (sample.DrawRandomSample(data))
                                    sample.SaveToFile(writer);
                            }
                            Console.WriteLine(string.Format(" sampled ({0:0.00}%)", percentSampled));
                        }
                        else
                            Console.WriteLine("File " + logDescriptorFilename + "skipped (missmatched variables in log)");
                    }
                }
                Console.WriteLine("FINISHED: All " + numSamples + " samples were saved to " + outputFilename);
            }
        }
    }
}