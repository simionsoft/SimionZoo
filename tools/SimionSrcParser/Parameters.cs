using System;
using Simion;

namespace SimionSrcParser
{
 
    public abstract class Parameter
    {
        public abstract string outputXML(int level);
        public abstract bool definesWorldClass()
    }
    public class SimpleParameter: Parameter
    {
        protected string m_xmlTag;
        protected string m_name;
        protected string m_comment;
        protected string m_default;

        public override string outputXML(int level)
        {
            string output= "";
            for (int i = 0; i < level; i++) output += "  ";
            output += "<" + m_xmlTag + " Name=\"" + m_name + "\" Comment=\"" + m_comment + "\" Default=\"" + m_default + "\">\n";
            return output;
        }
        public override bool definesWorldClass()
        {
            return false;
        }
    }
    public class IntParameter: SimpleParameter
    {
        public IntParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLConfig.integerNodeTag;
        }
    }
    public class DoubleParameter : SimpleParameter
    {
        public DoubleParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLConfig.doubleNodeTag;
        }
    }
    public class BoolParameter : SimpleParameter
    {
        public BoolParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLConfig.boolNodeTag;
        }
    }
    public class StringParameter : SimpleParameter
    {
        public StringParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLConfig.stringNodeTag;
        }
    }
    public class FilePathParameter : SimpleParameter
    {
        public FilePathParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLConfig.filePathNodeTag;
        }
    }
    public class DirPathParameter : SimpleParameter
    {
        public DirPathParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLConfig.dirPathNodeTag;
        }
    }
}
