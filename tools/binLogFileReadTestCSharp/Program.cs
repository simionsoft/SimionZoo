using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace binLogFileReadTestCSharp
{
    class Program
    {
        private const int HEADER_MAX_SIZE = 16;

        private const int EXPERIMENT_HEADER = 1;
        private const int EPISODE_HEADER = 2;
        private const int STEP_HEADER = 3;
        private const int EPISODE_END_HEADER = 4;
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
        static private void readExperimentLogHeader(BinaryReader logReader, ref int numEpisodes, ref int fileFormatVersion)
        {
            int magicNumber = (int)logReader.ReadInt64();
            fileFormatVersion = (int)logReader.ReadInt64();
            numEpisodes = (int)logReader.ReadInt64();
            byte[] padding = logReader.ReadBytes(sizeof(double) * (HEADER_MAX_SIZE - 3));
        }
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
        static private void readEpisodeHeader(BinaryReader logReader, ref int episodeType
            , ref int episodeIndex, ref int numVariablesLogged)
        {
            int magicNumber = (int)logReader.ReadInt64();
            episodeType = (int)logReader.ReadInt64();
            episodeIndex = (int)logReader.ReadInt64();
            numVariablesLogged = (int)logReader.ReadInt64();

            byte[] padding = logReader.ReadBytes(sizeof(double) * (HEADER_MAX_SIZE - 4));
        }
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

        //this function return whether there is more data to read from the current episode or not
        static private bool readStepHeader(BinaryReader logReader, ref int stepIndex
            , ref double ExperimentRealTime
            , ref double EpisodeSimTime, ref double EpisodeRealTime)
        {
            
            int magicNumber = (int)logReader.ReadInt64();
            stepIndex =(int) logReader.ReadInt64();
            ExperimentRealTime = logReader.ReadDouble();
            EpisodeSimTime = logReader.ReadDouble();
            EpisodeRealTime = logReader.ReadDouble();
            logReader.ReadChars(sizeof(double) * (HEADER_MAX_SIZE - 5));
            if (magicNumber == EPISODE_END_HEADER) return true;
            return false;
        }
        static private byte[] readStepData(BinaryReader logReader, int numVariables)
        {
            return logReader.ReadBytes(sizeof(double) * (int)numVariables);
        }

        static void Main(string[] args)
        {
            int numEpisodes= 0, fileFormatVersion= 0;
            int episodeType = 0, episodeIndex = 0;
            int stepIndex = 0, numVariablesLogged= 0;
            bool bLastStep;
            double expRealTime = 0.0, episodeSimTime = 0.0, episodeRealTime = 0.0;
            FileStream logFile = File.OpenRead("../../experiment-log.bin");
            using (BinaryReader binaryReader= new BinaryReader(logFile))
            {
                readExperimentLogHeader(binaryReader, ref numEpisodes, ref fileFormatVersion);
                for (int i= 0; i<numEpisodes; i++)
                {
                    readEpisodeHeader(binaryReader, ref episodeType,ref episodeIndex
                        , ref numVariablesLogged);

                    bLastStep= readStepHeader(binaryReader, ref stepIndex
                        ,ref expRealTime, ref episodeSimTime, ref episodeRealTime);

                    while(!bLastStep)
                    {
                        readStepData(binaryReader, numVariablesLogged);
                        bLastStep = readStepHeader(binaryReader, ref stepIndex
                                    , ref expRealTime, ref episodeSimTime, ref episodeRealTime);
                    }
                }
            }
            logFile.Close();
        }
    }
}
