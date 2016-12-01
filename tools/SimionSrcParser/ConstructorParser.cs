using System;
using System.Collections.Generic;
using Simion;

namespace SimionSrcParser
{
    public abstract class ParameterContainer
    {
        protected string m_className;
        public string m_world;
        protected List<Parameter> m_parameters = new List<Parameter>();

        public void addParameter(Parameter newParameter)
        {
            m_parameters.Add(newParameter);
        }
    }
    public class Constructor: ParameterContainer
    {
        static ParameterParser g_parameterParser= new ParameterParser();

        public Constructor(string className, string paramName, string body, string bodyPrefix)
        {
            m_className = className;
            g_parameterParser.parse(this,body);
        }

        public string outputXML(int level)
        {
            string output = "";
            for (int i = 0; i < level; i++) output += "  ";
            output += "<" + XMLConfig.classDefinitionNodeTag + " Name=\"" + m_className + "\"";
            if (m_world!=null)
                output+= " " + XMLConfig.worldAttribute + "=\"" + m_world + "\"";
            output += ">\n";
            foreach (Parameter param in m_parameters)
                output+= param.outputXML(level+1);
            output += "</" + XMLConfig.classDefinitionNodeTag + ">\n";
            return output;
        }
    }
    public class Factory: ParameterContainer
    {
        private string m_className;
        private string m_comment;
        private List<Choice> m_choices;
    }
}
