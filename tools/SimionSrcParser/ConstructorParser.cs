using System;
using System.Collections.Generic;
using Simion;

namespace SimionSrcParser
{
    public class Constructor: ParameterizedObject
    {
        public string m_world;

        static ConstructorParameterParser g_parameterParser = new ConstructorParameterParser();

        public void addParameter(IParameter newParameter)
        {
            m_parameters.Add(newParameter);
        }
        public static void addIndentation(ref string definition, int level)
        {
            for (int i = 0; i < level; i++) definition += "  ";
        }
        public Constructor(string className, string paramName, string body, string bodyPrefix)
        {
            m_name = className;
            g_parameterParser.parse(this,body);
        }

        public override string outputXML(int level)
        {
            string output = "";
            for (int i = 0; i < level; i++) output += "  ";
            output += "<" + XMLConfig.classDefinitionNodeTag + " Name=\"" + m_name + "\"";
            if (m_world!=null)
                output+= " " + XMLConfig.worldAttribute + "=\"" + m_world + "\"";
            output += ">\n";
            foreach (IParameter param in m_parameters)
                output+= param.outputXML(level+1);
            output += "</" + XMLConfig.classDefinitionNodeTag + ">\n";
            return output;
        }
    }
}
