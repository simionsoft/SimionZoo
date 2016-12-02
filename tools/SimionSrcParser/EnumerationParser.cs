using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public class Enumeration: IParameter
    {
        string m_name;
        static EnumerationParser enumerationParser= new EnumerationParser();
        List<string> m_enumerationValues= new List<string>();
        Enumeration(string name,string content)
        {
            m_name = name;
            enumerationParser.parse(content, ref m_enumerationValues);
        }
        public string outputXML(int level)
        {
            string output = "";
            return output;
        }
    }
    public class EnumerationParser
    {
        public EnumerationParser()
        {

        }
        public void parse(string content,ref List<string> valueList)
        { }
    }
}
