using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using Badger.ViewModels;
using System.Windows.Forms;
using Badger.Properties;
using Badger.Simion;
using Caliburn.Micro;


namespace Badger.Data
{
    public static class SimionFileData
    {
        public const string binDir = "bin";
        public const string experimentDir = "experiments";
        public const string imageDir = "images";
        public const string appConfigRelativeDir = "..\\config\\apps";
        public const string experimentRelativeDir = "..\\" + experimentDir;
        public const string imageRelativeDir = "..\\" + imageDir;
        public const string badgerLogFile = "badger-log.txt";

        public delegate void LogFunction(string message);
        public delegate void XmlNodeFunction(XmlNode node);

        /// <summary>
        ///     Load experiment batch file.
        /// </summary>
        /// <param name="perExperimentFunction"></param>
        /// <param name="batchFilename"></param>
        static public string LoadExperimentBatchFile(XmlNodeFunction perExperimentFunction, string batchFilename = "")
        {
            if (string.IsNullOrEmpty(batchFilename))
            {
                bool isOpen = OpenFile(ref batchFilename, Resources.ExperimentBatchFilter, XMLConfig.experimentBatchExtension);
                if (!isOpen)
                    return null;
            }

            // Load the experiment batch in queue
            XmlDocument batchDoc = new XmlDocument();
            batchDoc.Load(batchFilename);
            XmlElement fileRoot = batchDoc.DocumentElement;

            if (fileRoot != null && fileRoot.Name == XMLConfig.experimentBatchNodeTag)
            {
                foreach (XmlNode experiment in fileRoot.ChildNodes)
                {
                    if (experiment.Name == XMLConfig.experimentNodeTag)
                        perExperimentFunction(experiment);
                }
            }
            else
            {
                CaliburnUtility.ShowWarningDialog("Malformed XML in experiment queue file. No badger node.", "ERROR");
                return null;
            }

            return batchFilename;
        }

        /// <summary>
        ///     SAVE EXPERIMENT BATCH: the list of (possibly forked) experiments is saved a as set of experiments
        ///     without forks and a .exp-batch file in the root directory referencing them all and the forks/values
        ///     each one took.
        /// </summary>
        /// <param name="experiments"></param>
        /// <param name="batchFilename"></param>
        /// <param name="log"></param>
        /// <returns></returns>
        public static int SaveExperimentBatchFile(BindableCollection<ExperimentViewModel> experiments,
            ref string batchFilename, LogFunction log)
        {
            int experimentalUnitsCount = 0;

            if (experiments.Count == 0)
                return -1;

            //Validate the experiments
            foreach (ExperimentViewModel experiment in experiments)
            {
                if (!experiment.validate())
                {
                    CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.name
                        + ". Please check it", "VALIDATION ERROR");
                    return -1;
                }
            }

            //Dialog window asking for an output name
            if (batchFilename == "")
            {
                //Save dialog -> returns the experiment batch file
                var sfd = SaveFile(Resources.ExperimentBatchFilter, XMLConfig.experimentBatchExtension);
                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    batchFilename = sfd.FileName;
                }
                else
                {
                    log("Error saving the experiment queue");
                    return -1;
                }
            }
            string batchFileDir = batchFilename.Remove( batchFilename.LastIndexOf("." + XMLConfig.experimentBatchExtension));
            batchFileDir = Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), batchFileDir);
            // Clean output directory if it exists
            if (Directory.Exists(batchFileDir))
            {
                try { Directory.Delete(batchFileDir, true); }
                catch
                {
                    CaliburnUtility.ShowWarningDialog("It has not been possible to remove the directory: "
                        + batchFileDir + ". Make sure that it's not been using for other app.", "ERROR");
                    log("Error saving the experiment queue");
                    return -1;
                }
            }

            using (FileStream batchFile = File.Create(batchFileDir + "." + XMLConfig.experimentBatchExtension))
            {
                using (StreamWriter batchFileWriter = new StreamWriter(batchFile))
                {
                    // Write batch file header (i.e. open 'EXPERIMENT-BATCH' tag)
                    batchFileWriter.WriteLine("<" + XMLConfig.experimentBatchNodeTag + ">");

                    foreach (ExperimentViewModel experimentViewModel in experiments)
                    {
                        batchFileWriter.WriteLine("\t<" + XMLConfig.experimentNodeTag + " "
                            + XMLConfig.nameAttribute + "=\"" + experimentViewModel.name + "\" "
                            + XMLConfig.ExeFileNameAttr + "=\"" + experimentViewModel.getExeFilename() + "\">");

                        foreach (var prerequisite in experimentViewModel.getPrerrequisites())
                        {
                            batchFileWriter.Write("\t\t<" + XMLConfig.PrerequisiteTag + " "
                               + XMLConfig.valueAttribute + "=\"" + prerequisite + "\"");
                            //add the rename attribute
                            if (experimentViewModel.renameRules.ContainsKey(prerequisite))
                                batchFileWriter.Write(" " + XMLConfig.renameAttr 
                                    + "=\"" + experimentViewModel.renameRules[prerequisite] + "\"");
                            batchFileWriter.WriteLine("/>");
                        }

                        // Save the fork hierarchy and values. This helps to generate reports easier
                        experimentViewModel.saveToStream(batchFileWriter, SaveMode.ForkHierarchy, "\t");

                        int numCombinations = experimentViewModel.getNumForkCombinations();
                        for (int i = 0; i < numCombinations; i++)
                        {
                            // Save the combination of forks as a new experiment
                            string experimentName = experimentViewModel.setForkCombination(i);

                            string folderPath = batchFileDir + "\\" + experimentName;
                            Directory.CreateDirectory(folderPath);
                            string filePath = folderPath + "\\" + experimentName + "." + XMLConfig.experimentExtension;
                            experimentViewModel.save(filePath, SaveMode.AsExperimentalUnit);

                            // Save the experiment reference in the root batch file. Open 'EXPERIMENTAL-UNIT' tag
                            // with its corresponding attributes.
                            batchFileWriter.WriteLine("\t\t<" + XMLConfig.experimentalUnitNodeTag + " "
                                + XMLConfig.nameAttribute + "=\"" + experimentName + "\" "
                                + XMLConfig.pathAttribute + "=\"" + filePath + "\">");
                            // Write fork values in between
                            experimentViewModel.saveToStream(batchFileWriter, SaveMode.ForkValues, "\t");
                            // Close 'EXPERIMENTAL-UNIT' tag
                            batchFileWriter.WriteLine("\t\t</" + XMLConfig.experimentalUnitNodeTag + ">");
                        }

                        experimentalUnitsCount += numCombinations;
                        // Close 'EXPERIMENT' tag
                        batchFileWriter.WriteLine("\t</" + XMLConfig.experimentNodeTag + ">");
                    }
                    // Write batch file footer (i.e. close 'EXPERIMENT-BATCH' tag)
                    batchFileWriter.WriteLine("</" + XMLConfig.experimentBatchNodeTag + ">");
                    log("Succesfully saved " + experiments.Count + " experiments");
                }
            }

            return experimentalUnitsCount;
        }


        //BADGER project: LOAD
        static public void loadExperiments(MainWindowViewModel parentWindow,
            ref BindableCollection<ExperimentViewModel> experiments,
            Dictionary<string, string> appDefinitions, LogFunction log)
        {
            string fileDoc = null;
            bool isOpen = OpenFile(ref fileDoc, Resources.BadgerProjectFilter, XMLConfig.badgerProjectExtension);
            if (!isOpen)
                return;

            XmlDocument badgerDoc = new XmlDocument();
            badgerDoc.Load(fileDoc);
            XmlElement fileRoot = badgerDoc.DocumentElement;
            if (fileRoot.Name != XMLConfig.badgerNodeTag)
            {
                CaliburnUtility.ShowWarningDialog("Malformed XML in experiment badger project file.", "ERROR");
                log("ERROR: malformed XML in experiment badger project file.");
                return;
            }

            XmlNode configNode;
            foreach (XmlNode experiment in fileRoot.ChildNodes)
            {
                if (experiment.Name == XMLConfig.experimentNodeTag && experiment.ChildNodes.Count > 0)
                {
                    configNode = experiment.FirstChild;
                    experiments.Add(new ExperimentViewModel(parentWindow, appDefinitions[configNode.Name],
                        configNode, experiment.Attributes[XMLConfig.nameAttribute].Value));
                }
                else
                {
                    CaliburnUtility.ShowWarningDialog("Malformed XML in experiment queue file.", "ERROR");
                    log("ERROR: malformed XML in experiment queue file.");
                }
            }
        }

        /// <summary>
        ///     Save multiple experiments as a Badger project. Useful when we need to load a bunch
        ///     of experiments later.
        /// </summary>
        /// <param name="experiments"></param>
        static public void SaveExperiments(BindableCollection<ExperimentViewModel> experiments)
        {
            foreach (ExperimentViewModel experiment in experiments)
            {
                if (!experiment.validate())
                {
                    CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.name
                        + ". Please check it", "VALIDATION ERROR");
                    return;
                }
            }

            var sfd = SaveFile(Resources.BadgerProjectFilter, XMLConfig.badgerProjectExtension);
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                string leftSpace;
                using (FileStream outputFile = File.Create(sfd.FileName))
                {
                    using (StreamWriter writer = new StreamWriter(outputFile))
                    {
                        writer.WriteLine("<" + XMLConfig.badgerNodeTag + " " + XMLConfig.versionAttribute
                            + "=\"" + XMLConfig.BadgerProjectConfigVersion + "\">");
                        leftSpace = "  ";

                        foreach (ExperimentViewModel experiment in experiments)
                        {
                            writer.WriteLine(leftSpace + "<" + XMLConfig.experimentNodeTag
                                + " Name=\"" + experiment.name + "\">");
                            experiment.saveToStream(writer, SaveMode.AsExperiment, leftSpace + "  ");
                            writer.WriteLine(leftSpace + "</" + XMLConfig.experimentNodeTag + ">");
                        }

                        writer.WriteLine("</" + XMLConfig.badgerNodeTag + ">");
                    }
                }
            }
        }

        //EXPERIMENT file: LOAD
        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentWindow"></param>
        /// <param name="appDefinitions"></param>
        /// <returns></returns>
        static public ExperimentViewModel LoadExperiment(MainWindowViewModel parentWindow,
            Dictionary<string, string> appDefinitions)
        {
            string fileDoc = null;
            bool isOpen = OpenFile(ref fileDoc, Resources.ExperimentFilter, XMLConfig.experimentExtension);
            if (!isOpen)
                return null;

            // Open the config file to retrive the app's name before loading it
            XmlDocument configDocument = new XmlDocument();
            configDocument.Load(fileDoc);
            XmlNode rootNode = configDocument.LastChild;
            ExperimentViewModel newExperiment = new ExperimentViewModel(parentWindow, appDefinitions[rootNode.Name], fileDoc);
            return newExperiment;
        }


        static public void SaveExperiment(ExperimentViewModel experiment)
        {
            if (!experiment.validate())
            {
                CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.name
                    + ". Please check it", "VALIDATION ERROR");
                return;
            }

            var sfd = SaveFile(Resources.ExperimentFilter, XMLConfig.experimentExtension);
            if (sfd.ShowDialog() == DialogResult.OK)
                experiment.save(sfd.FileName, SaveMode.AsExperiment);
        }

        static public string getLogDescriptorsFilePath(string logDescriptorFilePath)
        {
            if (logDescriptorFilePath != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                lastPos1 = logDescriptorFilePath.LastIndexOf("/");
                lastPos2 = logDescriptorFilePath.LastIndexOf("\\");

                lastPos = Math.Max(lastPos1, lastPos2);
                if (lastPos > 0)
                {
                    string directory = logDescriptorFilePath.Substring(0, lastPos + 1);

                    return directory + "experiment-log.xml";
                }
            }
            return "";
        }


        static public string getLogFilePath(string experimentConfigFilePath)
        {
            if (experimentConfigFilePath != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                lastPos1 = experimentConfigFilePath.LastIndexOf("/");
                lastPos2 = experimentConfigFilePath.LastIndexOf("\\");

                lastPos = Math.Max(lastPos1, lastPos2);
                if (lastPos > 0)
                {
                    string directory = experimentConfigFilePath.Substring(0, lastPos + 1);

                    return directory + "experiment-log.bin";
                }
            }
            return "";
        }

        /// <summary>
        ///     Show a dialog used to save a file with an specific extension.
        /// </summary>
        /// <param name="filterPrefix"></param>
        /// <param name="extension">The extension of the file</param>
        /// <returns></returns>
        private static SaveFileDialog SaveFile(string filterPrefix, string extension)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = filterPrefix + extension;
            string combinedPath = Path.Combine(Directory.GetCurrentDirectory(), experimentRelativeDir);

            if (!Directory.Exists(combinedPath))
                Directory.CreateDirectory(combinedPath);

            sfd.InitialDirectory = Path.GetFullPath(combinedPath);
            return sfd;
        }

        /// <summary>
        ///     Open a file that fulfills the requirements passed as parameters.
        /// </summary>
        /// <param name="fileName">The name of the file</param>
        /// <param name="filterPrefix"></param>
        /// <param name="extension">The extension of the file</param>
        /// <returns>Wheter the file was successfully open or not</returns>
        public static bool OpenFile(ref string fileName, string filterPrefix, string extension)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = filterPrefix + extension;
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileName = ofd.FileName;
                return true;
            }

            return false;
        }
    }
}
