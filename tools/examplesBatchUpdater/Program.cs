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
                    , "*" + Herd.Files.Extensions.ExperimentExtension
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

        private delegate bool updateXMLTagsFile(XmlDocument XMLTagsFile);
        static List<updateXMLTagsFile> updaters = new List<updateXMLTagsFile>()
        {update0 , update1};

        static bool update0(XmlDocument XMLTagsFile)
        {
            return true;
        }
        static bool update1(XmlDocument XMLTagsFile)
        {
            //we move the first state-feature-map below SimGod to be global
            XmlNode root = XMLTagsFile.FirstChild;
            XmlNodeList stateFeatureMapNodes = XMLTagsFile.GetElementsByTagName("State-Feature-Map");
            XmlNodeList simGodNodes = XMLTagsFile.GetElementsByTagName("SimGod");
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
            XmlNodeList rewardNodes = XMLTagsFile.GetElementsByTagName("Reward");
            if (rewardNodes.Count>=0)
                rewardNodes[0].ParentNode.RemoveChild(rewardNodes[0]);
            return true;
        }

        static void setVersion(XmlDocument XMLTagsFile, int version)
        {
            XmlNode root = XMLTagsFile.FirstChild;
            XmlAttribute versionAttribute;

            if (root.Attributes["FileVersion"] == null)
            {
                versionAttribute = XMLTagsFile.CreateAttribute("FileVersion");
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
