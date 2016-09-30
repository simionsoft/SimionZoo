using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class IntegerValueConfigViewModel: ConfigNode
    {
        public IntegerValueConfigViewModel(XmlNode definition)
        {
            name = definition.Attributes["Name"].Value;


        }
    }
}
