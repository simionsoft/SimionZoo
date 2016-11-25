using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public class Constructor
    {
        static ParameterParser g_parameterParser= new ParameterParser();
        private string m_className;
        List<string> m_baseClassConstructors = new List<string>();
        List<Parameter> m_parameters = new List<Parameter>();
        public Constructor(string className, string paramName, string body, string bodyPrefix)
        {
            m_className = className;
            g_parameterParser.parse(body, ref m_parameters);
        }
    }
    public class Factory
    {
        private string m_className;
        private string m_comment;
        private List<Choice> m_choices;
    }
}
