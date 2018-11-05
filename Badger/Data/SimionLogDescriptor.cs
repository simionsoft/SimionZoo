using Badger.Simion;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.Data
{
    class SimionLogDescriptor
    {
        /// <summary>
        /// This method returns the list of variables in the log file from the log descriptor
        /// in the same order as they are defined in the descriptor
        /// </summary>
        /// <returns></returns>
        static public List<string> LoadLogDescriptor(string logDescriptorFileName)
        {
            List<string> variableList = new List<string>();
            XmlDocument logDescriptor = new XmlDocument();
            if (File.Exists(logDescriptorFileName))
            {
                try
                {
                    logDescriptor.Load(logDescriptorFileName);
                    XmlNode node = logDescriptor.FirstChild;
                    if (node.Name == XMLConfig.descriptorRootNodeName)
                    {
                        foreach (XmlNode child in node.ChildNodes)
                        {
                            if (child.Name == XMLConfig.descriptorStateVarNodeName
                                || child.Name == XMLConfig.descriptorActionVarNodeName
                                || child.Name == XMLConfig.descriptorRewardVarNodeName
                                || child.Name == XMLConfig.descriptorStatVarNodeName)
                            {
                                string variableName = child.InnerText;
                                variableList.Add(variableName);
                            }
                        }
                        //Add manually a variable representing the real time
                        variableList.Add(SimionLog.ExperimentRealTimeVariable);
                        variableList.Add(SimionLog.EpisodeRealTimeVariable);
                    }
                }
                catch (Exception ex)
                {
                    throw new Exception("Error loading log descriptor: " + logDescriptorFileName + ex.Message);
                }
            }
            return variableList;
        }
    }
}
