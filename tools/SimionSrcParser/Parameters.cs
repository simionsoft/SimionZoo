/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using System;
using System.Collections.Generic;

using Herd.Files;

namespace SimionSrcParser
{

    public interface IParameter
    {
        string GetParameterType();
        string GetDescription();
        string AsXML(int level);
        int ParameterClassSortValue();
        void SetParameterIndexInCode(int index);
        string GetName();
    }
    public abstract class SimpleParameter : IParameter
    {
        protected string m_xmlTag;
        protected string m_name;
        protected string m_comment;
        protected string m_default;
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueSimpleParameter; } //default value
        public string GetName() { return m_name; }
        public string GetDescription() { return m_comment; }
        public abstract string GetParameterType();
        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + m_xmlTag + " Name=\"" + m_name + "\" Comment=\"" + m_comment + "\" Default=\""
                + m_default + "\"/>\n";
            return output;
        }
    }
    public class IntParameter : SimpleParameter
    {
        public IntParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLTags.integerNodeTag;
        }
        public override string GetParameterType() { return "int"; }
    }
    public class DoubleParameter : SimpleParameter
    {
        public DoubleParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLTags.doubleNodeTag;
        }
        public override string GetParameterType() { return "double"; }
    }
    public class BoolParameter : SimpleParameter
    {
        public BoolParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLTags.boolNodeTag;
        }
        public override string GetParameterType() { return "bool"; }
    }
    public class StringParameter : SimpleParameter
    {
        public StringParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLTags.stringNodeTag;
        }
        public override string GetParameterType() { return "string"; }
    }
    public class FilePathParameter : SimpleParameter
    {
        public FilePathParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLTags.filePathNodeTag;
        }
        public override string GetParameterType() { return "File"; }
    }
    public class DirPathParameter : SimpleParameter
    {
        public DirPathParameter(string name, string comment, string defaultValue)
        {
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
            m_xmlTag = XMLTags.dirPathNodeTag;
        }
        public override string GetParameterType() { return "Dir"; }
    }

    public class EnumParameter : IParameter
    {
        string m_name, m_className, m_comment, m_default;
        public EnumParameter(string className, string name, string comment, string defaultValue)
        {
            m_className = className;
            m_name = name;
            m_comment = comment;
            m_default = defaultValue;
        }
        public string GetParameterType() { return m_className; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueSimpleParameter; } //default value
        public string GetName() { return m_name; }
        public string GetDescription() { return m_comment; }
        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.enumNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className
                + "\" " + XMLTags.commentAttribute + "=\"" + m_comment + "\" " + XMLTags.defaultAttribute
                + "=\"" + m_default + "\"/>\n";
            return output;
        }
    }

    public class ChildObjectParameter : IParameter
    {
        string m_className, m_name, m_comment, m_badgerMetadata;
        bool m_bOptional;
        public ChildObjectParameter(string className, string objectName, string comment, bool optional, string badgerMetadata)
        {
            m_className = className;
            m_name = objectName;
            m_comment = comment;
            m_badgerMetadata = badgerMetadata;
            m_bOptional = optional;
        }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue()
        {
            return m_parameterIndexInCode + XMLTags.sortingValueChildClass;
        }
        public string GetName() { return m_name; }
        public string GetParameterType() { return m_className; }
        public string GetDescription() { return m_comment; }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.branchNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className + "\" " + XMLTags.commentAttribute
                + "=\"" + m_comment + "\"";
            if (m_bOptional)
                output += " " + XMLTags.optionalAttribute + "=\"True\"";
            if (m_badgerMetadata != "")
                output += " " + XMLTags.badgerMetadataAttribute + "=\"" + m_badgerMetadata + "\"";
            output += "/>\n";
            return output;
        }
    }
    public class ChildObjectFactoryParameter : IParameter
    {
        string m_className, m_name, m_comment, m_badgerMetadata;
        bool m_bOptional;
        public ChildObjectFactoryParameter(string className, string objectName, string comment, bool optional, string badgerMetadata)
        {
            m_className = className;
            m_name = objectName;
            m_comment = comment;
            m_badgerMetadata = badgerMetadata;
            m_bOptional = optional;
        }
        public string GetName() { return m_name; }
        public string GetParameterType() { return m_className + " subclass"; }
        public string GetDescription() { return m_comment; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue()
        {
            return m_parameterIndexInCode + XMLTags.sortingValueChildClass;
        }


        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.branchNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className + "-Factory\" " + XMLTags.commentAttribute
                + "=\"" + m_comment + "\"";
            if (m_bOptional)
                output += " " + XMLTags.optionalAttribute + "=\"True\"";
            if (m_badgerMetadata != "")
                output += " " + XMLTags.badgerMetadataAttribute + "=\"" + m_badgerMetadata + "\"";
            output += "/>\n";
            return output;
        }
    }
    public class VarRefParameter : IParameter
    {
        protected string m_name;
        protected string m_comment;
        protected VarType m_type;
        public enum VarType { StateVariable, ActionVariable, WireConnection };
        public VarRefParameter(VarType type, string name, string comment)
        {
            m_type = type;
            m_name = name;
            m_comment = comment;
        }
        public string GetName() { return m_name; }
        public string GetParameterType()
        {
            switch (m_type)
            {
                case VarType.StateVariable: return "State";
                case VarType.ActionVariable: return "Action";
                case VarType.WireConnection: return "Wire";
            }
            return null;
        }
        public string GetDescription() { return m_comment; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueSimpleParameter; }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            if (m_type == VarType.StateVariable) output += "<" + XMLTags.stateVarRefTag;
            else if (m_type == VarType.ActionVariable) output += "<" + XMLTags.actionVarRefTag;
            else
                output += "<" + XMLTags.WireTag;
            output += " " + XMLTags.nameAttribute + "=\"" + m_name + "\" " + XMLTags.commentAttribute + "=\""
                + m_comment + "\"/>\n";
            return output;
        }
    }
    public class StateVarRefParameter : VarRefParameter
    {
        public StateVarRefParameter(string name, string comment) : base(VarType.StateVariable, name, comment) { }
    }
    public class ActionVarRefParameter : VarRefParameter
    {
        public ActionVarRefParameter(string name, string comment) : base(VarType.ActionVariable, name, comment) { }
    }
    public class WireConnectionParameter : VarRefParameter
    {
        public WireConnectionParameter(string name, string comment) : base(VarType.WireConnection, name, comment) { }
    }
    public class MultiValueParameter : IParameter
    {
        string m_className, m_name, m_comment;
        bool m_bOptional;
        public MultiValueParameter(string className, string objectName, string comment, bool optional)
        {
            m_className = className;
            m_name = objectName;
            m_comment = comment;
            m_bOptional = optional;
        }
        public string GetName() { return m_name; }
        public string GetParameterType() { return "Multiple " + m_className; }
        public string GetDescription() { return m_comment; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue()
        {
            return m_parameterIndexInCode + XMLTags.sortingValueMultiParameter;
        }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.multiValuedNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className + "\" " + XMLTags.commentAttribute
                + "=\"" + m_comment + "\"";
            if (m_bOptional)
                output += " " + XMLTags.optionalAttribute + "=\"True\"";
            output += "/>\n";
            return output;
        }
    }
    public class MultiValueFactoryParameter : IParameter
    {
        string m_className, m_name, m_comment;
        bool m_bOptional;
        public MultiValueFactoryParameter(string className, string objectName, string comment, bool optional)
        {
            m_className = className;
            m_name = objectName;
            m_comment = comment;
            m_bOptional = optional;
        }
        public string GetName() { return m_name; }
        public string GetParameterType() { return "Multiple " + m_className + " subclass"; }
        public string GetDescription() { return m_comment; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue()
        {
            return m_parameterIndexInCode + XMLTags.sortingValueMultiParameter;
        }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.multiValuedNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className + "-Factory\" " + XMLTags.commentAttribute
                + "=\"" + m_comment + "\"";
            if (m_bOptional)
                output += " " + XMLTags.optionalAttribute + "=\"True\"";
            output += "/>\n";
            return output;
        }
    }

    public class MultiVariableParameter : IParameter
    {
        string m_className, m_name, m_comment;
        WorldParser.WorldParameterType m_type;

        public MultiVariableParameter(string className, string objectName, string comment)
        {
            switch (className)
            {
                case "ACTION_VARIABLE": m_className = XMLTags.actionVarRefTag; m_type = WorldParser.WorldParameterType.ActionVariable; break;
                default:
                case "STATE_VARIABLE": m_className = XMLTags.stateVarRefTag; m_type = WorldParser.WorldParameterType.StateVariable; break;
            }

            m_name = objectName;
            m_comment = comment;
        }
        public string GetName() { return m_name; }
        public string GetParameterType() { return "Multiple " + WorldParser.GetVariableType(m_type); }
        public string GetDescription() { return m_comment; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueSimpleParameter; }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.multiValuedNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className + "\" " + XMLTags.commentAttribute
                + "=\"" + m_comment + "\"/>\n";
            return output;
        }
    }
    public class MultiValueSimpleParameter : IParameter
    {
        string m_className, m_name, m_comment, m_default;

        public MultiValueSimpleParameter(string className, string objectName, string comment, string defaultValue)
        {
            switch (className)
            {
                case "DOUBLE_PARAM": m_className = XMLTags.doubleNodeTag; break;
                case "BOOL_PARAM": m_className = XMLTags.boolNodeTag; break;
                case "INT_PARAM": m_className = XMLTags.integerNodeTag; break;
                case "STRING_PARAM": m_className = XMLTags.stringNodeTag; break;
                case "FILE_PATH_PARAM": m_className = XMLTags.filePathNodeTag; break;
                default:
                case "DIR_PATH_PARAM": m_className = XMLTags.dirPathNodeTag; break;
            }

            m_name = objectName;
            m_comment = comment;
            m_default = defaultValue;
        }
        public string GetName() { return m_name; }
        public string GetParameterType()
        {
            switch (m_className)
            {
                case "DOUBLE_PARAM": return "Multiple (double)";
                case "BOOL_PARAM": return "Multiple (bool)";
                case "INT_PARAM": return "Multiple (int)";
                case "STRING_PARAM": return "Multiple (string)";
                case "FILE_PATH_PARAM": return "Multiple (File)";
                default:
                case "DIR_PATH_PARAM": return "Multiple (Dir)";
            }
        }
        public string GetDescription() { return m_comment; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueSimpleParameter; }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.multiValuedNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className + "\" " + XMLTags.commentAttribute
                + "=\"" + m_comment + "\" " + XMLTags.defaultAttribute + "=\"" + m_default + "\"/>\n";
            return output;
        }
    }
    public class WorldParameter : IParameter
    {
        WorldParser.WorldParameterType m_type;
        string m_name;
        double m_min, m_max;
        string m_unit;
        public WorldParameter(WorldParser.WorldParameterType type, string name, double min, double max, string unit)
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
            m_type = type;
            m_name = name;
        }
        public string GetName() { return m_name; }
        public string GetParameterType() { return WorldParser.GetVariableType(m_type);}
        public WorldParser.WorldParameterType GetWorldParameterType() { return m_type; }
        public string GetDescription() { return null; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueWorldVariable; }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            string typeTag;
            switch (m_type)
            {
                case WorldParser.WorldParameterType.StateVariable: typeTag = XMLTags.stateVarTag; break;
                case WorldParser.WorldParameterType.ActionVariable: typeTag = XMLTags.actionVarTag; break;
                default: typeTag = XMLTags.constantTag; break;
            }
            output += "<" + typeTag + " " + XMLTags.nameAttribute + "=\"" + m_name + "\"";
            if (m_type != WorldParser.WorldParameterType.Constant)
                output += " " + XMLTags.minValueAttribute
                + "=\"" + m_min + "\" " + XMLTags.maxValueAttribute + "=\"" + m_max + "\" "
                + XMLTags.unitAttribute + "=\"" + m_unit + "\"/>\n";
            else output += "/>\n";
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
        public void AddParameter(IParameter parameter)
        {
            parameter.SetParameterIndexInCode(m_parameters.Count);
            m_parameters.Add(parameter);
        }
        public string GetName() { return m_name; }
        public string GetParameterType() { return m_className + " subclass"; }
        public string GetDescription() { return m_comment; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueSimpleParameter; }

        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            output += "<" + XMLTags.choiceNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + m_className + "\" " + XMLTags.commentAttribute
                + "=\"" + m_comment + "\">\n";
            foreach (IParameter parameter in m_parameters)
            {
                output += parameter.AsXML(level + 1);
            }
            FileFormatter.AddIndentation(ref output, level);
            output += "</" + XMLTags.choiceNodeTag + ">\n";
            return output;
        }
    }
    public class ChoiceElementParameter : IParameter
    {
        public enum Type { New, Factory };
        string m_name, m_className;
        Type m_type;
        string m_badgerMetadata;

        public string GetName() { return m_name; }
        public string GetParameterType() { return m_className + " subclass"; }
        public string GetDescription() { return null; }
        public int m_parameterIndexInCode;

        public void SetParameterIndexInCode(int index) { m_parameterIndexInCode = index; }
        public int ParameterClassSortValue() { return m_parameterIndexInCode + XMLTags.sortingValueSimpleParameter; }

        public ChoiceElementParameter(string name, string className, Type type)
        {
            m_name = name;
            m_className = className;
            m_type = type;
        }
        public ChoiceElementParameter(string name, string className, Type type, string badgerMetadata)
        {
            m_name = name;
            m_className = className;
            m_type = type;
            m_badgerMetadata = badgerMetadata;
        }
        public string AsXML(int level)
        {
            string output = "";
            FileFormatter.AddIndentation(ref output, level);
            string className = m_className;
            if (m_type == Type.Factory)
                className += "-Factory";
            output += "<" + XMLTags.choiceElementNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name
                + "\" " + XMLTags.classAttribute + "=\"" + className + "\"";
            if (m_badgerMetadata != null)
                output += " " + XMLTags.badgerMetadataAttribute + "=\"" + m_badgerMetadata + "\"";
            output += "/>\n";

            return output;
        }
    }
}
