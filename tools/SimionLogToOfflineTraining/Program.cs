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

using Herd.Files;
using System;
using System.Collections.Generic;
using System.IO;

namespace SimionLogToOfflineTraining
{
    class Program
    {
        public class Sampler
        {
            public double[] State { get; } = null;
            public double[] Action { get; } = null;
            public double[] State_p { get; } = null;
            public double[] Reward { get; } = null;

            Log.Descriptor m_descriptor;

            public int ElementsPerSample { get { return State.Length + Action.Length + State_p.Length + Reward.Length; } }
            public long SampleSizeInBytes { get { return sizeof(double) * ElementsPerSample; } }

            public Sampler(Log.Descriptor descriptor)
            {
                m_descriptor = descriptor;

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

            public void SaveLastSampleToBinaryFile(BinaryWriter writer)
            {
                double[] sample = new double[ElementsPerSample];
                int bufferOffset = 0;
                for (int i = 0; i < State.Length; i++) sample[bufferOffset + i] = State[i];
                bufferOffset += State.Length;
                for (int i = 0; i < Action.Length; i++) sample[bufferOffset + i] = Action[i];
                bufferOffset += Action.Length;
                for (int i = 0; i < State_p.Length; i++) sample[bufferOffset + i] = State_p[i];
                bufferOffset += State_p.Length;
                for (int i = 0; i < Reward.Length; i++) sample[bufferOffset + i] = Reward[i];
                bufferOffset += Reward.Length;

                byte [] result = new byte[sample.Length * sizeof(double)];
                Buffer.BlockCopy(sample, 0, result, 0, result.Length);
                writer.Write(result, 0, result.Length);
            }

            void SaveVariables(StreamWriter writer, string xmlTag, List<string> names)
            {
                foreach(string name in names)
                    writer.WriteLine("  <" + xmlTag + ">" + name + "</" + xmlTag + ">");
            }

            public void SaveSampleFileDescriptor(string filename, string binaryFilename, int numSamples)
            {
                using (StreamWriter writer = File.CreateText(filename))
                {
                    writer.WriteLine("<" + XMLTags.sampleFiledescriptorRootNodeName + " " + XMLTags.descriptorBinaryDataFile + "=\"" + binaryFilename + "\" " 
                        + XMLTags.sampleFileNumSamplesAttr + "=\"" + numSamples + "\">");
                    SaveVariables(writer, XMLTags.descriptorStateVarNodeName, m_descriptor.StateVariables);
                    SaveVariables(writer, XMLTags.descriptorActionVarNodeName, m_descriptor.ActionVariables);
                    SaveVariables(writer, XMLTags.descriptorRewardVarNodeName, m_descriptor.RewardVariables);
                    writer.WriteLine("</" + XMLTags.sampleFiledescriptorRootNodeName + ">");
                }
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

        static void ShuffleSamplesInFile(string inputFilename, Sampler sampler)
        {
            string outputFilename = inputFilename + ".tmp";
            FileInfo inputFileInfo = new FileInfo(inputFilename);
            long numInputSamples = inputFileInfo.Length / sampler.SampleSizeInBytes;
            Random randomGenerator = new Random();

            byte[] buffer = new byte[sampler.SampleSizeInBytes];

            using (FileStream inputStream = File.OpenRead(inputFilename))
            {
                BinaryReader inputReader = new BinaryReader(inputStream);
                using (FileStream outputStream = File.Create(outputFilename))
                {
                    BinaryWriter writer = new BinaryWriter(outputStream);

                    for (int i= 0; i<numInputSamples; i++)
                    {
                        long sampleIndex = randomGenerator.Next() % numInputSamples;
                        inputStream.Seek(sampleIndex * sampler.SampleSizeInBytes, SeekOrigin.Begin);

                        inputReader.Read(buffer, 0, buffer.Length);
                        writer.Write(buffer, 0, buffer.Length);
                    }
                }
            }
            File.Delete(inputFilename);
            File.Move(outputFilename, inputFilename);
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
            string outputBinaryFilename = null;

            foreach(string arg in args)
            {
                if (arg.StartsWith(inputFolderArg)) inputFolder = arg.Substring(inputFolderArg.Length);
                if (arg.StartsWith(numSamplesArg)) numSamples = int.Parse(arg.Substring(numSamplesArg.Length));
                if (arg.StartsWith(outputFileArg)) outputFilename = arg.Substring(outputFileArg.Length);
            }

            //check all required arguments are set
            if (inputFolder == null || numSamples == 0 || outputFilename == null)
            {
                Console.WriteLine("ERROR. Usage: SimionLogToOfflineTraining " + inputFolderArg + "<dir> " + numSamplesArg + "<numTuples> " + outputFileArg + "<outputFile>");
                return;
            }

            //check/set correct file extensions to output files
            if (!outputFilename.EndsWith(Extensions.SampleFileDescriptor))
                outputFilename += Extensions.SampleFileDescriptor;
            outputBinaryFilename = Herd.Utils.RemoveExtension(outputFilename,2) + Extensions.SampleBinaryFile;

            //Traverse all the log files
            string[] logDescriptorFiles = Directory.GetFiles(inputFolder, "*" + Herd.Files.Extensions.LogDescriptor, SearchOption.AllDirectories);
            int numFiles = logDescriptorFiles.Length;

            if (numFiles == 0)
            {
                Console.WriteLine("ERROR. No log files found in the provided directory: " + inputFolder);
                return;
            }

            int numDesiredSamplesPerFile = numSamples / numFiles;

            //We use the first descriptor as the common descriptor used for tuples. We are assuming they all have the same variables. BEWARE!!
            Log.Descriptor commonDescriptor = new Log.Descriptor(logDescriptorFiles[0]);
            Sampler sampler = new Sampler(commonDescriptor);
            int numSavedSamples = 0;

            using (FileStream outputStream = File.Create(outputBinaryFilename))
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
                        if (sampler.State.Length + sampler.Action.Length + sampler.Reward.Length ==
                            logDescriptor.StateVariables.Count + logDescriptor.ActionVariables.Count + logDescriptor.RewardVariables.Count)
                        {
                            int totalNumSamples = GetTotalNumSamples(data);
                            int actualNumSamples = Math.Min(totalNumSamples - 1, numDesiredSamplesPerFile);

                            double percentSampled = 100 * ((double)actualNumSamples / (double)totalNumSamples);
                            Console.Write("Log file " + Path.GetFileName(logDescriptorFilename) + ":");
                            for (int i = 0; i < actualNumSamples; i++)
                            {
                                if (sampler.DrawRandomSample(data))
                                {
                                    sampler.SaveLastSampleToBinaryFile(writer);
                                    numSavedSamples++;
                                }
                            }
                            Console.WriteLine(string.Format(" sampled ({0:0.00}%)", percentSampled));
                        }
                        else
                            Console.WriteLine("File " + logDescriptorFilename + "skipped (missmatched variables in log)");
                    }
                }
            }
            //Shuffle the samples in the file
            Console.WriteLine("Shuffling samples in file");
            ShuffleSamplesInFile(outputBinaryFilename, sampler);
            //Save the descriptor
            if (numSavedSamples > 0)
            {
                sampler.SaveSampleFileDescriptor(outputFilename, Path.GetFileName(outputBinaryFilename), numSavedSamples);
                Console.WriteLine("FINISHED: " + numSavedSamples + " samples were drawn from the log files and saved\nDescriptor: " + outputFilename + "\nBinary data: " + outputBinaryFilename);
            }

        }
    }
}