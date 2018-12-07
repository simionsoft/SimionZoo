using System;
using System.Collections.Generic;
using System.Xml;
using System.IO;

namespace Herd.Files
{
    public class ExperimentBatch
    {
        public List<Experiment> Experiments = new List<Experiment>();

        public string FileName = null;

        public ExperimentBatch()
        {

        }

        public void Load(string batchFilename, LoadOptions loadOptions)
        {
            Experiments.Clear();
            FileName = batchFilename;
            try
            {
                XmlDocument batchDoc = new XmlDocument();
                batchDoc.Load(batchFilename);
                XmlElement fileRoot = batchDoc.DocumentElement;

                if (fileRoot != null && fileRoot.Name == XMLTags.ExperimentBatchNodeTag)
                {
                    foreach (XmlNode experiment in fileRoot.ChildNodes)
                    {
                        if (experiment.Name == XMLTags.ExperimentNodeTag)
                            Experiments.Add(new Experiment(experiment, Utils.GetDirectory(batchFilename), loadOptions));
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        /// <summary>
        /// This method counts the number of experimental units loaded. Load() must be called before!!
        /// </summary>
        /// <returns></returns>
        public int CountExperimentalUnits()
        {
            int counter = 0;

            foreach(Experiment exp in Experiments)
                counter += exp.ExperimentalUnits.Count;

            return counter;
        }

        /// <summary>
        /// This method loads the batch file and deletes any log file found
        /// </summary>
        /// <param name="batchFilename"></param>
        /// <returns></returns>
        public static int DeleteLogFiles(string batchFilename)
        {
            int counter = 0;
            try
            {
                XmlDocument batchDoc = new XmlDocument();
                batchDoc.Load(batchFilename);
                XmlElement fileRoot = batchDoc.DocumentElement;

                if (fileRoot != null && fileRoot.Name == XMLTags.ExperimentBatchNodeTag)
                {
                    foreach (XmlNode experiment in fileRoot.ChildNodes)
                    {
                        if (experiment.Name == XMLTags.ExperimentNodeTag)
                        {
                            foreach(XmlNode child in experiment.ChildNodes)
                            {
                                if (child.Name == XMLTags.ExperimentalUnitNodeTag)
                                {
                                    string expUnitPath = child.Attributes[XMLTags.pathAttribute].Value;
                                    string baseDirectory = Utils.GetDirectory(batchFilename);
                                    string logDescPath = Utils.GetLogFilePath(baseDirectory + expUnitPath, true);
                                    string logPath = Utils.GetLogFilePath(baseDirectory + expUnitPath, false);
                                    if (File.Exists(logDescPath))
                                    {
                                        counter++;
                                        File.Delete(logDescPath);
                                    }
                                    if (File.Exists(logPath))
                                    {
                                        counter++;
                                        File.Delete(logPath);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return counter;
        }


        /// <summary>
        /// This method loads the experiment batch file and counts the type of experimental units required: All, only finished, or only unfinished
        /// </summary>
        /// <param name="batchFilename"></param>
        /// <param name="selection"></param>
        /// <returns></returns>
        public static int CountExperimentalUnits(string batchFilename, LoadOptions.ExpUnitSelection selection)
        {
            int counter = 0;
            try
            {
                XmlDocument batchDoc = new XmlDocument();
                batchDoc.Load(batchFilename);
                XmlElement fileRoot = batchDoc.DocumentElement;

                if (fileRoot != null && fileRoot.Name == XMLTags.ExperimentBatchNodeTag)
                {
                    foreach (XmlNode experiment in fileRoot.ChildNodes)
                    {
                        if (experiment.Name == XMLTags.ExperimentNodeTag)
                        {
                            foreach (XmlNode child in experiment.ChildNodes)
                            {
                                if (child.Name == XMLTags.ExperimentalUnitNodeTag)
                                {
                                    if (selection == LoadOptions.ExpUnitSelection.All) counter++;
                                    else
                                    {                                 
                                        string expUnitPath = child.Attributes[XMLTags.pathAttribute].Value;
                                        string baseDirectory = Utils.GetDirectory(batchFilename);
                                        string logDescPath = Utils.GetLogFilePath(baseDirectory + expUnitPath, true);
                                        string logPath = Utils.GetLogFilePath(baseDirectory + expUnitPath, false);
                                        bool logExists = File.Exists(logDescPath) && File.Exists(logPath);

                                        if (logExists == (selection==LoadOptions.ExpUnitSelection.OnlyFinished))
                                            counter++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return counter;
        }
    }
}
