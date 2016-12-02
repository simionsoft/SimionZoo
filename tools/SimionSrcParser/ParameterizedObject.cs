using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public abstract class ParameterizedObject
    {
        protected string m_name;
        private List<IParameter> m_parameters = new List<IParameter>();
        public void addParameter(IParameter parameter) { m_parameters.Add(parameter); }
        
        public virtual string outputXML(int level)
        {
            string output = "";
            foreach (IParameter parameter in m_parameters)
                output += parameter.outputXML(level);
            return output;
        }
    }
}
