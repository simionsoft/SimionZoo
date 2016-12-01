using System;
using System.Collections.Generic;

using System.Text.RegularExpressions;

namespace SimionSrcParser
{
    abstract public class Parser
    {
        protected List<string> parsedArguments;
        protected string m_inSrcTemplateName;
        protected bool m_bUseTemplateArgument;
        protected string m_templateArgument;
        public Parser(string inSrcTemplateName, bool useTemplateArgument)
        {
            m_inSrcTemplateName = inSrcTemplateName;
            m_bUseTemplateArgument = useTemplateArgument;
            parsedArguments = new List<string>();
        }
        public void parse(string content, Constructor parent)
        {
            string sPattern;
            if (!m_bUseTemplateArgument)
                sPattern = m_inSrcTemplateName + @"\s*\(" + ConstructorParameterParser.extractTokenRegex + @"\)";
            else
                sPattern = m_inSrcTemplateName + @"\s*<([^>]+)>\s*?\(" + ConstructorParameterParser.extractTokenRegex + @"\)";

            foreach (Match match in Regex.Matches(content, sPattern))
            {
                string strippedArgumentValue;
                parsedArguments.Clear();
                if (m_bUseTemplateArgument)
                    parsedArguments.Add(match.Groups[1].Value.Trim(' '));

                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, ConstructorParameterParser.extractFuncRegex);
                string arguments = functionArgumentsMatch.Groups[2].Value;
                var argumentMatches = Regex.Matches(arguments, ConstructorParameterParser.extractArgsRegex);
                foreach (Match argMatch in argumentMatches)
                {
                    strippedArgumentValue = argMatch.Value.Trim(' ', '\"', '\n', '\t');

                    parsedArguments.Add(strippedArgumentValue);
                }
                processParameter(parent);
            }
        }

        public abstract void processParameter(Constructor parent);
    }

    public class IntParameterParser : Parser
    {
        public IntParameterParser() : base("INT_PARAM", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new IntParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class DoubleParameterParser : Parser
    {
        public DoubleParameterParser() : base("DOUBLE_PARAM", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new DoubleParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class BoolParameterParser : Parser
    {
        public BoolParameterParser() : base("BOOL_PARAM", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new BoolParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class StringParameterParser : Parser
    {
        public StringParameterParser() : base("STRING_PARAM", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new StringParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class FilePathParameterParser : Parser
    {
        public FilePathParameterParser() : base("FILE_PATH_PARAM", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new FilePathParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class DirPathParameterParser : Parser
    {
        public DirPathParameterParser() : base("DIR_PATH_PARAM", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new DirPathParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class ChildObjectParameterParser : Parser
    {
        public ChildObjectParameterParser() : base("CHILD_OBJECT", true) { }
        public override void processParameter(Constructor parent)
        {
            bool bOptional = false; //default value
            string badgerInfo = "";
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);
            if (parsedArguments.Count > 5)
                badgerInfo = parsedArguments[5];
            parent.addParameter(new ChildObjectParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], bOptional, badgerInfo));
        }
    }
    public class ChildObjectFactoryParameterParser : Parser
    {
        public ChildObjectFactoryParameterParser() : base("CHILD_OBJECT_FACTORY", true) { }
        public override void processParameter(Constructor parent)
        {
            bool bOptional = false; //default value
            string badgerInfo = "";
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);
            if (parsedArguments.Count > 5)
                badgerInfo = parsedArguments[5];
            parent.addParameter(new ChildObjectFactoryParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], bOptional, badgerInfo));
        }
    }
    public class StateVarRefParser : Parser
    {
        public StateVarRefParser() : base("STATE_VARIABLE", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new StateVarRefParameter(parsedArguments[1], parsedArguments[2]));
        }
    }
    public class ActionVarRefParser : Parser
    {
        public ActionVarRefParser() : base("ACTION_VARIABLE", false) { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new ActionVarRefParameter(parsedArguments[1], parsedArguments[2]));
        }
    }
    public class MultiValueParameterParser : Parser
    {
        public MultiValueParameterParser() : base("MULTI_VALUE", true) { }
        public override void processParameter(Constructor parent)
        {
            bool bOptional = false; //default value
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);

            parent.addParameter(new MultiValueParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], bOptional));
        }
    }
    public class MultiValueFactoryParameterParser : Parser
    {
        public MultiValueFactoryParameterParser() : base("MULTI_VALUE", true) { }
        public override void processParameter(Constructor parent)
        {
            bool bOptional = false; //default value
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);

            parent.addParameter(new MultiValueFactoryParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], bOptional));
        }
    }
    public class MultiValueSimpleParameterParser : Parser
    {
        public MultiValueSimpleParameterParser() : base("MULTI_VALUE_SIMPLE_PARAM", true) { }
        public override void processParameter(Constructor parent)
        {
            string simpleParameterType = parsedArguments[0].Substring(0, parsedArguments[0].IndexOf(",") - 1);
            parent.addParameter(new MultiValueSimpleParameter(simpleParameterType, parsedArguments[2]
                , parsedArguments[3], parsedArguments[4]));
        }
    }
    public abstract class WorldParser : Parser
    {
        public enum WorldParameterType { StateVariable, ActionVariable, Constant };
        WorldParameterType m_type;
        public WorldParser(WorldParameterType type, string srcTag) : base(srcTag, false)
        {
            m_type = type;
        }
    }
    public class StateVariableParser : WorldParser
    {
        public StateVariableParser() : base(WorldParser.WorldParameterType.StateVariable, "addStateVariable") { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new WorldParameter(WorldParser.WorldParameterType.StateVariable
                , parsedArguments[0], double.Parse(parsedArguments[2]), double.Parse(parsedArguments[3]), parsedArguments[1]));
        }
    }
    public class ActionVariableParser : WorldParser
    {
        public ActionVariableParser() : base(WorldParser.WorldParameterType.ActionVariable, "addActionVariable") { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new WorldParameter(WorldParser.WorldParameterType.ActionVariable
                , parsedArguments[0], double.Parse(parsedArguments[2]), double.Parse(parsedArguments[3]), parsedArguments[1]));
        }
    }
    public class ConstantParser : WorldParser
    {
        public ConstantParser() : base(WorldParser.WorldParameterType.StateVariable, "addConstant") { }
        public override void processParameter(Constructor parent)
        {
            parent.addParameter(new WorldParameter(WorldParser.WorldParameterType.Constant
                , parsedArguments[0]));
        }
    }
    public class MetadataParser : Parser
    {
        public MetadataParser() : base("METADATA", false) { }
        public override void processParameter(Constructor parent)
        {
            if (parsedArguments[0] == "World")
                parent.m_world = parsedArguments[1];
        }
    }
}