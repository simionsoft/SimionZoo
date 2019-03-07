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
using System.Drawing;
using System.Xml;
using System.IO;
using System.Windows.Media.Imaging;
using System.Windows.Media;

using Caliburn.Micro;

using Herd.Files;

namespace Badger.ViewModels
{
    class FunctionLogViewModel: Screen
    {
        private Image m_functionImage= null;
        public Image FunctionImage { get { return m_functionImage; } set { m_functionImage = value; NotifyOfPropertyChange(() => FunctionImage); } }

        FunctionLog m_functionLog= new FunctionLog();

        BitmapImage BitmapToImageSource(Bitmap bitmap)
        {
            if (bitmap == null) return null;

            using (MemoryStream memory = new MemoryStream())
            {
                bitmap.Save(memory, System.Drawing.Imaging.ImageFormat.Bmp);
                memory.Position = 0;
                BitmapImage bitmapimage = new BitmapImage();
                bitmapimage.BeginInit();
                bitmapimage.StreamSource = memory;
                bitmapimage.CacheOption = BitmapCacheOption.OnLoad;
                bitmapimage.EndInit();

                return bitmapimage;
            }
        }
        void AllNotifies()
        {
            NotifyOfPropertyChange(() => CurrentBitmap);
            NotifyOfPropertyChange(() => CurrentFunctionAsString);
            NotifyOfPropertyChange(() => CurrentSampleAsString);
        }
        private int m_currentFunction = 0;
        private int m_currentSample = 0;
        public ImageSource CurrentBitmap
        {
            get
            {
                FunctionSample f = CurrentFunctionSample; if (f != null)
                {
                    return BitmapToImageSource(f.Sample);
                }
                return null;
            }
        }

        public string CurrentFunctionAsString
        {
            get
            {
                if (m_currentFunction < m_functionLog.Functions.Count)
                    return m_functionLog.Functions[m_currentFunction].Name;
                return null;
            }
        }
        public string CurrentSampleAsString
        {
            get
            {
                FunctionSample f = CurrentFunctionSample;
                if (f != null)
                {
                    return m_currentSample + " (Ep. " + f.Episode + ")";
                }
                return null;
            }
        }
        public FunctionSample CurrentFunctionSample
        {
            get
            {
                if (m_currentFunction < m_functionLog.Functions.Count && m_currentSample < m_functionLog.Functions[m_currentFunction].Samples.Count)
                    return m_functionLog.Functions[m_currentFunction].Samples[m_currentSample];

                return null;
            }
        }
        /// <summary>
        /// Shows the first sample of the next function in the log
        /// </summary>
        public void NextFunction()
        {
            m_currentFunction = (++m_currentFunction) % m_functionLog.Functions.Count;
            m_currentSample = 0;
            AllNotifies();
        }
        /// <summary>
        /// Shows the next sample of the current function
        /// </summary>
        public void NextSample()
        {
            m_currentSample = (++m_currentSample) % m_functionLog.Functions[m_currentFunction].Samples.Count;
            AllNotifies();
        }
        /// <summary>
        /// Shows the first sample of the previous the function in the log
        /// </summary>
        public void PreviousFunction()
        {
            m_currentFunction = Math.Max(0, --m_currentFunction);
            m_currentSample = 0;
            AllNotifies();
        }
        /// <summary>
        /// Shows the previous the sample of the current function
        /// </summary>
        public void PreviousSample()
        {
            m_currentSample = Math.Max(0, --m_currentSample);
            AllNotifies();
        }
        string m_outputDirectory;
        /// <summary>
        /// Exports all the function samples, each one in a different "png" file
        /// </summary>
        public void ExportAll()
        {
            m_outputDirectory = Files.SelectOutputDirectoryDialog(m_outputDirectory);
            foreach (Function f in m_functionLog.Functions)
            {
                foreach (FunctionSample s in f.Samples)
                {
                    //.png by default
                    s.Sample.Save(m_outputDirectory + "/" + f.Name + "." + s.Episode + ".png");
                }
            }

        }

        public FunctionLogViewModel(string logDescriptorFileName)
        {
            XmlDocument logDescriptorDoc = new XmlDocument();
            if (File.Exists(logDescriptorFileName))
            {
                try
                {
                    logDescriptorDoc.Load(logDescriptorFileName);
                    XmlNode node = logDescriptorDoc.FirstChild;
                    string functionLogFileName;
                    if (node.Name == XMLTags.descriptorRootNodeName)
                    {
                        m_outputDirectory = Herd.Utils.GetDirectory(logDescriptorFileName);
                        functionLogFileName = node.Attributes[XMLTags.descriptorFunctionsDataFile].Value;
                        m_functionLog.Load(m_outputDirectory + functionLogFileName);
                        AllNotifies();
                    }
                }
                catch
                {
                    throw new Exception("Error loading log descriptor: " + logDescriptorDoc);
                }
            }

            m_currentFunction = 0;
            m_currentSample = 0;
        }
    }
}
