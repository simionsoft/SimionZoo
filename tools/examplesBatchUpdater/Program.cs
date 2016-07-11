using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;

namespace examplesBatchUpdater
{
    class Program
    {
        static void Main(string[] args)
        {
            IEnumerable<string> experimentFiles= Directory.EnumerateFiles("../experiments/examples", "*.experiment", SearchOption.AllDirectories);
            foreach (string file in experimentFiles)
            {
                updateExperimentFile(file);
    
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
                updaters[updater](experimentXML);
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

        private delegate void updateXMLConfigFile(XmlDocument XMLConfigFile);
        static List<updateXMLConfigFile> updaters = new List<updateXMLConfigFile>()
        {update0 /*, update1*/};

        static void update0(XmlDocument XMLConfigFile)
        {
        }
        //static void update1(XmlDocument XMLConfigFile)
        //{ }

        static void setVersion(XmlDocument XMLConfigFile, int version)
        {
            XmlNode root = XMLConfigFile.FirstChild;

            XmlAttribute versionAttribute = XMLConfigFile.CreateAttribute("FileVersion");
            versionAttribute.Value = version.ToString();
            root.Attributes.SetNamedItem(versionAttribute);
        }

    }
}
