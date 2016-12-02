using System;
using System.Collections.Generic;
using Simion;

namespace SimionSrcParser
{
    public class Constructor: ParameterizedObject
    {
        public string m_world;

        static ConstructorParameterParser g_parameterParser = new ConstructorParameterParser();

        public Constructor(string className, string paramName, string body, string bodyPrefix)
        {
            m_name = className;
            g_parameterParser.parse(this,body);
        }

        public override string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output,level);
            output += "<" + XMLConfig.classDefinitionNodeTag + " Name=\"" + m_name + "\"";
            if (m_world!=null)
                output+= " " + XMLConfig.worldAttribute + "=\"" + m_world + "\"";
            output += ">\n";
            output+= outputChildrenXML(level + 1);
            output += "</" + XMLConfig.classDefinitionNodeTag + ">\n";
            return output;
        }
    }
}
