using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace SimionSrcParser
{
    public enum ParameterType { INT_PARAM, DOUBLE_PARAM, BOOL_PARAM };
    public class Parameter
    {
        protected ParameterType type;
        protected string name;
        protected string outXmlTag;
        protected string inSrcTemplateName;
        protected Dictionary<string, string> properties;
    }
    public class ParameterParser
    {
        public string outXmlTag;
        public string inSrcTemplateName;

        ParameterParser()
        {
        }
        public void parse(string code, ref List<Parameter> outParameterList)
        { }
        public void output(FileStream outStream)
        { }
    }
}
