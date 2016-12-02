using System;
using Simion;
using System.Collections.Generic;

namespace SimionSrcParser
{
 
    public interface  IParameter
    {
        string outputXML(int level);

    }
    public class SimpleParameter: IParameter
    {
        protected string m_xmlTag;
        protected string m_name;
        protected string m_comment;
        protected string m_default;

        public string outputXML(int level)
        {
            string output= "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + m_xmlTag + " Name=\"" + m_name + "\" Comment=\"" + m_comment + "\" Default=\"" 
                + m_default + "\"/>\n";
            return output;
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

    public class ChildObjectParameter : IParameter
    {
        string m_className, m_objectName, m_comment, m_badgerMetadata;
        bool m_bOptional;
        public ChildObjectParameter(string className, string objectName, string comment, bool optional, string badgerMetadata)
        {
            m_className = className;
            m_objectName = objectName;
            m_comment = comment;
            m_badgerMetadata = badgerMetadata;
            m_bOptional = optional;
        }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + XMLConfig.branchNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className 
                + "\" " + XMLConfig.nameAttribute + "=\"" + m_objectName + "\" " + XMLConfig.commentAttribute 
                + "=\"" + m_comment + "\"";
            if (m_badgerMetadata != "")
                output += " " + XMLConfig.badgerMetadataAttribute + "=\"" + m_badgerMetadata + "\"";
            output+= "/>\n";
            return output;
        }
    }
    public class ChildObjectFactoryParameter : IParameter
    {
        string m_className, m_objectName, m_comment, m_badgerMetadata;
        bool m_bOptional;
        public ChildObjectFactoryParameter(string className, string objectName, string comment, bool optional, string badgerMetadata)
        {
            m_className = className;
            m_objectName = objectName;
            m_comment = comment;
            m_badgerMetadata = badgerMetadata;
            m_bOptional = optional;
        }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + XMLConfig.branchNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className
                + "-Factory\" " + XMLConfig.nameAttribute + "=\"" + m_objectName + "\" " + XMLConfig.commentAttribute
                + "=\"" + m_comment + "\"";
            if (m_badgerMetadata != "")
                output += " " + XMLConfig.badgerMetadataAttribute + "=\"" + m_badgerMetadata + "\"";
            output += "/>\n";
            return output;
        }
    }
    public class VarRefParameter : IParameter
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
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            if (m_type == VarType.StateVariable) output += "<" + XMLConfig.stateVarRefTag;
            else output+= "<" + XMLConfig.actionVarRefTag;
            output += " " + XMLConfig.nameAttribute + "=\"" + m_name + " " + XMLConfig.commentAttribute + "=\"" 
                + m_comment + "\"/>\n";
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
    public class MultiValueParameter : IParameter
    {
        string m_className, m_objectName, m_comment;
        bool m_bOptional;
        public MultiValueParameter(string className, string objectName, string comment, bool optional)
        {
            m_className = className;
            m_objectName = objectName;
            m_comment = comment;
            m_bOptional = optional;
        }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + XMLConfig.multiValuedNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className
                + "\" " + XMLConfig.nameAttribute + "=\"" + m_objectName + "\" " + XMLConfig.commentAttribute
                + "=\"" + m_comment + "\"/>\n";
            return output;
        }
    }
    public class MultiValueFactoryParameter : IParameter
    {
        string m_className, m_objectName, m_comment;
        bool m_bOptional;
        public MultiValueFactoryParameter(string className, string objectName, string comment, bool optional)
        {
            m_className = className;
            m_objectName = objectName;
            m_comment = comment;
            m_bOptional = optional;
        }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + XMLConfig.multiValuedNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className
                + "-Factory\" " + XMLConfig.nameAttribute + "=\"" + m_objectName + "\" " + XMLConfig.commentAttribute
                + "=\"" + m_comment + "\"/>\n";
            return output;
        }
    }
    public class MultiValueSimpleParameter : IParameter
    {
        string m_className, m_objectName, m_comment, m_default;
        
        public MultiValueSimpleParameter(string className, string objectName, string comment, string defaultValue)
        {
            m_className = className;
            m_objectName = objectName;
            m_comment = comment;
            m_default = defaultValue;
        }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + XMLConfig.multiValuedNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className
                + "\" " + XMLConfig.nameAttribute + "=\"" + m_objectName + "\" " + XMLConfig.commentAttribute
                + "=\"" + m_comment + "\" " + XMLConfig.defaultAttribute + "=\"" + m_default + "/>\n";
            return output;
        }
    }
    public class WorldParameter : IParameter
    {
        WorldParser.WorldParameterType m_type;
        string m_name;
        double m_min, m_max;
        string m_unit;
        public WorldParameter(WorldParser.WorldParameterType type, string name, double min,double max,string unit)
        {
            //used for state variables and action variables
            m_type = type;
            m_name = name;
            m_min = min;
            m_max = max;
            m_unit = unit;
        }
        public WorldParameter(WorldParser.WorldParameterType type, string name)
        {
            //used for constants
            m_type = type;
            m_name = name;
        }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            string typeTag;
            switch (m_type)
            {
                case WorldParser.WorldParameterType.StateVariable: typeTag = XMLConfig.stateVarTag;break;
                case WorldParser.WorldParameterType.ActionVariable: typeTag= XMLConfig.actionVarTag; break;
                default: typeTag= XMLConfig.constantTag; break;
            }
            output += "<" + typeTag + " " + XMLConfig.nameAttribute + "=\"" + m_name + "\"";
            if (m_type != WorldParser.WorldParameterType.Constant)
                output += " " + XMLConfig.minValueAttribute
                + "=\"" + m_min + "\" " + XMLConfig.maxValueAttribute + "=\"" + m_max + "\" "
                + XMLConfig.unitAttribute + "=\"" + m_unit + "\"/>\n";
            else output += ">\n";
            return output;
        }
    }
    public class ChoiceParameter : IParameter
    {
        private List<IParameter> m_parameters = new List<IParameter>();
        string m_className, m_name, m_comment;
        public ChoiceParameter(string className, string name, string comment)
        {
            m_className = className;
            m_name = name;
            m_comment = comment;
        }
        public void addParameter(IParameter parameter) { m_parameters.Add(parameter); }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + XMLConfig.choiceNodeTag + " " + XMLConfig.classAttribute + "=\"" + m_className
                + "\" " + XMLConfig.nameAttribute + "=\"" + m_name + "\" " + XMLConfig.commentAttribute
                + "=\"" + m_comment + "\">\n";
            foreach(IParameter parameter in m_parameters)
            {
                output+= parameter.outputXML(level + 1);
            }
            SimionSrcParser.addIndentation(ref output, level);
            output += "</" + XMLConfig.choiceNodeTag + ">";
            return output;
        }
    }
    public class ChoiceElementParameter: IParameter
    {
        public enum Type { New,Factory};
        string m_name, m_className;
        Type m_type;
        string m_badgerMetadata;
        public ChoiceElementParameter(string name,string className,Type type)
        {
            m_name = name;
            m_className = className;
            m_type = type;
        }
        public ChoiceElementParameter(string name, string className, Type type,string badgerMetadata)
        {
            m_name = name;
            m_className = className;
            m_type = type;
            m_badgerMetadata = badgerMetadata;
        }
        public string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            string className = m_className;
            if (m_type == Type.Factory)
                className += "-Factory";
            output += "<" + XMLConfig.choiceElementNodeTag + " " + XMLConfig.classAttribute + "=\"" + className
                + "\" " + XMLConfig.nameAttribute + "=\"" + m_name + "\"";
            if (m_badgerMetadata!=null)
                output+= " " + XMLConfig.badgerMetadataAttribute + "=\"" + m_badgerMetadata + "\"";
            output+= "/>\n";

            return output;
        }
    }
}
