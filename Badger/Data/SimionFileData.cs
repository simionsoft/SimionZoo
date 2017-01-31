using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using Badger.ViewModels;
using System.Windows.Forms;
using Simion;
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
        //LOAD EXPERIMENT BATCH
        static public void loadExperimentBatch(XmlNodeFunction nodeFunction, string batchFilename="")
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

            //LOAD THE EXPERIMENT BATCH IN THE QUEUE
            XmlDocument batchDoc = new XmlDocument();
            batchDoc.Load(batchFilename);
            XmlElement fileRoot = batchDoc.DocumentElement;

            if (fileRoot.Name == XMLConfig.batchNodeTag)
            {
                foreach (XmlNode experiment in fileRoot.ChildNodes)
                {
                    if (experiment.Name == XMLConfig.experimentNodeTag)
                    {
                        nodeFunction(experiment);
                    }
                }
            }
            else CaliburnUtility.showWarningDialog("Malformed XML in experiment queue file. No badger node.","ERROR");
            return;
        }
        //SAVE EXPERIMENT BATCH: the list of (possibly forked) experiments is saved a as set of experiments without forks
        public static List<Experiment> saveExperimentBatchFile(BindableCollection<AppViewModel> appViewModelList
            ,ref string batchFilename, logFunction log)
        {
            List<Experiment> experimentBatch = new List<Experiment>();

            if (appViewModelList.Count == 0)
                return null;

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

            //clean output directory if it exists
            string batchFileDir;
            batchFileDir = batchFilename.Split('.')[0];
            batchFileDir = Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), batchFileDir);
            if (Directory.Exists(batchFileDir))
            {
                try
                {
                    Directory.Delete(batchFileDir, true);
                }
                catch
                {
                    CaliburnUtility.showWarningDialog("It has not been possible to remove the directory: "
                        + batchFileDir + ". Make sure that it's not been using for other app."
                        , "ERROR");
                    log("Error saving the experiment queue");
                    return null;
                }
            }

            using (FileStream batchFile = File.Create(batchFileDir + "." + XMLConfig.experimentBatchExtension))
            {
                using (StreamWriter batchFileWriter = new StreamWriter(batchFile))
                {
                    //batch file header
                    batchFileWriter.WriteLine("<" + XMLConfig.batchNodeTag + ">");

                    int numCombinations;
                    string filePath, folderPath;
                    string experimentName;

                    foreach (AppViewModel experiment in appViewModelList)
                    {
                        numCombinations = experiment.getNumForkCombinations();
                        for (int i = 0; i < numCombinations; i++)
                        {

                            //Save the combination of forks as a new experiment
                            experimentName= experiment.setForkCombination(i);
                            
                            folderPath = batchFileDir + "/" + experimentName;
                            Directory.CreateDirectory(folderPath);
                            filePath = folderPath + "/" + experimentName + "." + XMLConfig.experimentExtension;
                            experiment.save(filePath, SaveMode.CombineForks);

                            //Save the experiment reference in the batch file
                            batchFileWriter.WriteLine("<" + XMLConfig.experimentNodeTag + " " + XMLConfig.nameAttribute
                                + "=\"" + experimentName + "\" " + XMLConfig.pathAttribute + "=\"" + filePath + "\"/>");


                            //Add the experiment to the output list
                            experimentBatch.Add(new Experiment(experimentName, filePath, experiment.getExeFilename()
                                , experiment.getPrerrequisites()));
                        }
                    }
                    //batch file footer
                    batchFileWriter.WriteLine("</" + XMLConfig.batchNodeTag + ">");
                    log("Succesfully saved " + appViewModelList.Count + " experiments");
                }
            }

            return experimentBatch;
        }

        //BADGER files: LOAD
        static public void loadExperiments(WindowViewModel parentWindow
            ,ref BindableCollection<AppViewModel> appViewModelList
            ,Dictionary<string,string> appDefinitions
            ,logFunction log)
        {
            string fileDoc = null;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Experiment batch | *." + XMLConfig.badgerExtension;
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
                CaliburnUtility.showWarningDialog("Malformed XML in experiment queue file. No badger node.", "ERROR");
                log("ERROR: malformed XML in experiment queue file. No badger node.");
                return;
            }
            XmlNode configNode;
            foreach (XmlNode experiment in fileRoot.ChildNodes)
            {
                if (experiment.Name == XMLConfig.experimentNodeTag && experiment.ChildNodes.Count > 0)
                {
                    configNode = experiment.FirstChild;
                    appViewModelList.Add(new AppViewModel(parentWindow,appDefinitions[configNode.Name], configNode
                        , experiment.Attributes[XMLConfig.nameAttribute].Value));
                }
                else
                {
                    CaliburnUtility.showWarningDialog("Malformed XML in experiment queue file. No badger node.", "ERROR");
                    log("ERROR: malformed XML in experiment queue file");
                }
            }
        }
        //BADGER files: SAVE
        static public void saveExperiments(BindableCollection<AppViewModel> appViewModelList)
        {
            foreach (AppViewModel app in appViewModelList)
            {
                if (!app.validate())
                {
                    CaliburnUtility.showWarningDialog("The app can't be validated. See error log.", "Error");
                    return;
                }
            }
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment | *." + XMLConfig.badgerExtension;
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
                        writer.WriteLine("<" + XMLConfig.badgerNodeTag +  " " + XMLConfig.versionAttribute 
                            + "=\"" + XMLConfig.BadgerConfigVersion + "\">");
                        leftSpace = "  ";
                        foreach (AppViewModel app in appViewModelList)
                        {
                            writer.WriteLine(leftSpace + "<" + XMLConfig.experimentNodeTag 
                                + " Name=\"" + app.name + "\">");
                            app.saveToStream(writer, SaveMode.SaveForks, leftSpace + "  ");
                            writer.WriteLine(leftSpace + "</" + XMLConfig.experimentNodeTag + ">");
                        }
                        writer.WriteLine("</" + XMLConfig.badgerNodeTag + ">");
                    }
                }
            }
        }

        //EXPERIMENT file: LOAD
        static public AppViewModel loadExperiment(WindowViewModel parentWindow,Dictionary<string,string> appDefinitions)
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
            AppViewModel newApp = new AppViewModel(parentWindow,appDefinitions[rootNode.Name], fileDoc);
            return newApp;
        }

        static public void saveExperiment(AppViewModel experiment)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Experiment | *." + XMLConfig.experimentExtension;
            sfd.InitialDirectory = experimentRelativeDir;
            string CombinedPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), experimentRelativeDir);
            if (!Directory.Exists(CombinedPath))
                System.IO.Directory.CreateDirectory(CombinedPath);
            sfd.InitialDirectory = System.IO.Path.GetFullPath(CombinedPath);
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                experiment.save(sfd.FileName, SaveMode.SaveForks);
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
