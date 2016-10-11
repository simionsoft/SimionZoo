using Simion;
using System.Xml;
using System.IO;
using System.Linq;
using Microsoft.Win32;

namespace Badger.ViewModels
{
    class FilePathValueConfigViewModel: ConfigNodeViewModel
    {
        public FilePathValueConfigViewModel(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, definitionNode, parentXPath);
            comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
            if (configNode == null)
            {
                //default init
                content = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
            }
            else
            {
                //init from config file
                content= configNode[name].InnerText;
            }
        }

        public override bool validate()
        {
            return Directory.Exists(content);
        }

        public override string getXML()
        {
            return "<" + name + ">" + content + "</" + name + ">";
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
