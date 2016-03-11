using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Models
{
    public class CIntegerValue:CNode
    {
        public string m_value { get; set; }
        public validTypes type { get; set; }
        public string enumClas { get; set; }
        public string defaultValue { get; set; }
        public Boolean isOptional { get; set; }
        public string comment { get; set; }
        public CIntegerValue(string tipo)
        {
            if (tipo == "INTEGER-VALUE")
            {
                type = validTypes.IntergerValue;
            }
            else if (tipo == "STRING-VALUE")
            {
                type = validTypes.StringValue;
            }
            else if (tipo == "DOUBLE-VALUE")
            {
                type = validTypes.DecimalValue;
            }
            else if (tipo == "DIR-PATH-VALUE")
            {
                type = validTypes.DirPathValue;
            }
            else if (tipo == "FILE-PATH-VALUE")
            {
                type = validTypes.FilePathValue;
            }
           
        }
        public CIntegerValue(XmlNode nodo)
        {
            type = validTypes.EnumValue;
            enumClas = nodo.Attributes["Name"].Value;
            if (node.Attributes["Optional"] != null)
                isOptional = Convert.ToBoolean(node.Attributes["Optional"].Value);
            if (node.Attributes["Comment"] != null)
                comment = node.Attributes["Comment"].Value;
        }
        public CIntegerValue(XmlNode node, string n, string value)
        { 
            name = n; 
            m_value = value;
            if(node.Attributes["Default"]!=null)
                defaultValue=node.Attributes["Default"].Value;
            if (node.Attributes["Optional"] != null)
                isOptional = Convert.ToBoolean(node.Attributes["Optional"].Value);
            if (node.Attributes["Comment"] != null)
                comment = node.Attributes["Comment"].Value;
            string tipo = node.Name;
            if(tipo == "INTEGER-VALUE")
            {
                type=validTypes.IntergerValue;
            }
            else if(tipo=="STRING-VALUE")
            {
                type=validTypes.StringValue;
            }
            else if(tipo=="DOUBLE-VALUE")
            {
                type=validTypes.DecimalValue;
            }
            else if(tipo=="DIR-PATH-VALUE")
            {
                type= validTypes.DirPathValue;
            }
            else if(tipo=="FILE-PATH-VALUE")
            {
                type=validTypes.FilePathValue;
            }
            else if(tipo=="ENUM-VALUE")
            {
                type = validTypes.EnumValue;
                enumClas = node.Attributes["Class"].Value;
            }
            
        }
    }
    public enum validTypes
    {
        IntergerValue,
        StringValue,
        DecimalValue,
        FilePathValue,
        DirPathValue,
        EnumValue,
    };
}
