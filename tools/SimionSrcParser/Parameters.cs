using System;
using Simion;

namespace SimionSrcParser
{
 
    public abstract class Parameter
    {
        public abstract string outputXML(int level);
        public abstract bool definesWorldClass();
        public static void addIndentation(ref string definition,int level)
        {
            for (int i = 0; i < level; i++) definition += "  ";
        }
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
            addIndentation(ref output, level);
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

    public class ChildObjectParameter : Parameter
    {
        string m_className, m_objectName, m_comment, m_badgerInfo;
        bool m_bOptional;
        public ChildObjectParameter(string className, string objectName, string comment, bool optional, string badgerInfo)
        {
            m_className = className;
            m_objectName = objectName;
            m_comment = comment;
            m_badgerInfo = badgerInfo;
            m_bOptional = optional;
        }
        public override string outputXML(int level)
        {
            string output = "";
            addIndentation(ref output, level);
            output += "<" + XMLConfig.branchNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className 
                + "\" " + XMLConfig.nameAttribute + "=\"" + m_objectName + "\" " + XMLConfig.commentAttribute 
                + "=\"" + m_comment + "\"";
            if (m_badgerInfo != "")
                output += " " + XMLConfig.badgerInfoAttribute + "=\"" + m_badgerInfo + "\"";
            output+= ">\n";
            return output;
        }
        public override bool definesWorldClass()
        {
            return false;
        }
    }
    public class ChildObjectFactoryParameter : Parameter
    {
        string m_className, m_objectName, m_comment, m_badgerInfo;
        bool m_bOptional;
        public ChildObjectFactoryParameter(string className, string objectName, string comment, bool optional, string badgerInfo)
        {
            m_className = className;
            m_objectName = objectName;
            m_comment = comment;
            m_badgerInfo = badgerInfo;
            m_bOptional = optional;
        }
        public override string outputXML(int level)
        {
            string output = "";
            addIndentation(ref output, level);
            output += "<" + XMLConfig.branchNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className
                + "-Factory\" " + XMLConfig.nameAttribute + "=\"" + m_objectName + "\" " + XMLConfig.commentAttribute
                + "=\"" + m_comment + "\"";
            if (m_badgerInfo != "")
                output += " " + XMLConfig.badgerInfoAttribute + "=\"" + m_badgerInfo + "\"";
            output += ">\n";
            return output;
        }
        public override bool definesWorldClass() {return false;}
    }
    public class VarRefParameter : Parameter
    {
        protected string m_name;
        protected string m_comment;
        protected VarType m_type;
        public enum VarType{StateVariable,ActionVariable };
        public VarRefParameter(VarType type,string name, string comment)
        {
            m_type = type;
            m_name = name;
            m_comment = comment;
        }
        public override bool definesWorldClass() { return false; }
        public override string outputXML(int level)
        {
            string output = "";
            addIndentation(ref output, level);
            if (m_type == VarType.StateVariable) output += "<" + XMLConfig.stateVarRefTag;
            else output+= "<" + XMLConfig.actionVarRefTag;
            output += " " + XMLConfig.nameAttribute + "=\"" + m_name + " " + XMLConfig.commentAttribute + "=\"" + m_comment + "\">\n";
            return output;
        }
    }
    public class StateVarRefParameter : VarRefParameter
    {
        public StateVarRefParameter(string name, string comment): base(VarType.StateVariable,name,comment){}
    }
    public class ActionVarRefParameter : VarRefParameter
    {
        public ActionVarRefParameter(string name, string comment) : base(VarType.ActionVariable, name, comment) { }
    }
}
