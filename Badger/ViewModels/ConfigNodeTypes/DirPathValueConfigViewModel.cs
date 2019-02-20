using System.Xml;
using System.IO;
using System.Windows.Forms;

using Herd.Files;

namespace Badger.ViewModels
{
    class DirPathValueConfigViewModel : ConfigNodeViewModel
    {
        public DirPathValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
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
            DirPathValueConfigViewModel newInstance =
                new DirPathValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }


        public override bool Validate()
        {
            return Directory.Exists(content);
        }


        public void selectFolder()
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            if (m_default != "" && Directory.Exists(m_default))
            {
                if (Path.IsPathRooted(m_default))
                    fbd.SelectedPath = m_default;
                else
                    fbd.SelectedPath = Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), content));
            }

            if (fbd.ShowDialog() == DialogResult.OK)
                content = Herd.Utils.GetRelativePathTo(Directory.GetCurrentDirectory(), fbd.SelectedPath);

            content = content;
        }
    }
}
