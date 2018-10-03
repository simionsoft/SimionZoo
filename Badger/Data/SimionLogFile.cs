using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using Badger.Data;
using Badger.ViewModels;

namespace Badger.Simion
{
    public class StepData
    {
        public int stepIndex;
        public double expRealTime, episodeSimTime, episodeRealTime;
        public double[] data;

        //this function return whether there is more data to read from the current episode or not
        public bool readStep(BinaryReader logReader, int numLoggedVariables)
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
            data = new double[numLoggedVariables];
            Buffer.BlockCopy(buffer, 0, data, 0, numLoggedVariables * sizeof(double));
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
        public void ReadEpisodeHeader(BinaryReader logReader)
        {
            int magicNumber = (int)logReader.ReadInt64();
            type = (int)logReader.ReadInt64();
            index = (int)logReader.ReadInt64();
            numVariablesLogged = (int)logReader.ReadInt64();
            subIndex = (int)logReader.ReadInt64();
            byte[] padding = logReader.ReadBytes(sizeof(double) * (SimionLog.HEADER_MAX_SIZE - 5));
        }
        public Series GetVariableData(int variableIndex, Report trackParameters)
        {

            if (steps[steps.Count - 1].episodeSimTime < trackParameters.MinEpisodeLength)
                return null;

            Series data = new Series();

            foreach (StepData step in steps)
            {
                if (step.episodeSimTime>=trackParameters.TimeOffset)
                    data.AddValue(step.episodeSimTime
                     , ProcessFunc.Get(trackParameters.ProcessFunc, step.data[variableIndex]));
            }

            if (data.Values.Count == 0)
                return null;

            data.CalculateStats(trackParameters);

            if (trackParameters.Resample)
                data.Resample(trackParameters.NumSamples);
            return data;
        }
        public double GetEpisodeAverage(int variableIndex, Report trackParameters)
        {
            double avg = 0.0;
            int count = 0;
            if (steps.Count == 0) return 0.0;
            foreach (StepData step in steps)
            {
                if (step.episodeSimTime >= trackParameters.TimeOffset)
                {
                    avg += ProcessFunc.Get(trackParameters.ProcessFunc, step.data[variableIndex]);
                    count++;
                }
            }
            return avg / count;
        }
    }
    public class SimionLog
    {
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
                            bool bLastStep = stepData.readStep(binaryReader, episodeData.numVariablesLogged);

                            while (!bLastStep)
                            {
                                //we only add the step if it's not the last one
                                //last steps don't contain any info but the end marker
                                episodeData.steps.Add(stepData);

                                stepData = new StepData();
                                bLastStep = stepData.readStep(binaryReader, episodeData.numVariablesLogged);
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

   
        public Series GetEpisodeData(EpisodesData episode, Report trackParameters, int variableIndex)
        {
            return episode.GetVariableData(variableIndex, trackParameters);
        }

        public SeriesGroup GetAveragedData(List<EpisodesData> episodes, Report trackParameters, int variableIndex)
        {
            SeriesGroup data = new SeriesGroup(trackParameters);
            Series xYSeries = new Series();

            foreach (EpisodesData episode in episodes)
            {
                xYSeries.AddValue(episode.index
                    , episode.GetEpisodeAverage(variableIndex, trackParameters));
            }
            xYSeries.CalculateStats(trackParameters);
            if (trackParameters.Resample)
                xYSeries.Resample(trackParameters.NumSamples);
            data.AddSeries(xYSeries);
            return data;
        }

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
