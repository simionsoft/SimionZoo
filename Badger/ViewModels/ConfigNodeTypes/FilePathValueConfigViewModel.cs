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
            return File.Exists(content);
        }


        public void selectFile()
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

                if (openFileDialog.ShowDialog()==true)
                {
                    content = Herd.Utils.GetRelativePathTo(Directory.GetCurrentDirectory(), openFileDialog.FileName);
                }
                content = content;
            }
        }
    }
}
