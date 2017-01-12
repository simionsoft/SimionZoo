using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data
{

    public class SimionLogFile
    {
        //BINARY LOG FILE STUFF: constants, reading methods, etc...
        public const int HEADER_MAX_SIZE = 16;

        public const int EXPERIMENT_HEADER = 1;
        public const int EPISODE_HEADER = 2;
        public const int STEP_HEADER = 3;
        public const int EPISODE_END_HEADER = 4;

        public delegate void StepAction(int auxId, int stepIndex, double value);
        public delegate void ScalarValueAction(double action);
        public delegate double EpisodeFunc(EpisodeData episode,int varIndex);

        public static ExperimentData load(string logFilename)
        {
            ExperimentData experimentData = new ExperimentData();
            try
            {
                FileStream logFile = File.OpenRead(logFilename);
                using (BinaryReader binaryReader = new BinaryReader(logFile))
                {
                    experimentData.readExperimentLogHeader(binaryReader);
                    for (int i = 0; i < experimentData.numEpisodes; i++)
                    {
                        EpisodeData episodeData = new EpisodeData();
                        experimentData.episodes.Add(episodeData);

                        episodeData.readEpisodeHeader(binaryReader);

                        StepData stepData = new StepData();
                        episodeData.steps.Add(stepData);
                        bool bLastStep = stepData.readStep(binaryReader, episodeData.numVariablesLogged);

                        while (!bLastStep)
                        {
                            stepData = new StepData();
                            episodeData.steps.Add(stepData);
                            bLastStep = stepData.readStep(binaryReader, episodeData.numVariablesLogged);
                        }
                    }
                }
                logFile.Close();

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return experimentData;
        }
    }
 
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
            logReader.ReadChars(sizeof(double) * (SimionLogFile.HEADER_MAX_SIZE - 5));
            if (magicNumber == SimionLogFile.EPISODE_END_HEADER) return true;

            //not the final step, we have to read the logged variables
            byte[] buffer = logReader.ReadBytes(sizeof(double) * (int)numLoggedVariables);
            data = new double[numLoggedVariables];
            Buffer.BlockCopy(buffer, 0, data, 0, numLoggedVariables * sizeof(double));
            return false;
        }
    }
    public class EpisodeData
    {
        public const int episodeTypeEvaluation = 0;
        public const int episodeTypeTraining = 1;

        public int type = 0;
        public int index = 0;
        public int numVariablesLogged = 0;
        public List<StepData> steps = new List<StepData>();
        public void readEpisodeHeader(BinaryReader logReader)
        {
            int magicNumber = (int)logReader.ReadInt64();
            type = (int)logReader.ReadInt64();
            index = (int)logReader.ReadInt64();
            numVariablesLogged = (int)logReader.ReadInt64();

            byte[] padding = logReader.ReadBytes(sizeof(double) * (SimionLogFile.HEADER_MAX_SIZE - 4));
        }
        public static double calculateMin(EpisodeData episode, int varIndex)
        {
            if (varIndex < 0 || episode.steps.Count == 0)
                return 0.0;
            double min = episode.steps[0].data[varIndex];
            foreach (StepData step in episode.steps)
            {
                if (step.data[varIndex]<min)
                    min = step.data[varIndex];
            }
            return min;
        }
        public static double calculateMax(EpisodeData episode, int varIndex)
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
        public static double calculateVarAvg(EpisodeData episode,int varIndex)
        {
            if (varIndex < 0 || episode.steps.Count == 0)
                return 0.0;
            double avg = 0.0;
            foreach(StepData step in episode.steps)
            {
                avg += step.data[varIndex];
            }
            return avg / episode.steps.Count;
        }
        public static double calculateStdDev(EpisodeData episode,int varIndex)
        {
            if (varIndex < 0 || episode.steps.Count == 0)
                return 0.0;
            double avg = calculateVarAvg(episode,varIndex);
            double sum = 0.0, diff;
            foreach(StepData step in episode.steps)
            {
                diff = step.data[varIndex] - avg;
                sum = diff * diff;
            }
            return Math.Sqrt(sum / episode.steps.Count);
        }

        public void doForEachValue(SimionLogFile.ScalarValueAction action,int varIndex)
        {
            foreach (StepData stepData in steps)
            {
                action(stepData.data[varIndex]);
            }
        }
        //actionIndex is used to pass an id to the action. For example, the id of the plot to be used
        public void doForEachValue(SimionLogFile.StepAction action, int actionIndex, int varIndex)
        {
            int stepIndex = 0;
            foreach (StepData stepData in steps)
            {
                action(actionIndex,stepIndex,stepData.data[varIndex]);

            }
        }
    }
    public class ExperimentData
    {
        public int numEpisodes = 0;
        public int fileFormatVersion = 0;

        public List<EpisodeData> episodes = new List<EpisodeData>();

        public void readExperimentLogHeader(BinaryReader logReader)
        {
            int magicNumber = (int)logReader.ReadInt64();
            fileFormatVersion = (int)logReader.ReadInt64();
            numEpisodes = (int)logReader.ReadInt64();
            byte[] padding = logReader.ReadBytes(sizeof(double) * (SimionLogFile.HEADER_MAX_SIZE - 3));
        }

       // public delegate System.Action EpisodeAction(EpisodeData episode,int varIndex);

        //for each evaluation episode the episodeAction is performed
        public void doForEachEvalEpisode(System.Action<EpisodeData> episodeAction)
        {
            foreach (EpisodeData episode in episodes)
            {
                if (episode.type == EpisodeData.episodeTypeEvaluation)
                {
                    episodeAction(episode);
                }
            }
        }
        //for the i-th episode, the action is performed for each step data value
        public void doForEpisodeSteps(int episodeIndex,System.Action<StepData> stepAction)
        {
            foreach (EpisodeData episode in episodes)
            {
                if (episode.index == episodeIndex && episode.type== EpisodeData.episodeTypeEvaluation)
                {
                    foreach(StepData step in episode.steps)
                        stepAction(step);
                }
            }
        }
        //public void doForEachEvalStep(SimionLogFile.ScalarValueAction action, int varIndex)
        //{
        //    foreach (EpisodeData episode in episodes)
        //    {
        //        if (episode.type == EpisodeData.episodeTypeEvaluation)
        //        {
        //            episode.doForEachValue(action,varIndex);
        //        }
        //    }
        //}
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

    __int64 padding[HEADER_MAX_SIZE - 4]; //extra space
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
