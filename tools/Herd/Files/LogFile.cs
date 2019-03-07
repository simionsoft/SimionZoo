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
using System.Xml;

namespace Herd.Files
{
    public class Log
    {
        /// <summary>
        /// This method returns the list of variables in the log file from the log descriptor
        /// in the same order as they are defined in the descriptor
        /// </summary>
        /// <returns>The list of variables read from the log descriptor</returns>
        static public List<string> LoadLogDescriptor(string logDescriptorFileName)
        {
            List<string> variableList = new List<string>();
            XmlDocument logDescriptor = new XmlDocument();
            if (File.Exists(logDescriptorFileName))
            {
                try
                {
                    logDescriptor.Load(logDescriptorFileName);
                    XmlNode node = logDescriptor.FirstChild;
                    if (node.Name == XMLTags.descriptorRootNodeName)
                    {
                        foreach (XmlNode child in node.ChildNodes)
                        {
                            if (child.Name == XMLTags.descriptorStateVarNodeName
                                || child.Name == XMLTags.descriptorActionVarNodeName
                                || child.Name == XMLTags.descriptorRewardVarNodeName
                                || child.Name == XMLTags.descriptorStatVarNodeName)
                            {
                                string variableName = child.InnerText;
                                variableList.Add(variableName);
                            }
                        }
                        //Add manually a variable representing the real time
                        variableList.Add(SimionLog.ExperimentRealTimeVariable);
                        variableList.Add(SimionLog.EpisodeRealTimeVariable);
                    }
                }
                catch (Exception ex)
                {
                    throw new Exception("Error loading log descriptor: " + logDescriptorFileName + ex.Message);
                }
            }
            return variableList;
        }

        public class StepData
        {
            public int stepIndex;
            public double expRealTime, episodeSimTime, episodeRealTime;
            public double[] data;

            /// <summary>
            /// Reads the data from a single step from the log file
            /// </summary>
            /// <param name="logReader">The log reader</param>
            /// <param name="numLoggedVariables">The number of variables in the log file</param>
            /// <returns>true if there more steps to read from the current episode, else false</returns>
            public bool ReadStep(BinaryReader logReader, int numLoggedVariables)
            {
                int magicNumber = (int)logReader.ReadInt64();
                stepIndex = (int)logReader.ReadInt64();
                expRealTime = logReader.ReadDouble();
                episodeSimTime = logReader.ReadDouble();
                episodeRealTime = logReader.ReadDouble();
                logReader.ReadChars(sizeof(double) * (SimionLog.HEADER_MAX_SIZE - 5));
                if (magicNumber == SimionLog.EPISODE_END_HEADER) return true;

                //not the final step, we have to read the logged variables
                byte[] buffer = logReader.ReadBytes(sizeof(double) * (int)numLoggedVariables);
                if (buffer.Length == 0)
                    return true;
                data = new double[numLoggedVariables + 2]; //room for the variables and also the experiment real time/ episode real time
                Buffer.BlockCopy(buffer, 0, data, 0, numLoggedVariables * sizeof(double));
                data[numLoggedVariables] = expRealTime;
                data[numLoggedVariables + 1] = episodeRealTime;
                return false;
            }
        }
        public class EpisodesData
        {
            public const int episodeTypeEvaluation = 0;
            public const int episodeTypeTraining = 1;

            public int type = 0;
            public int index = 0;
            public int subIndex = 0;
            public int numVariablesLogged = 0;
            public List<StepData> steps = new List<StepData>();
            public EpisodesData() { }

            /// <summary>
            /// Reads the episode header.
            /// </summary>
            /// <param name="logReader">The log reader.</param>
            public void ReadEpisodeHeader(BinaryReader logReader)
            {
                int magicNumber = (int)logReader.ReadInt64();
                type = (int)logReader.ReadInt64();
                index = (int)logReader.ReadInt64();
                numVariablesLogged = (int)logReader.ReadInt64();
                subIndex = (int)logReader.ReadInt64();
                byte[] padding = logReader.ReadBytes(sizeof(double) * (SimionLog.HEADER_MAX_SIZE - 5));
            }
        }
        public class SimionLog
        {
            public const string ExperimentRealTimeVariable = "Experiment-RealTime";
            public const string EpisodeRealTimeVariable = "Episode-RealTime";

            //BINARY LOG FILE STUFF: constants, reading methods, etc...
            public const int HEADER_MAX_SIZE = 16;

            public const int EXPERIMENT_HEADER = 1;
            public const int EPISODE_HEADER = 2;
            public const int STEP_HEADER = 3;
            public const int EPISODE_END_HEADER = 4;

            public int TotalNumEpisodes = 0;
            public int NumTrainingEpisodes => TrainingEpisodes.Count;
            public int NumEvaluationEpisodes => EvaluationEpisodes.Count;
            public int NumEpisodesPerEvaluation = 1; //to make things easier, we update this number if we find
            int FileFormatVersion = 0;
            public bool SuccessfulLoad = false; //true if the binary file was correctly loaded

            public List<EpisodesData> EvaluationEpisodes = new List<EpisodesData>();
            public List<EpisodesData> TrainingEpisodes = new List<EpisodesData>();
            public EpisodesData[] Episodes = null;



            /// <summary>
            /// Read the binary log file. To know whether the log information has been succesfully loaded
            /// or not, BinFileLoadSuccess can be checked after calling this method.
            /// </summary>
            /// <returns></returns>
            public bool LoadBinaryLog(string LogFileName)
            {
                try
                {
                    using (FileStream logFile = File.OpenRead(LogFileName))
                    {
                        using (BinaryReader binaryReader = new BinaryReader(logFile))
                        {
                            ReadExperimentLogHeader(binaryReader);
                            Episodes = new EpisodesData[TotalNumEpisodes];

                            for (int i = 0; i < TotalNumEpisodes; i++)
                            {
                                Episodes[i] = new EpisodesData();
                                EpisodesData episodeData = Episodes[i];

                                episodeData.ReadEpisodeHeader(binaryReader);
                                //if we find an episode subindex greater than the current max, we update it
                                //Episode subindex= Episode within an evaluation
                                if (episodeData.subIndex > NumEpisodesPerEvaluation)
                                    NumEpisodesPerEvaluation = episodeData.subIndex;

                                //count evaluation and training episodes
                                if (episodeData.type == 0)
                                    EvaluationEpisodes.Add(episodeData);
                                else
                                    TrainingEpisodes.Add(episodeData);

                                StepData stepData = new StepData();
                                bool bLastStep = stepData.ReadStep(binaryReader, episodeData.numVariablesLogged);

                                while (!bLastStep)
                                {
                                    //we only add the step if it's not the last one
                                    //last steps don't contain any info but the end marker
                                    episodeData.steps.Add(stepData);

                                    stepData = new StepData();
                                    bLastStep = stepData.ReadStep(binaryReader, episodeData.numVariablesLogged);
                                }
                            }
                            SuccessfulLoad = true;
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                    SuccessfulLoad = false;
                }
                return SuccessfulLoad;
            }

            public delegate void StepAction(int auxId, int stepIndex, double value);
            public delegate void ScalarValueAction(double action);
            public delegate double EpisodeFunc(EpisodesData episode, int varIndex);


             private void ReadExperimentLogHeader(BinaryReader logReader)
            {
                int magicNumber = (int)logReader.ReadInt64();
                FileFormatVersion = (int)logReader.ReadInt64();
                TotalNumEpisodes = (int)logReader.ReadInt64();
                byte[] padding = logReader.ReadBytes(sizeof(double) * (SimionLog.HEADER_MAX_SIZE - 3));
            }
        }

        /*
    struct ExperimentHeader
    {
    __int64 magicNumber = EXPERIMENT_HEADER;
    __int64 fileVersion = CLogger::BIN_FILE_VERSION;
    __int64 numEpisodes = 0;

    __int64 padding[HEADER_MAX_SIZE - 3]; //extra space
    ExperimentHeader()
    {
     memset(padding, 0, sizeof(padding));
    }
    };
    */


        /*
        struct EpisodeHeader
        {
        __int64 magicNumber = EPISODE_HEADER;
        __int64 episodeType;
        __int64 episodeIndex;
        __int64 numVariablesLogged;
        //Added in version 2: if the episode belongs to an evaluation, the number of episodes per evaluation might be >1
        //the episodeSubIndex will be in [1..numEpisodesPerEvaluation]
        __int64 episodeSubIndex;

        __int64 padding[HEADER_MAX_SIZE - 5]; //extra space
        EpisodeHeader()
        {
            memset(padding, 0, sizeof(padding));
        }
    };*/

        /*
    struct StepHeader
    {
    __int64 magicNumber = STEP_HEADER;
    __int64 stepIndex;
    double experimentRealTime;
    double episodeSimTime;
    double episodeRealTime;

    __int64 padding[HEADER_MAX_SIZE - 5]; //extra space
    StepHeader()
    {
        memset(padding, 0, sizeof(padding));
    }
    };
        */
    }
}
