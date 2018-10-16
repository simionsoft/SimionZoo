using System;
using Caliburn.Micro;
using System.Drawing;
using Badger.Data;
using System.Xml;
using System.IO;
using Badger.Simion;
using System.Windows.Media.Imaging;
using System.Windows.Media;


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
        public void NextFunction()
        {
            m_currentFunction = (++m_currentFunction) % m_functionLog.Functions.Count;
            m_currentSample = 0;
            AllNotifies();
        }
        public void NextSample()
        {
            m_currentSample = (++m_currentSample) % m_functionLog.Functions[m_currentFunction].Samples.Count;
            AllNotifies();
        }
        public void PreviousFunction()
        {
            m_currentFunction = Math.Max(0, --m_currentFunction);
            m_currentSample = 0;
            AllNotifies();
        }
        public void PreviousSample()
        {
            m_currentSample = Math.Max(0, --m_currentSample);
            AllNotifies();
        }
        string m_outputDirectory;
        public void ExportAll()
        {
            m_outputDirectory = SimionFileData.SelectOutputDirectoryDialog(m_outputDirectory);
            foreach (Function f in m_functionLog.Functions)
            {
                foreach (FunctionSample s in f.Samples)
                {
                    //.png by default
                    s.Sample.Save(m_outputDirectory + "\\" + f.Name + "." + s.Episode + ".png");
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
                    if (node.Name == XMLConfig.descriptorRootNodeName)
                    {
                        m_outputDirectory = Utility.GetDirectory(logDescriptorFileName);
                        functionLogFileName = node.Attributes[XMLConfig.descriptorFunctionsDataFile].Value;
                        m_functionLog.Load(m_outputDirectory + functionLogFileName);
                        AllNotifies();
                    }
                }
                catch (Exception ex)
                {
                    throw new Exception("Error loading log descriptor: " + logDescriptorDoc);
                }
            }

            m_currentFunction = 0;
            m_currentSample = 0;
        }
    }
}
