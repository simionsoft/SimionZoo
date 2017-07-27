using Badger.Simion;
using System.Xml;
using System.IO;
using System.Linq;
using Microsoft.Win32;

namespace Badger.ViewModels
{
    class FilePathValueConfigViewModel: ConfigNodeViewModel
    {
        public FilePathValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(parentExperiment, parent, definitionNode, parentXPath);

            if (configNode == null || configNode[name] == null)
            {
                //default init
                content = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
                textColor = XMLConfig.colorDefaultValue;
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

        public override bool validate()
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
                    content = Data.Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), openFileDialog.FileName);
                }
                content = content;
            }
        }
    }
}
