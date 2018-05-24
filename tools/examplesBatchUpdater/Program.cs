using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using Badger.Simion;

namespace examplesBatchUpdater
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                //process all the experiment files
                IEnumerable<string> experimentFiles = Directory.EnumerateFiles("../experiments"
                    , "*" + SimionFileData.ExperimentExtension
                    , SearchOption.AllDirectories);
                foreach (string file in experimentFiles)
                {
                    updateExperimentFile(file);
                }
            }
            else
            {
                updateExperimentFile(args[0]);
            }
        }
        
        static void updateExperimentFile(string filename)
        {
            Console.WriteLine("Updating experiment :" + filename);
            XmlDocument experimentXML = new XmlDocument();
            experimentXML.Load(filename);
            //save a backup
            experimentXML.Save(filename + ".old");

            //run all the updates from the file's version onwards
            int version = getExperimentFileVersion(experimentXML);
            for (int updater= version+1; updater<updaters.Count; updater++)
            {
                if (updaters[updater](experimentXML))
                    setVersion(experimentXML, updater);
            }
            experimentXML.Save(filename);
        }
        static public int getExperimentFileVersion(XmlDocument experimentDoc)
        {
            XmlNode root= experimentDoc.FirstChild;
            if (root.Attributes["FileVersion"]!=null)
            {
                return int.Parse(root.Attributes["FileVersion"].Value);
            }
            return -1;
        }

        private delegate bool updateXMLConfigFile(XmlDocument XMLConfigFile);
        static List<updateXMLConfigFile> updaters = new List<updateXMLConfigFile>()
        {update0 , update1};

        static bool update0(XmlDocument XMLConfigFile)
        {
            return true;
        }
        static bool update1(XmlDocument XMLConfigFile)
        {
            //we move the first state-feature-map below SimGod to be global
            XmlNode root = XMLConfigFile.FirstChild;
            XmlNodeList stateFeatureMapNodes = XMLConfigFile.GetElementsByTagName("State-Feature-Map");
            XmlNodeList simGodNodes = XMLConfigFile.GetElementsByTagName("SimGod");
            if (stateFeatureMapNodes.Count != 0 && simGodNodes.Count != 0)
            {
                //move the first appeareance to all the simion nodes
                foreach (XmlNode simgodNode in simGodNodes)
                    simgodNode.PrependChild(stateFeatureMapNodes[0]);
                //remove the rest
                for (int i = 1; i < stateFeatureMapNodes.Count; i++)
                    stateFeatureMapNodes[i].ParentNode.RemoveChild(stateFeatureMapNodes[i]);
            }
            else return false;
            //we remove the reward node, it's now hard-coded inside the dynamic model
            XmlNodeList rewardNodes = XMLConfigFile.GetElementsByTagName("Reward");
            if (rewardNodes.Count>=0)
                rewardNodes[0].ParentNode.RemoveChild(rewardNodes[0]);
            return true;
        }

        static void setVersion(XmlDocument XMLConfigFile, int version)
        {
            XmlNode root = XMLConfigFile.FirstChild;
            XmlAttribute versionAttribute;

            if (root.Attributes["FileVersion"] == null)
            {
                versionAttribute = XMLConfigFile.CreateAttribute("FileVersion");
            }
            else
            {
                versionAttribute = root.Attributes["FileVersion"];
            }
            versionAttribute.Value = version.ToString();
            root.Attributes.SetNamedItem(versionAttribute);
        }

    }
}
