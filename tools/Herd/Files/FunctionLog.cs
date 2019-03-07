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
using System.Drawing;
using System.IO;

namespace Herd.Files
{


    //From SimionZoo/logger-functions.cpp:
    //
    //#define FUNCTION_SAMPLE_HEADER 6543
    //#define FUNCTION_DECLARATION_HEADER 5432
    //#define FUNCTION_LOG_FILE_HEADER 4321
    //#define FUNCTION_LOG_FILE_VERSION 1

    //#define MAX_FUNCTION_ID_LENGTH 128

    ////using __int64 to assure C# data-type compatibility

    //struct FunctionLogHeader
    //{
    //    __int64 magicNumber = FUNCTION_LOG_FILE_HEADER;
    //    __int64 fileVersion = FUNCTION_LOG_FILE_VERSION;
    //    __int64 numFunctions = 0;
    //};

    //struct FunctionDeclarationHeader
    //{
    //    __int64 magicNumber = FUNCTION_DECLARATION_HEADER;
    //    __int64 id;
    //    char name[MAX_FUNCTION_ID_LENGTH];
    //    __int64 numSamplesX;
    //    __int64 numSamplesY;
    //    __int64 numSamplesZ;
    //};

    //struct FunctionSampleHeader
    //{
    //    __int64 magicNumber = FUNCTION_SAMPLE_HEADER;
    //    __int64 episode;
    //    __int64 step;
    //    __int64 experimentStep;
    //    __int64 id;
    //};


    public class FunctionSample
    {
        int m_episode, m_step, m_experimentStep, m_id;
        public int Episode { get { return m_episode; } }
        public int Step { get { return m_step; } }
        public int ExperimentStep { get { return m_experimentStep; } }
        public int Id { get { return m_id; } }
        Bitmap m_sample = null;
        public Bitmap Sample { get { return m_sample; } }

        public FunctionSample(BinaryReader binaryReader)
        {
            //struct FunctionSampleHeader
            //{
            //    __int64 magicNumber = FUNCTION_SAMPLE_HEADER;
            //    __int64 episode;
            //    __int64 step;
            //    __int64 experimentStep;
            //    __int64 id;
            //};
            int magicNumber = (int)binaryReader.ReadInt64();
            if (magicNumber != FunctionLog.SAMPLE_HEADER)
                throw new Exception("Incorrect sample header format");
            m_episode = (int)binaryReader.ReadInt64();
            m_step = (int)binaryReader.ReadInt64();
            m_experimentStep = (int)binaryReader.ReadInt64();
            m_id = (int)binaryReader.ReadInt64();
        }
        double[] m_functionData;

        /// <summary>
        /// Reads the data of a function sample with size= sizeX*sizeY
        /// </summary>
        /// <param name="binaryReader">The binary reader</param>
        /// <param name="sizeX">The size x</param>
        /// <param name="sizeY">The size y</param>
        public void ReadData(BinaryReader binaryReader, int sizeX, int sizeY)
        {
            //Read data from file
            m_functionData = new double[sizeX * sizeY];
            for (int i = 0; i < sizeX * sizeY; ++i)
                m_functionData[i] = binaryReader.ReadDouble();
        }

        /// <summary>
        /// Calculates the value range of the function
        /// </summary>
        /// <param name="sizeX">The size x</param>
        /// <param name="sizeY">The size y</param>
        /// <param name="minValue">The minimum value. By reference so that we can use it to calculate the absolute min</param>
        /// <param name="maxValue">The maximum value. By reference so that we can use it to calculate the absolute max</param>
        public void CalculateValueRange(int sizeX, int sizeY, ref double minValue, ref double maxValue)
        {
            for (int i = 0; i < sizeX * sizeY; ++i)
            {
                if (m_functionData[i] < minValue) minValue = m_functionData[i];
                if (m_functionData[i] > maxValue) maxValue = m_functionData[i];
            }
        }
        /// <summary>
        /// This method must be called after reading the sample header and finding the declaration of the function to
        /// request the size of the function.
        /// </summary>
        /// <param name="binaryReader"></param>
        /// <param name="sizeX"></param>
        /// <param name="sizeY"></param>
        public void GenerateBitmap(int sizeX, int sizeY, double minValue, double maxValue)
        {
            //Convert data to bitmap
            m_sample = new Bitmap(sizeX, sizeY, System.Drawing.Imaging.PixelFormat.Format32bppRgb);
            Color color = new Color();
            double minColorR = 1.0;
            double minColorG = 0.0;
            double minColorB = 0.0;
            double maxColorR = 0.0;
            double maxColorG = 0.0;
            double maxColorB = 1.0;
            double u;

            double range = Math.Max(maxValue - minValue, 0.0000001);
            for (int y = 0; y < sizeY; y++)
            {
                for (int x = 0; x < sizeX; x++)
                {
                    u = (m_functionData[y * sizeX + x] - minValue) / range;
                    color = Color.FromArgb((int)(255.0 * (minColorR * (1 - u) + maxColorR * u))
                        , (int)(255.0 * (minColorG * (1 - u) + maxColorG * u))
                        , (int)(255.0 * (minColorB * (1 - u) + maxColorB * u)));
                    m_sample.SetPixel(x, y, color);
                }
            }
        }
    }
    public class Function
    {
        List<FunctionSample> m_samples = new List<FunctionSample>();
        public List<FunctionSample> Samples { get { return m_samples; } set { m_samples = value; } }

        int m_numSamplesX, m_numSamplesY, m_numSamplesZ;
        public int NumSamplesX { get { return m_numSamplesX; } }
        public int NumSamplesY { get { return m_numSamplesY; } }
        public int NumSamplesZ { get { return m_numSamplesZ; } }
        int m_id;
        public int Id { get { return m_id; } }
        string m_name;
        public string Name { get { return m_name; } }
        public Function(int id, string name, int numSamplesX, int numSamplesY, int numSamplesZ)
        {
            m_id = id;
            m_name = name;
            m_numSamplesX = numSamplesX;
            m_numSamplesY = numSamplesY;
            m_numSamplesZ = numSamplesZ;
        }

        /// <summary>
        /// Generates all the bitmaps for all the samples
        /// </summary>
        public void GenerateBitmaps()
        {
            double minValue = double.MaxValue;
            double maxValue = double.MinValue;
            //we calculate the per-function minimum and maximum
            foreach (FunctionSample s in Samples)
            {
                s.CalculateValueRange(NumSamplesX, NumSamplesY, ref minValue, ref maxValue);
            }
            // and then, using them as value ranges, we can generate the bitmaps
            foreach (FunctionSample s in Samples)
            {
                s.GenerateBitmap(NumSamplesX, NumSamplesY, minValue, maxValue);
            }
        }
    }
    public class FunctionLog
    {
        public const int SAMPLE_HEADER = 6543;
        public const int DECLARATION_HEADER = 5432;
        public const int FILE_HEADER = 4321;
        public const int FILE_VERSION = 1;

        public const int MAX_FUNCTION_ID_LENGTH = 128;

        List<Function> m_functions = new List<Function>();
        public List<Function> Functions { get { return m_functions; } set { m_functions = value; } }

        public FunctionLog() { }
        public FunctionLog(string filename)
        {
            Load(filename);
        }


        //struct FunctionLogHeader
        //{
        //    __int64 magicNumber = FUNCTION_LOG_FILE_HEADER;
        //    __int64 fileVersion = FUNCTION_LOG_FILE_VERSION;
        //    __int64 numFunctions = 0;
        //};        

        /// <summary>
        /// Loads the specified function log file
        /// </summary>
        /// <param name="filename">The filename</param>
        public void Load(string filename)
        {
            Functions.Clear();
            using (FileStream logFileStream = File.OpenRead(filename))
            {
                using (BinaryReader binaryReader = new BinaryReader(logFileStream))
                {
                    int magicNumber = (int)binaryReader.ReadInt64();
                    if (magicNumber != FILE_HEADER)
                        throw new Exception("Wrong magic number read in function log");
                    int fileVersion = (int)binaryReader.ReadInt64();
                    if (fileVersion != FILE_VERSION)
                        throw new Exception("Wrong file version read in function log");
                    int numFunctions = (int)binaryReader.ReadInt64();

                    //read all the function declarations
                    for (int i = 0; i < numFunctions; i++)
                    {
                        m_functions.Add(ReadFunctionDeclaration(binaryReader));
                    }

                    //read all the samples
                    while (binaryReader.BaseStream.Position != binaryReader.BaseStream.Length)
                    {
                        //read the header of the sample
                        FunctionSample sample = new FunctionSample(binaryReader);

                        //find the declaration of the function from its id
                        Function function = GetFunctionFromId(sample.Id);

                        //we only support 2-input functions so far
                        if (function != null && function.NumSamplesX > 0
                            && function.NumSamplesY > 0 && function.NumSamplesZ == 1)
                        {
                            //Read the data of the sample and add it to the function
                            sample.ReadData(binaryReader, function.NumSamplesX, function.NumSamplesY);
                            function.Samples.Add(sample);
                        }
                    }
                    //post-process: calculate per-function maximum and minimum and generate bitmaps
                    for (int i = 0; i < numFunctions; i++)
                    {
                        m_functions[i].GenerateBitmaps();
                    }
                }
            }
        }
        /// <summary>
        /// Gets a function from an identifier
        /// </summary>
        /// <param name="id">The identifier</param>
        /// <returns>The function</returns>
        Function GetFunctionFromId(int id)
        {
            foreach (Function function in m_functions)
                if (id == function.Id)
                    return function;
            return null;
        }

        /// <summary>
        /// Reads a function declaration from the function log file
        /// </summary>
        /// <param name="binaryReader">The binary reader</param>
        /// <returns>The function object with the parameters read</returns>
        Function ReadFunctionDeclaration(BinaryReader binaryReader)
        {
            //struct FunctionDeclarationHeader
            //{
            //    __int64 magicNumber = FUNCTION_DECLARATION_HEADER;
            //    __int64 id;
            //    char name[MAX_FUNCTION_ID_LENGTH];
            //    __int64 numSamplesX;
            //    __int64 numSamplesY;
            //    __int64 numSamplesZ;
            //};
            int magicNumber = (int)binaryReader.ReadInt64();
            if (magicNumber != FunctionLog.DECLARATION_HEADER)
                throw new Exception("Wrong magic number read in function declaration");
            int id = (int)binaryReader.ReadInt64();
            char[] name = binaryReader.ReadChars(FunctionLog.MAX_FUNCTION_ID_LENGTH);
            string nameAsString = new string(name);
            nameAsString = nameAsString.Substring(0, nameAsString.IndexOf('\0'));
            int numSamplesX = (int)binaryReader.ReadInt64();
            int numSamplesY = (int)binaryReader.ReadInt64();
            int numSamplesZ = (int)binaryReader.ReadInt64();
            Function function = new Function(id, nameAsString, numSamplesX, numSamplesY, numSamplesZ);
            return function;

        }
    }
}
