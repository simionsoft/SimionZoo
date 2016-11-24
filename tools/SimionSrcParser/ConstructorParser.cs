using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public class Constructor
    {
        private string m_className;
        List<string> m_baseClassConstructors = new List<string>();
        List<Parameter> m_parameters = new List<Parameter>();
        public Constructor(string className, string paramName, string body, string bodyPrefix)
        {
            m_className = className;
        }
    }
    public class Factory
    {
        private string m_className;
        private string m_comment;
        private List<Choice> m_choices;
    }
}
