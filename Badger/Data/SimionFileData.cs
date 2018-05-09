using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using Badger.ViewModels;
using System.Windows.Forms;
using Badger.Properties;
using Caliburn.Micro;
using Badger.Data;

namespace Badger.Simion
{
    public static class SimionFileData
    {
        public const string logDescriptorExtension = ".log";
        public const string logBinaryExtension = ".log.bin";
        public const string binDir = "bin";
        public const string experimentDir = "experiments";
        public const string imageDir = "images";
        public const string appConfigRelativeDir = "..\\config\\apps";
        public const string experimentRelativeDir = "..\\" + experimentDir;
        public const string imageRelativeDir = "..\\" + imageDir;
        public const string badgerLogFile = "badger-log.txt";
        public const string tempRelativeDir = "..\\temp\\";
        public const string logViewerExePath = "..\\" + binDir + "\\SimionLogViewer.exe";

        public const string ExperimentBatchFilter = "Experiment-batch|*.";
        public const string BadgerProjectFilter = "Badger project|*.";
        public const string ExperimentFilter = "Experiment|*.";

        public delegate void LogFunction(string message);
        public delegate void LoadUpdateFunction();


        //baseDirectory: directory where the batch file is located. Included to allow using paths
        //relative to the batch file, instead of relative to the RLSimion folder structure
        public delegate int XmlNodeFunction(XmlNode node, string baseDirectory,LoadUpdateFunction loadUpdateFunction);

        /// <summary>
        /// This function cleans the log files
        /// </summary>
        /// <param name="node">The node used in each call of the function</param>
        /// <param name="baseDirectory">Base directory</param>
        /// <param name="loadUpdateFunction">Callback function called after processing an experimental unit</param>
        /// <returns></returns>
        public static int CountFinishedExperimentalUnitsInBatch(XmlNode node, string baseDirectory
                , LoadUpdateFunction loadUpdateFunction)
        {
            int finishedExperimentalUnits = 0;
            foreach (XmlNode child in node.ChildNodes)
            {
                if (child.Name == XMLConfig.experimentalUnitNodeTag)
                {
                    string pathToExpUnit = baseDirectory + child.Attributes[XMLConfig.pathAttribute].Value;
                    string pathToLogFile = GetLogFilePath(pathToExpUnit, false);
                    if (File.Exists(pathToLogFile))
                    {
                        FileInfo fileInfo = new FileInfo(pathToLogFile);
                        if (fileInfo.Length > 0)
                            finishedExperimentalUnits++;
                    }
                }
                else
                    finishedExperimentalUnits+= CountFinishedExperimentalUnitsInBatch(child, baseDirectory, loadUpdateFunction);
            }
            return finishedExperimentalUnits;
        }

        /// <summary>
        /// This function cleans the log files
        /// </summary>
        /// <param name="node">The node used in each call of the function</param>
        /// <param name="baseDirectory">Base directory</param>
        /// <param name="loadUpdateFunction">Callback function called after processing an experimental unit</param>
        /// <returns></returns>
        public static int CleanExperimentalUnitLogFiles(XmlNode node, string baseDirectory
                , LoadUpdateFunction loadUpdateFunction)
        {
            int numFilesDeleted = 0;
            foreach (XmlNode child in node.ChildNodes)
            {
                if (child.Name == XMLConfig.experimentalUnitNodeTag)
                {
                    string pathToExpUnit = baseDirectory + child.Attributes[XMLConfig.pathAttribute].Value;
                    string pathToLogFile = GetLogFilePath(pathToExpUnit, false);
                    if (File.Exists(pathToLogFile))
                    {
                        File.Delete(pathToLogFile);
                        numFilesDeleted++;
                    }
                    string pathToLogFileDesc = GetLogFilePath(pathToExpUnit, true);
                    if (File.Exists(pathToLogFileDesc))
                    {
                        File.Delete(pathToLogFileDesc);
                        numFilesDeleted++;
                    }
                }
                else
                    numFilesDeleted+= CleanExperimentalUnitLogFiles(child, baseDirectory, loadUpdateFunction);
            }
            return numFilesDeleted;
        }
        /// <summary>
        /// This function can be passed as an argument to LoadExperimentBatch to quickly count the number
        /// of experimental units in a batch
        /// </summary>
        /// <param name="node">The node used in each call of the function</param>
        /// <param name="baseDirectory">Base directory</param>
        /// <param name="loadUpdateFunction">Callback function called after processing an experimental unit</param>
        /// <returns></returns>
        public static int CountExperimentalUnitsInBatch(XmlNode node, string baseDirectory
                , LoadUpdateFunction loadUpdateFunction)
        {
            int ExperimentalUnitCount = 0;
            foreach (XmlNode child in node.ChildNodes)
            {
                if (child.Name == XMLConfig.experimentalUnitNodeTag)
                    ExperimentalUnitCount++;
                else ExperimentalUnitCount += CountExperimentalUnitsInBatch(child, null, loadUpdateFunction);
            }
            return ExperimentalUnitCount;
        }

        
        /// <summary>
        ///     Load experiment batch file.
        /// </summary>
        /// <param name="perExperimentFunction"></param>
        /// <param name="batchFilename"></param>
        static public int LoadExperimentBatchFile(string batchFilename, XmlNodeFunction perExperimentFunction
            , LoadUpdateFunction onExperimentLoaded = null)
        {
            int retValue = 0;
            // Load the experiment batch in queue
            XmlDocument batchDoc = new XmlDocument();
            batchDoc.Load(batchFilename);
            XmlElement fileRoot = batchDoc.DocumentElement;

            if (fileRoot != null && fileRoot.Name == XMLConfig.experimentBatchNodeTag)
            {
                foreach (XmlNode experiment in fileRoot.ChildNodes)
                {
                    if (experiment.Name == XMLConfig.experimentNodeTag)
                        retValue+= perExperimentFunction(experiment, Utility.GetDirectory(batchFilename), onExperimentLoaded);
                }
            }
            else
            {
                CaliburnUtility.ShowWarningDialog("Malformed XML in experiment queue file. No badger node.", "ERROR");
                return retValue;
            }

            return retValue;
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
                    CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.Name
                        + ". Please check it", "VALIDATION ERROR");
                    return -1;
                }
            }

            //Dialog window asking for an output name
            if (batchFilename == "")
            {
                //Save dialog -> returns the experiment batch file
                var sfd = SaveFile(ExperimentBatchFilter, XMLConfig.experimentBatchExtension);
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
            string batchFileDir = batchFilename.Remove(batchFilename.LastIndexOf("." + XMLConfig.experimentBatchExtension));
            string batchFileName = Utility.GetFilename(batchFileDir);
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
            string fullBatchFileName = batchFileDir + "." + XMLConfig.experimentBatchExtension;

            
            using (StreamWriter batchFileWriter = new StreamWriter(fullBatchFileName))
            {
                // Write batch file header (i.e. open 'EXPERIMENT-BATCH' tag)
                batchFileWriter.WriteLine("<" + XMLConfig.experimentBatchNodeTag + ">");

                foreach (ExperimentViewModel experimentViewModel in experiments)
                {
                    batchFileWriter.WriteLine("\t<" + XMLConfig.experimentNodeTag + " "
                        + XMLConfig.nameAttribute + "=\"" + experimentViewModel.Name + "\">");

                    foreach (AppVersion appVersion in experimentViewModel.AppVersions)
                    {
                        batchFileWriter.WriteLine(appVersion.ToString());
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
                        string relativePathToExperimentalUnit = batchFileName + "\\" + experimentName + "\\" + experimentName + "." + XMLConfig.experimentExtension;

                        // Save the experiment reference in the root batch file. Open 'EXPERIMENTAL-UNIT' tag
                        // with its corresponding attributes.
                        batchFileWriter.WriteLine("\t\t<" + XMLConfig.experimentalUnitNodeTag + " "
                            + XMLConfig.nameAttribute + "=\"" + experimentName + "\" "
                            + XMLConfig.pathAttribute + "=\"" + relativePathToExperimentalUnit + "\">");
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

            return experimentalUnitsCount;
        }


        //BADGER project: LOAD
        static public void loadExperiments(ref BindableCollection<ExperimentViewModel> experiments,
            Dictionary<string, string> appDefinitions, LogFunction log)
        {
            string fileDoc = null;
            bool isOpen = OpenFileDialog(ref fileDoc, BadgerProjectFilter, XMLConfig.badgerProjectExtension);
            if (!isOpen) return;

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
                    experiments.Add(new ExperimentViewModel(appDefinitions[configNode.Name],
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
                    CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.Name
                        + ". Please check it", "VALIDATION ERROR");
                    return;
                }
            }

            var sfd = SaveFile(BadgerProjectFilter, XMLConfig.badgerProjectExtension);
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                string leftSpace;
                using (StreamWriter writer = new StreamWriter(sfd.FileName))
                {
                    writer.WriteLine("<" + XMLConfig.badgerNodeTag + " " + XMLConfig.versionAttribute
                        + "=\"" + XMLConfig.BadgerProjectConfigVersion + "\">");
                    leftSpace = "  ";

                    foreach (ExperimentViewModel experiment in experiments)
                    {
                        writer.WriteLine(leftSpace + "<" + XMLConfig.experimentNodeTag
                            + " Name=\"" + experiment.Name + "\">");
                        experiment.saveToStream(writer, SaveMode.AsExperiment, leftSpace + "  ");
                        writer.WriteLine(leftSpace + "</" + XMLConfig.experimentNodeTag + ">");
                    }

                    writer.WriteLine("</" + XMLConfig.badgerNodeTag + ">");
                }
            }
        }

        //EXPERIMENT file: LOAD
        /// <summary>
        /// 
        /// </summary>
        /// <param name="appDefinitions"></param>
        /// <returns></returns>
        static public ExperimentViewModel LoadExperiment(Dictionary<string, string> appDefinitions)
        {
            string fileDoc = null;
            bool isOpen = OpenFileDialog(ref fileDoc, ExperimentFilter, XMLConfig.experimentExtension);
            if (!isOpen)
                return null;

            // Open the config file to retrive the app's name before loading it
            XmlDocument configDocument = new XmlDocument();
            configDocument.Load(fileDoc);
            XmlNode rootNode = configDocument.LastChild;
            ExperimentViewModel newExperiment = new ExperimentViewModel(appDefinitions[rootNode.Name], fileDoc);
            return newExperiment;
        }


        static public void SaveExperiment(ExperimentViewModel experiment)
        {
            if (!experiment.validate())
            {
                CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.Name
                    + ". Please check it", "VALIDATION ERROR");
                return;
            }

            var sfd = SaveFile(ExperimentFilter, XMLConfig.experimentExtension);
            if (sfd.ShowDialog() == DialogResult.OK)
                experiment.save(sfd.FileName, SaveMode.AsExperiment);
        }



        static public string GetLogFilePath(string experimentFilePath, bool descriptor = true, bool legacyName= false)
        {
            if (experimentFilePath != "")
            {
                if (!legacyName)
                {
                    if (descriptor)
                        return Utility.RemoveExtension(experimentFilePath, 1) + logDescriptorExtension;
                    else
                        return Utility.RemoveExtension(experimentFilePath, 1) + logBinaryExtension;
                }
                else
                {
                    if (descriptor)
                        return Utility.GetDirectory(experimentFilePath) + "experiment-log.xml";
                    else
                        return Utility.GetDirectory(experimentFilePath) + "experiment-log.bin";
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
        public static bool OpenFileDialog(ref string fileName, string filterPrefix, string extension)
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

