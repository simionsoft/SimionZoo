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

using System.Xml;
using System.IO;
using System.Linq;

using Microsoft.Win32;

using Herd.Files;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace Badger.ViewModels
{
    class FilePathValueConfigViewModel: ConfigNodeViewModel
    {
        public FilePathValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            if (configNode == null || configNode[name] == null)
            {
                //default init
                content = definitionNode.Attributes[XMLTags.defaultAttribute].Value;
                textColor = XMLTags.colorDefaultValue;
            }
            else
            {
                //init from config file
                content = configNode[name].InnerText;
            }
        }

        public override ConfigNodeViewModel clone()
        {
            FilePathValueConfigViewModel newInstance =
                new FilePathValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }

        public override bool Validate()
        {
            //this validates files that exist and paths that match the default (for example, if the default value is "", it is valid to leave it unset)
            return content==m_default || File.Exists(content);
        }


        public void SelectFile()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (m_default != "")
            {
                string extension = m_default.Split('.').Last();
                string filter = "File (." + extension + ")|*." + extension + "|All Files (*.*)|*.*";
                openFileDialog.Filter = filter;

                if (!m_default.Contains("*."))
                {
                    openFileDialog.InitialDirectory =
                        Path.GetDirectoryName(Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory()
                        , m_default)));
                }
                else
                {
                    string dirPath = m_default.Split('*').First();
                    if (Directory.Exists(dirPath))
                    {
                        openFileDialog.InitialDirectory =
                            Path.GetDirectoryName(System.IO.Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory()
                                , dirPath)));
                    }
                }
            }
            else
            {
                string filter = "All Files (*.*)|*.*";
                openFileDialog.Filter = filter;
            }

            string selectedFilePath= content;
            if (openFileDialog.ShowDialog()==true)
            {
                selectedFilePath = Herd.Utils.GetRelativePathTo(Directory.GetCurrentDirectory(), openFileDialog.FileName);

                //Outside the project folder??
                if (Path.IsPathRooted(selectedFilePath) || selectedFilePath.StartsWith("../../"))
                {
                    //copy selected file and related files (exception need to be included for sample files with descriptor and binary file)
                    //to a temp folder inside the root folder of the project so that it can be correctly sent to herd agents
                    List<string> relatedFiles = new List<string>();
                    if (selectedFilePath.EndsWith(Extensions.SampleFileDescriptor))
                        relatedFiles.Add(selectedFilePath + ".bin");
                    string dstTempFile = Folders.tempRelativeDir + Path.GetFileName(selectedFilePath);
                    Task.Run(() =>
                    {
                        //copy related files
                        string dstTempFileAux;
                        foreach (string relatedFile in relatedFiles)
                        {
                            dstTempFileAux = Folders.tempRelativeDir + Path.GetFileName(relatedFile);
                            if (File.Exists(dstTempFileAux))
                                File.Delete(dstTempFileAux);
                            File.Copy(relatedFile, dstTempFileAux);
                        }
                        //copy main file    
                        if (File.Exists(dstTempFile))
                            File.Delete(dstTempFile);
                        File.Copy(selectedFilePath, dstTempFile);
                        content = dstTempFile;
                    });
                }
                else content = selectedFilePath;
            }
            else content = content; //force re-validation
        }
    }
}
