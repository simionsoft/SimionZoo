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
        protected List<IParameter> m_parameters = new List<IParameter>();
        public abstract string outputXML(int level);
    }
}
