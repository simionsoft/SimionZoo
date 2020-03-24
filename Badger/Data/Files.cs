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
using System.IO;
using System.Xml;
using System.Windows.Forms;

using Caliburn.Micro;

using Herd.Files;

using Badger.ViewModels;
using Badger.Data;

namespace Badger
{
    public static class Files
    {
        public const string ExperimentBatchFilter = "*" + Extensions.ExperimentBatch;
        public const string ProjectFilter = "*" + Extensions.Project;
        public const string ExperimentFilter = "*" + Extensions.Experiment;
        public const string ReportFilter = "*" + Extensions.Report;

        public const string ExperimentFileTypeDescription = "Experiment";
        public const string ExperimentBatchDescription = "Batch";
        public const string ProjectDescription = "Project";
        public const string ReportDescription = "Report";
        public const string ExperimentBatchOrReportFileTypeDescription = "File";

        public const string ExperimentOrProjectFilter = ExperimentFilter + ";" + ProjectFilter;
        public const string ExperimentBatchOrReportFilter= ExperimentBatchFilter + ";" + ReportFilter;

        public delegate void LogFunction(string message);
        public delegate void LoadUpdateFunction();
        public delegate void ProgressUpdateFunction(double progress);


        //baseDirectory: directory where the batch file is located. Included to allow using paths
        //relative to the batch file, instead of relative to the RLSimion folder structure
        public delegate int XmlNodeFunction(XmlNode node, string baseDirectory,LoadUpdateFunction loadUpdateFunction);

        /// <summary>
        ///     Save an experiment batch: the list of (possibly forked) experiments is saved a as set of experiments
        ///     without forks and a .exp-batch file in the root directory referencing them all and the forks/values
        ///     each one took.
        /// </summary>
        /// <param name="experiments"></param>
        /// <param name="batchFilename"></param>
        /// <param name="log"></param>
        /// <returns></returns>
        public static int SaveExperimentBatchFile(BindableCollection<ExperimentViewModel> experiments,
            string batchFilename, LogFunction log, ProgressUpdateFunction progressUpdateFunction)
        {
            //set culture as invariant to write numbers as in english
            System.Threading.Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            int experimentalUnitsCount = 0;

            if (experiments.Count == 0)
                return -1;

            //Validate the experiments
            foreach (ExperimentViewModel experiment in experiments)
            {
                if (!experiment.Validate())
                {
                    Data.CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.Name
                        + ". Please check it", "VALIDATION ERROR");

                    return -1;
                }
                experimentalUnitsCount += experiment.getNumForkCombinations();
            }

            //Dialog window asking for an output name
            if (batchFilename == "")
            {
                //Save dialog -> returns the experiment batch file
                var sfd = SaveFileDialog(ExperimentBatchDescription, ExperimentBatchFilter);
                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    //When using multiple extensions, they are duplicated
                    //To fix this behavior, we remove from the first extension, then add it back
                    if (sfd.FileName.Contains(Extensions.ExperimentBatch))
                        batchFilename = sfd.FileName.Substring(0, sfd.FileName.IndexOf(Extensions.ExperimentBatch)) + Extensions.ExperimentBatch;
                    else
                        batchFilename = sfd.FileName + Extensions.ExperimentBatch;
                }
                else
                {
                    log("Error saving the experiment queue");
                    return -1;
                }
            }
            string relPathToBatchDir = batchFilename.Remove(batchFilename.LastIndexOf(Extensions.ExperimentBatch));
            string bareBatchName = Herd.Utils.GetFilename(relPathToBatchDir);
            relPathToBatchDir = Herd.Utils.GetRelativePathTo(Directory.GetCurrentDirectory(), relPathToBatchDir);
            // Clean output directory if it exists
            if (Directory.Exists(relPathToBatchDir))
            {
                try { Directory.Delete(relPathToBatchDir, true); }
                catch
                {
                    Data.CaliburnUtility.ShowWarningDialog("It has not been possible to remove the directory: "
                        + relPathToBatchDir + ". Make sure that it's not been using for other app.", "ERROR");
                    log("Error saving the experiment queue");
                    return -1;
                }
            }
            string relPathToBatchFile = relPathToBatchDir + Extensions.ExperimentBatch;

            progressUpdateFunction?.Invoke(0.0);
            int numExperimentalUnitsSaved = 0;

            using (StreamWriter batchFileWriter = new StreamWriter(relPathToBatchFile))
            {
                // Write batch file header (i.e. open 'EXPERIMENT-BATCH' tag)
                batchFileWriter.WriteLine("<" + XMLTags.ExperimentBatchNodeTag + ">");

                foreach (ExperimentViewModel experimentViewModel in experiments)
                {
                    batchFileWriter.WriteLine("  <" + XMLTags.ExperimentNodeTag + " "
                        + XMLTags.nameAttribute + "=\"" + experimentViewModel.Name + "\">");

                    foreach (AppVersion appVersion in experimentViewModel.AppVersions)
                    {
                        batchFileWriter.WriteLine(appVersion.ToString());
                    }

                    // Save the fork hierarchy and values. This helps to generate reports easier
                    experimentViewModel.SaveToStream(batchFileWriter, SaveMode.ForkHierarchy, "  ");

                    int numCombinations = experimentViewModel.getNumForkCombinations();
                    for (int i = 0; i < numCombinations; i++)
                    {
                        // Save the combination of forks as a new experiment
                        string experimentName = experimentViewModel.setForkCombination(i);

                        string relPathToExpUnitDir = relPathToBatchDir + "/" + experimentName;
                        Directory.CreateDirectory(relPathToExpUnitDir);
                        string relPathToExpUnitFile = relPathToExpUnitDir + "/" + experimentName + Extensions.Experiment;
                        experimentViewModel.Save(relPathToExpUnitFile, SaveMode.AsExperimentalUnit);
                        string relPathToExpUnitFileFromBatchDir = bareBatchName + "/" + experimentName + "/" + experimentName + Extensions.Experiment;

                        // Save the experiment reference in the root batch file. Open 'EXPERIMENTAL-UNIT' tag
                        // with its corresponding attributes.
                        batchFileWriter.WriteLine("    <" + XMLTags.ExperimentalUnitNodeTag + " "
                            + XMLTags.nameAttribute + "=\"" + experimentName + "\" "
                            + XMLTags.pathAttribute + "=\"" + relPathToExpUnitFileFromBatchDir + "\">");
                        // Write fork values in between
                        experimentViewModel.SaveToStream(batchFileWriter, SaveMode.ForkValues, "\t");
                        // Close 'EXPERIMENTAL-UNIT' tag
                        batchFileWriter.WriteLine("    </" + XMLTags.ExperimentalUnitNodeTag + ">");

                        numExperimentalUnitsSaved++;
                        progressUpdateFunction?.Invoke(numExperimentalUnitsSaved / (double)experimentalUnitsCount);
                    }

                    // Close 'EXPERIMENT' tag
                    batchFileWriter.WriteLine("  </" + XMLTags.ExperimentNodeTag + ">");
                }
                // Write batch file footer (i.e. close 'EXPERIMENT-BATCH' tag)
                batchFileWriter.WriteLine("</" + XMLTags.ExperimentBatchNodeTag + ">");
                log("Succesfully saved " + experiments.Count + " experiments");
            }

            return experimentalUnitsCount;
        }


        /// <summary>
        /// Loads a project in the Editor, creating one experiment view model for every experiment in the project
        /// </summary>
        /// <param name="experiments">The list of experiments where loaded experiments are added</param>
        /// <param name="appDefinitions">The application definitions</param>
        /// <param name="log">The log function used to log messages</param>
        /// <param name="filename">The filename of the input project</param>
        static public void LoadExperiments(ref BindableCollection<ExperimentViewModel> experiments,
            Dictionary<string, string> appDefinitions, LogFunction log, string filename= null)
        {
            if (filename == null)
            {
                bool isOpen = OpenFileDialog(ref filename, ProjectDescription, ProjectFilter);
                if (!isOpen)
                    return;
            }

            XmlDocument badgerDoc = new XmlDocument();
            badgerDoc.Load(filename);
            XmlElement fileRoot = badgerDoc.DocumentElement;
            if (fileRoot.Name != XMLTags.BadgerNodeTag)
            {
                Data.CaliburnUtility.ShowWarningDialog("Malformed XML in experiment badger project file.", "ERROR");
                log("ERROR: malformed XML in experiment badger project file.");
                return;
            }

            XmlNode configNode;
            foreach (XmlNode experiment in fileRoot.ChildNodes)
            {
                if (experiment.Name == XMLTags.ExperimentNodeTag && experiment.ChildNodes.Count > 0)
                {
                    configNode = experiment.FirstChild;
                    experiments.Add(new ExperimentViewModel(appDefinitions[configNode.Name],
                        configNode, experiment.Attributes[XMLTags.nameAttribute].Value));
                }
                else
                {
                    Data.CaliburnUtility.ShowWarningDialog("Malformed XML in experiment queue file.", "ERROR");
                    log("ERROR: malformed XML in experiment queue file.");
                }
            }
        }

        /// <summary>
        /// Saves multiple (potentially forked) experiment view models as a Badger project
        /// </summary>
        /// <param name="experiments">Experiment view models to be saved</param>
        /// <param name="outputFile">Path to the output project file</param>
        static public string SaveExperiments(BindableCollection<ExperimentViewModel> experiments, string outputFile = null)
        {
            //set culture as invariant to write numbers as in english
            System.Threading.Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            foreach (ExperimentViewModel experiment in experiments)
            {
                if (!experiment.Validate())
                {
                    Data.CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.Name
                        + ". Please check it", "VALIDATION ERROR");
                    return null;
                }
            }

            if (outputFile == null)
            {
                var sfd = SaveFileDialog(ProjectDescription, ProjectFilter);
                if (sfd.ShowDialog() == DialogResult.OK)
                    outputFile = sfd.FileName;
            }

            if (outputFile != null) //either because a name was provided when the method was called or because the user selected one from the dialog
            {
                string leftSpace;
                using (StreamWriter writer = new StreamWriter(outputFile))
                {
                    writer.WriteLine("<" + XMLTags.BadgerNodeTag + " " + XMLTags.versionAttribute
                        + "=\"" + XMLTags.BadgerProjectConfigVersion + "\">");
                    leftSpace = "  ";

                    foreach (ExperimentViewModel experiment in experiments)
                    {
                        writer.WriteLine(leftSpace + "<" + XMLTags.ExperimentNodeTag
                            + " Name=\"" + experiment.Name + "\">");
                        experiment.SaveToStream(writer, SaveMode.AsExperiment, leftSpace + "  ");
                        writer.WriteLine(leftSpace + "</" + XMLTags.ExperimentNodeTag + ">");
                    }

                    writer.WriteLine("</" + XMLTags.BadgerNodeTag + ">");
                }
            }
            return outputFile;
        }

        /// <summary>
        /// Loads an experiment view model
        /// </summary>
        /// <param name="appDefinitions">App definitions. Needed to load the experiment correctly</param>
        /// <param name="filename">The path to the input experiment file. If null, a dialgo window will be displayed</param>
        /// <returns></returns>
        static public ExperimentViewModel LoadExperiment(Dictionary<string, string> appDefinitions, string filename= null)
        {
            //set culture as invariant to write numbers as in english
            System.Threading.Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            if (filename == null)
            {
                bool isOpen = OpenFileDialog(ref filename, ExperimentFileTypeDescription, ExperimentFilter);
                if (!isOpen)
                    return null;
            }

            // Open the config file to retrive the app's name before loading it
            XmlDocument configDocument = new XmlDocument();
            configDocument.Load(filename);
            XmlNode rootNode = configDocument.LastChild;
            ExperimentViewModel newExperiment = new ExperimentViewModel(appDefinitions[rootNode.Name], filename);
            return newExperiment;
        }

        /// <summary>
        /// Saves an experiment view model. The output filename will be asked using a dialog window
        /// </summary>
        /// <param name="experiment">The experiment to be saved</param>
        /// <returns>Path to the file saved. Null if something failed</returns>
        static public string SaveExperiment(ExperimentViewModel experiment)
        {
            //set culture as invariant to write numbers as in english
            System.Threading.Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            if (!experiment.Validate())
            {
                Data.CaliburnUtility.ShowWarningDialog("The configuration couldn't be validated in " + experiment.Name
                    + ". Please check it", "VALIDATION ERROR");
                return null;
            }

            var sfd = SaveFileDialog(ExperimentFileTypeDescription, ExperimentFilter);
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                experiment.Save(sfd.FileName, SaveMode.AsExperiment);
                return sfd.FileName;
            }

            return null;
        }


        static string globalOutputDirectory = null;
        static string globalInputDirectory = null;

        public static object Utils { get; internal set; }

        /// <summary>
        ///     Show a dialog used to save a file with an specific extension.
        /// </summary>
        /// <param name="description">Short description of the file type</param>
        /// <param name="filter">Extension</param>
        /// <param name="suggestedFileName">Name suggested for output. Null by default</param>
        /// <returns>Returns the SavedFileDialog structure with all the info</returns>
        public static SaveFileDialog SaveFileDialog(string description, string filter, string suggestedFileName= null)
        {
            SaveFileDialog sfd = new SaveFileDialog();

            sfd.Filter = description + "|" + filter;
            sfd.SupportMultiDottedExtensions = true;

            if (globalInputDirectory == null)
            {
                string initialDirectory = Path.Combine(Directory.GetCurrentDirectory(), Folders.experimentRelativeDir);

                if (!Directory.Exists(initialDirectory))
                    Directory.CreateDirectory(initialDirectory);

                sfd.InitialDirectory = Path.GetFullPath(initialDirectory);
            }
            else
                sfd.InitialDirectory = globalInputDirectory;

            if (suggestedFileName != null)
            {
                globalInputDirectory = Herd.Utils.GetDirectory(sfd.FileName);
                sfd.FileName = suggestedFileName;
            }
            return sfd;
        }

        /// <summary>
        ///     Shows a dialog used to select a directory where files are to be saved. If something goes wrong, null is returned
        /// </summary>
        /// <param name="initialDirectory">The directory from which to being browsing</param>
        /// <returns>The name of the selected folder. Null if something goes wrong</returns>
        public static string SelectOutputDirectoryDialog(string initialDirectory= null)
        {
            
            var folderBrowserDialog = new FolderBrowserDialog();

            folderBrowserDialog.RootFolder = Environment.SpecialFolder.Desktop;

            if (globalOutputDirectory == null)
                folderBrowserDialog.SelectedPath = initialDirectory;
            else
                folderBrowserDialog.SelectedPath = globalOutputDirectory;

            // Show the FolderBrowserDialog.
            DialogResult result = folderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                globalOutputDirectory = folderBrowserDialog.SelectedPath;
                return folderBrowserDialog.SelectedPath;
            }
            return null;
        }

        /// <summary>
        ///     Open a file that fulfills the requirements passed as parameters.
        /// </summary>
        /// <param name="fileName">The name of the file</param>
        /// <param name="description">Description of the file type</param>
        /// <param name="extension">The extension of the file type</param>
        /// <returns>Wheter the file was successfully open or not</returns>
        public static bool OpenFileDialog(ref string fileName, string description, string extension)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = description + "|" + extension;

            if (globalInputDirectory == null)
                ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory()), Folders.experimentDir);
            else
                ofd.InitialDirectory = globalInputDirectory;

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                globalInputDirectory = Herd.Utils.GetDirectory(ofd.FileName);
                fileName = ofd.FileName;
                return true;
            }

            return false;
        }

        /// <summary>
        /// Opens a dialog window to select files of different types
        /// </summary>
        /// <param name="extensions">Extensions (separated by a semicolon</param>
        /// <param name="descriptions">Description for each extension (separated by a semicolon)</param>
        /// <param name="multiSelect">Allow the user to select more than one file?</param>
        /// <returns>A list of the selected files</returns>
        public static List<string> OpenFileDialogMultipleFiles(string extensions, string descriptions, bool multiSelect= true)
        {
            List<string> filenames = new List<string>();
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Multiselect = multiSelect;
            ofd.SupportMultiDottedExtensions = true;
            ofd.Filter = extensions + "|" + descriptions;

            if (globalInputDirectory == null)
            {
                ofd.InitialDirectory = Path.Combine(Path.GetDirectoryName(Directory.GetCurrentDirectory())
                    , Folders.experimentDir);
            }
            else
                ofd.InitialDirectory = globalInputDirectory;

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                foreach (string filename in ofd.FileNames)
                    filenames.Add(filename);
            }

            return filenames;
        }

        /// <summary>
        /// This method saves a set of queries on a experiment batch and their results to some output folder
        /// </summary>
        /// <param name="absBatchFilename">The absolute path to the experiment batch to which the query belongs</param>
        /// <param name="logQueryResults">The set of queries to be saved</param>
        /// <param name="absOutputFolder">Absolute path to the output folder, the name of the output report is taken from the input batch file</param>
        static public void SaveReport(string absBatchFilename, BindableCollection<LogQueryResultViewModel> logQueryResults, string absOutputFolder)
        {
            //save the queries and the report (a set of log query results referencing the queries and the experiment batch)
            string nakedBatchFilename = Herd.Utils.RemoveDirectories(Herd.Utils.RemoveExtension(absBatchFilename, 2));
            string reportFilename = absOutputFolder + "/" + nakedBatchFilename + Extensions.Report;
            using (TextWriter writer = File.CreateText(reportFilename))
            {
                //open the root node in the report
                writer.WriteLine("<" + XMLTags.ReportNodeTag + " " + XMLTags.BatchFilenameAttr + "=\"" + absBatchFilename + "\">");

                foreach (LogQueryResultViewModel logQueryResult in logQueryResults)
                {
                    string nakedLogQueryName = Herd.Utils.RemoveSpecialCharacters(logQueryResult.Name);
                    //Save the reports, each one in a different subfolder
                    string absQueryOutputFolder = absOutputFolder + "/" + nakedLogQueryName;
                    string relOutputFolder = nakedLogQueryName;

                    if (!Directory.Exists(absQueryOutputFolder))
                        Directory.CreateDirectory(absQueryOutputFolder);

                    //Save serializable data
                    string relLogQueryResultFilename = nakedLogQueryName + "/" + nakedLogQueryName + ".xml";
                    string absLogQueryResultFilename = absOutputFolder + "/" + relLogQueryResultFilename;
                    Serialiazer.WriteObject(absLogQueryResultFilename, logQueryResult);

                    //Save non-serializable data
                    Dictionary<string,string> additionalOutputFiles= logQueryResult.ExportNonSerializable(absQueryOutputFolder);

                    //the reference to the query and non-serializable data files
                    writer.WriteLine("  <" + XMLTags.QueryNodeTag + " " + XMLTags.nameAttribute + "=\"" + relLogQueryResultFilename + "\">");

                    foreach(string file in additionalOutputFiles.Keys)
                    {
                        writer.WriteLine("    <" + additionalOutputFiles[file] + ">" + file + "</" + additionalOutputFiles[file] + ">");
                    }
                    writer.WriteLine("  </" + XMLTags.QueryNodeTag + ">");
                }

                //close the root node in the report
                writer.WriteLine("</" + XMLTags.ReportNodeTag + ">");
            }
        }

        /// <summary>
        /// Loads a previously saved report
        /// </summary>
        /// <param name="reportFilename">The report filename</param>
        /// <param name="batchFilename">The batch filename to which the report was generated</param>
        /// <param name="logQueryResults">The loaded log query results</param>
        /// <returns></returns>
        static public int LoadReport(string reportFilename, out string batchFilename, out BindableCollection<LogQueryResultViewModel> logQueryResults)
        {
            logQueryResults = new BindableCollection<LogQueryResultViewModel>();
            XmlDocument reportDoc= new XmlDocument();
            reportDoc.Load(reportFilename);
            XmlNode root= reportDoc.FirstChild;
            batchFilename = null;
            string inputBaseFolder = Herd.Utils.GetDirectory(reportFilename);

            if (root.Name==XMLTags.ReportNodeTag)
            {
                batchFilename = root.Attributes[XMLTags.BatchFilenameAttr].Value;
                
                foreach(XmlNode child in root.ChildNodes)
                {
                    if (child.Name==XMLTags.QueryNodeTag)
                    {
                        LogQueryResultViewModel logQueryResult;
                        string queryFilename = inputBaseFolder + child.Attributes[XMLTags.nameAttribute].Value;
                        
                        if (File.Exists(queryFilename))
                        {
                            logQueryResult = Serialiazer.ReadObject<LogQueryResultViewModel>(queryFilename);
                            logQueryResult.ImportNonSerializable(Herd.Utils.GetDirectory(queryFilename));
                            logQueryResult.IsNotifying = true;
                            logQueryResults.Add(logQueryResult);
                        }
                    }
                }
            }

            return logQueryResults.Count;
        }
    }
}

