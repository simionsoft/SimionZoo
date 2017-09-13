using System;
using System.Collections.Generic;
using System.IO;

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
        public void readEpisodeHeader(BinaryReader logReader)
        {
            int magicNumber = (int)logReader.ReadInt64();
            type = (int)logReader.ReadInt64();
            index = (int)logReader.ReadInt64();
            numVariablesLogged = (int)logReader.ReadInt64();
            subIndex = (int)logReader.ReadInt64();
            byte[] padding = logReader.ReadBytes(sizeof(double) * (SimionLog.HEADER_MAX_SIZE - 5));
        }
        public static double calculateMin(EpisodesData episode, int varIndex)
        {
            if (varIndex < 0 || episode.steps.Count == 0)
                return 0.0;
            double min = episode.steps[0].data[varIndex];
            foreach (StepData step in episode.steps)
            {
                if (step.data[varIndex] < min)
                    min = step.data[varIndex];
            }
            return min;
        }
        public static double calculateMax(EpisodesData episode, int varIndex)
        {
            if (varIndex < 0 || episode.steps.Count == 0)
                return 0.0;
            double max = episode.steps[0].data[varIndex];
            foreach (StepData step in episode.steps)
            {
                if (step.data[varIndex] > max)
                    max = step.data[varIndex];
            }
            return max;
        }
        public static double calculateVarAvg(EpisodesData episode, int varIndex)
        {
            if (varIndex < 0 || episode.steps.Count == 0)
                return 0.0;
            double avg = 0.0;
            foreach (StepData step in episode.steps)
            {
                avg += step.data[varIndex];
            }
            return avg / episode.steps.Count;
        }
        public static double calculateStdDev(EpisodesData episode, int varIndex)
        {
            if (varIndex < 0 || episode.steps.Count == 0)
                return 0.0;
            double avg = calculateVarAvg(episode, varIndex);
            double sum = 0.0, diff;
            foreach (StepData step in episode.steps)
            {
                diff = step.data[varIndex] - avg;
                sum = diff * diff;
            }
            return Math.Sqrt(sum / episode.steps.Count);
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

        public delegate void StepAction(int auxId, int stepIndex, double value);
        public delegate void ScalarValueAction(double action);
        public delegate double EpisodeFunc(EpisodesData episode, int varIndex);

        public int TotalNumEpisodes = 0;
        public int NumTrainingEpisodes => TrainingEpisodes.Count;
        public int NumEvaluationEpisodes => EvaluationEpisodes.Count;
        public int NumEpisodesPerEvaluation = 1; //to make things easier, we update this number if we find
        public int FileFormatVersion = 0;
        public bool Succesful = true; //true if it finished correctly: the number of episodes in the header must match the number of episodes read from the log file

        public List<EpisodesData> EvaluationEpisodes = new List<EpisodesData>();
        public List<EpisodesData> TrainingEpisodes = new List<EpisodesData>();
        public List<EpisodesData> Episodes = new List<EpisodesData>();

        public bool LoadBinaryLog(string logFilename)
        {
            try
            {
                using (FileStream logFile = File.OpenRead(logFilename))
                {
                    using (BinaryReader binaryReader = new BinaryReader(logFile))
                    {
                        ReadExperimentLogHeader(binaryReader);
                        for (int i = 0; i < TotalNumEpisodes; i++)
                        {
                            EpisodesData episodeData = new EpisodesData();

                            episodeData.readEpisodeHeader(binaryReader);
                            //if we find an episode subindex greater than the current max, we update it
                            //Episode subindex= Episode within an evaluation
                            if (episodeData.subIndex > NumEpisodesPerEvaluation)
                                NumEpisodesPerEvaluation = episodeData.subIndex;

                            //count evaluation and training episodes
                            if (episodeData.type == 0)
                                EvaluationEpisodes.Add(episodeData);
                            else
                                TrainingEpisodes.Add(episodeData);
                            Episodes.Add(episodeData);

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
                    }
                }
            }
            catch (Exception ex)
            {
                Succesful = false;
                Console.WriteLine("Exception reading " + logFilename + "\n" + ex.ToString());
            }
            return Succesful;
        }

        void ReadExperimentLogHeader(BinaryReader logReader)
        {
            int magicNumber = (int)logReader.ReadInt64();
            FileFormatVersion = (int)logReader.ReadInt64();
            TotalNumEpisodes = (int)logReader.ReadInt64();
            byte[] padding = logReader.ReadBytes(sizeof(double) * (SimionLog.HEADER_MAX_SIZE - 3));
        }

        //for each evaluation episode the episodeAction is performed
        public void doForEachEvalEpisode(System.Action<EpisodesData> episodeAction)
        {
            foreach (EpisodesData episode in EvaluationEpisodes)
            {
                if (episode.type == EpisodesData.episodeTypeEvaluation)
                {
                    episodeAction(episode);
                }
            }
        }
        //for each evaluation episode the episodeFunc is performed
        public double doForEpisodeVar(int episodeIndex, int varIndex, SimionLog.EpisodeFunc episodeFunc)
        {
            foreach (EpisodesData episode in EvaluationEpisodes)
            {
                if (episode.index == episodeIndex && episode.subIndex == 1 // <- this obviously is not the right way to do it
                    && episode.type == EpisodesData.episodeTypeEvaluation)
                {
                    return episodeFunc(episode, varIndex);
                }
            }
            return 0.0;
        }
        //for the i-th episode, the action is performed for each step data value
        public void doForEpisodeSteps(int episodeIndex, System.Action<StepData> stepAction)
        {
            foreach (EpisodesData episode in EvaluationEpisodes)
            {
                if (episode.index == episodeIndex && episode.type == EpisodesData.episodeTypeEvaluation)
                {
                    foreach (StepData step in episode.steps)
                        stepAction(step);
                }
            }
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
