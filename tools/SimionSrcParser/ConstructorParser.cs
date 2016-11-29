using System;
using System.Collections.Generic;
using Simion;

namespace SimionSrcParser
{
    public class Constructor
    {
        static ParameterParser g_parameterParser= new ParameterParser();
        private string m_className;
        List<string> m_baseClassConstructors = new List<string>();
        List<Parameter> m_parameters = new List<Parameter>();
        bool m_bDefinesWorldClass;
        public Constructor(string className, string paramName, string body, string bodyPrefix)
        {
            m_className = className;
            g_parameterParser.parse(body, ref m_parameters);

            if (m_parameters.Exists(p => p.definesWorldClass()))
                m_bDefinesWorldClass = true;
            else m_bDefinesWorldClass = false;
        }
        public string outputXML(int level)
        {
            string output = "";
            for (int i = 0; i < level; i++) output += "  ";
            output += "<" + XMLConfig.classDefinitionNodeTag + " Name=\"" + m_className + ">\n";
            foreach (Parameter param in m_parameters)
                output+= param.outputXML(level+1);
            output += "</" + XMLConfig.classDefinitionNodeTag + ">\n";
            return output;
        }
    }
    public class Factory
    {
        private string m_className;
        private string m_comment;
        private List<Choice> m_choices;
    }
}
