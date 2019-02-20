using System;
using System.Collections.Generic;

using Herd.Files;

namespace SimionSrcParser
{
    public class Enumeration: ParameterizedObject
    {
        static EnumerationParser enumerationParser= new EnumerationParser();
        List<string> m_enumerationValues= new List<string>();
        public Enumeration(string name,string content)
        {
            m_name = name;
            enumerationParser.parse(content, ref m_enumerationValues);
        }
        public override string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output,level);
            output += "<" + XMLTags.EnumDefinitionNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name + "\">\n";
            output+= outputChildrenXML(level + 1);
            foreach (string value in m_enumerationValues)
            {
                SimionSrcParser.addIndentation(ref output, level + 1);
                output += "<" + XMLTags.enumNodeTag + ">" + value + "</" + XMLTags.enumNodeTag + ">\n";
            }
            SimionSrcParser.addIndentation(ref output, level);
            output += "</" + XMLTags.EnumDefinitionNodeTag + ">\n";
            return output;
        }
    }
    public class EnumerationParser
    {
        public EnumerationParser()
        {

        }
        public void parse(string content,ref List<string> valueList)
        {
            string [] values= content.Split(',');
            foreach (string value in values)
                valueList.Add(value.Trim(' '));
        }
    }
}
