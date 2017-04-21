using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using Badger.ViewModels;
using System.Windows.Forms;
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

        public delegate void logFunction(string message);
        public delegate void XmlNodeFunction(XmlNode node);

        /// <summary>
        ///     Load experiment batch file.
        /// </summary>
        /// <param name="perExperimentFunction"></param>
        /// <param name="batchFilename"></param>
        static public void loadExperimentBatch(XmlNodeFunction perExperimentFunction, string batchFilename = "")
        {
            if (batchFilename == "")
            {
                OpenFileDialog ofd = new OpenFileDialog();
                ofd.Filter = "Experiment batch | *." + XMLConfig.experimentBatchExtension;
                ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");

                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    batchFilename = ofd.FileName;
                }
                else return;
            }

            // LOAD THE EXPERIMENT BATCH IN THE QUEUE
            XmlDocument batchDoc = new XmlDocument();
            batchDoc.Load(batchFilename);
            XmlElement fileRoot = batchDoc.DocumentElement;

            if (fileRoot.Name == XMLConfig.experimentBatchNodeTag)
            {
                foreach (XmlNode experiment in fileRoot.ChildNodes)
                {
                    if (experiment.Name == XMLConfig.experimentNodeTag)
                        perExperimentFunction(experiment);
                }
            }
            else CaliburnUtility.showWarningDialog("Malformed XML in experiment queue file. No badger node.", "ERROR");
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
        public static List<ExperimentalUnit> SaveExperimentBatchFile(BindableCollection<ExperimentViewModel> experiments,
            ref string batchFilename, logFunction log)
        {
            List<ExperimentalUnit> experimentBatch = new List<ExperimentalUnit>();

            if (experiments.Count == 0)
                return null;

            //Validate the experiments
            foreach (ExperimentViewModel experiment in experiments)
            {
                if (!experiment.validate())
                {
                    CaliburnUtility.showWarningDialog("The configuration couldn't be validated in " + experiment.name
                        + ". Please check it", "VALIDATION ERROR");
                    return experimentBatch;
                }
            }

            //Dialog window asking for an output name
            if (batchFilename == "")
            {
                //Save dialog -> returns the experiment batch file
                SaveFileDialog sfd = new SaveFileDialog();
                sfd.Filter = "Experiment batch | *." + XMLConfig.experimentBatchExtension;
                sfd.InitialDirectory = experimentRelativeDir;
                string CombinedPath = Path.Combine(Directory.GetCurrentDirectory(), experimentRelativeDir);
                if (!Directory.Exists(CombinedPath))
                    Directory.CreateDirectory(CombinedPath);
                sfd.InitialDirectory = Path.GetFullPath(CombinedPath);

                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    batchFilename = sfd.FileName;
                }
                else
                {
                    log("Error saving the experiment queue");
                    return null;
                }
            }

            string batchFileDir = batchFilename.Split('.')[0];
            batchFileDir = Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), batchFileDir);
            // Clean output directory if it exists
            if (Directory.Exists(batchFileDir))
            {
                try { Directory.Delete(batchFileDir, true); }
                catch
                {
                    CaliburnUtility.showWarningDialog("It has not been possible to remove the directory: "
                        + batchFileDir + ". Make sure that it's not been using for other app.", "ERROR");
                    log("Error saving the experiment queue");
                    return null;
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
                        batchFileWriter.WriteLine("\t<" + XMLConfig.experimentNodeTag + " " + XMLConfig.nameAttribute
                            + "=\"" + experimentViewModel.name + "\">");

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
                            batchFileWriter.WriteLine("\t\t<" + XMLConfig.experimentalUnitNodeTag
                                + " " + XMLConfig.nameAttribute + "=\"" + experimentName
                                + "\" " + XMLConfig.pathAttribute + "=\"" + filePath + "\">");
                            // Write fork values in between
                            experimentViewModel.saveToStream(batchFileWriter, SaveMode.ForkValues, "\t");
                            // Close 'EXPERIMENTAL-UNIT' tag
                            batchFileWriter.WriteLine("\t\t</" + XMLConfig.experimentalUnitNodeTag + ">");

                            // Add this experimental unit to the output list
                            experimentBatch.Add(new ExperimentalUnit(experimentName, filePath, experimentViewModel.getExeFilename(),
                                experimentViewModel.getPrerrequisites()));
                        }
                        // Close 'EXPERIMENT' tag
                        batchFileWriter.WriteLine("\t</" + XMLConfig.experimentNodeTag + ">");
                    }
                    // Write batch file footer (i.e. close 'EXPERIMENT-BATCH' tag)
                    batchFileWriter.WriteLine("</" + XMLConfig.experimentBatchNodeTag + ">");
                    log("Succesfully saved " + experiments.Count + " experiments");
                }
            }

            return experimentBatch;
        }

        //BADGER project: LOAD
        static public void loadExperiments(WindowViewModel parentWindow,
            ref BindableCollection<ExperimentViewModel> experiments,
            Dictionary<string, string> appDefinitions, logFunction log)
        {
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Badger project | *." + XMLConfig.badgerProjectExtension;
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else return;

            XmlDocument badgerDoc = new XmlDocument();
            badgerDoc.Load(fileDoc);
            XmlElement fileRoot = badgerDoc.DocumentElement;
            if (fileRoot.Name != XMLConfig.badgerNodeTag)
            {
                CaliburnUtility.showWarningDialog("Malformed XML in experiment badger project file.", "ERROR");
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
                    CaliburnUtility.showWarningDialog("Malformed XML in experiment queue file.", "ERROR");
                    log("ERROR: malformed XML in experiment queue file.");
                }
            }
        }
        //BADGER project: SAVE
        static public void saveExperiments(BindableCollection<ExperimentViewModel> experiments)
        {
            foreach (ExperimentViewModel experiment in experiments)
            {
                if (!experiment.validate())
                {
                    CaliburnUtility.showWarningDialog("The configuration couldn't be validated in " + experiment.name
                        + ". Please check it", "VALIDATION ERROR");
                    return;
                }
            }
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Badger project | *." + XMLConfig.badgerProjectExtension;
            string CombinedPath = Path.Combine(Directory.GetCurrentDirectory(), experimentRelativeDir);
            if (!Directory.Exists(CombinedPath))
                Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = Path.GetFullPath(CombinedPath);
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
        static public ExperimentViewModel LoadExperiment(WindowViewModel parentWindow,
            Dictionary<string, string> appDefinitions)
        {
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Experiment | *." + XMLConfig.experimentExtension;
            ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), "experiments");

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                fileDoc = ofd.FileName;
            }
            else return null;

            //open the config file to retrive the app's name before loading it
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
                CaliburnUtility.showWarningDialog("The configuration couldn't be validated in " + experiment.name
                    + ". Please check it", "VALIDATION ERROR");
                return;
            }

            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment | *." + XMLConfig.experimentExtension;
            sfd.InitialDirectory = experimentRelativeDir;
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), experimentRelativeDir);
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath);

            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                experiment.save(sfd.FileName, SaveMode.AsExperiment);
            }
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
    }
}
