using System;
using System.Collections.Generic;
using Badger.Simion;

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
            output += "<" + XMLConfig.enumDefinitionNodeTag + " " + XMLConfig.nameAttribute + "=\"" + m_name + "\">\n";
            output+= outputChildrenXML(level + 1);
            foreach (string value in m_enumerationValues)
            {
                SimionSrcParser.addIndentation(ref output, level + 1);
                output += "<" + XMLConfig.enumNodeTag + ">" + value + "</" + XMLConfig.enumNodeTag + ">\n";
            }
            SimionSrcParser.addIndentation(ref output, level);
            output += "</" + XMLConfig.enumDefinitionNodeTag + ">\n";
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
