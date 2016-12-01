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
        public void parse(string content, ref List<Parameter> parsedParameters)
        {
            string sPattern;
            if (!m_bUseTemplateArgument)
                sPattern= m_inSrcTemplateName + @"\s*\(" + ParameterParser.extractTokenRegex + @"\)";
            else
                sPattern = m_inSrcTemplateName + @"\s*<\s*(\w+?)\s*>\s*?\(" + ParameterParser.extractTokenRegex + @"\)";

            foreach (Match match in Regex.Matches(content, sPattern))
            {
                string strippedArgumentValue;
                parsedArguments.Clear();
                if (m_bUseTemplateArgument)
                    parsedArguments.Add(match.Groups[1].Value);

                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, ParameterParser.extractFuncRegex);
                string arguments = functionArgumentsMatch.Groups[2].Value;
                var argumentMatches = Regex.Matches(arguments, ParameterParser.extractArgsRegex);
                foreach (Match argMatch in argumentMatches)
                {
                    strippedArgumentValue = argMatch.Value.Trim(' ', '\"','\n','\t');

                    parsedArguments.Add(strippedArgumentValue);
                }
                addParameter(ref parsedParameters);
            }
        }

        public abstract void addParameter(ref List<Parameter> parsedParameters);
    }

    public class IntParameterParser : Parser
    {
        public IntParameterParser() : base("INT_PARAM", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new IntParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class DoubleParameterParser : Parser
    {
        public DoubleParameterParser() : base("DOUBLE_PARAM", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new DoubleParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class BoolParameterParser : Parser
    {
        public BoolParameterParser() : base("BOOL_PARAM", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new BoolParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class StringParameterParser : Parser
    {
        public StringParameterParser() : base("STRING_PARAM", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new StringParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class FilePathParameterParser : Parser
    {
        public FilePathParameterParser() : base("FILE_PATH_PARAM", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new FilePathParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class DirPathParameterParser : Parser
    {
        public DirPathParameterParser() : base("DIR_PATH_PARAM", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new DirPathParameter(parsedArguments[1], parsedArguments[2], parsedArguments[3]));
        }
    }
    public class ChildObjectParameterParser : Parser
    {
        public ChildObjectParameterParser() : base("CHILD_OBJECT", true) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            bool bOptional = false; //default value
            string badgerInfo = "";
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);
            if (parsedArguments.Count>5)
                badgerInfo= parsedArguments[5];
            parsedParameters.Add(new ChildObjectParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3],bOptional ,badgerInfo));
        }
    }
    public class ChildObjectFactoryParameterParser : Parser
    {
        public ChildObjectFactoryParameterParser() : base("CHILD_OBJECT_FACTORY", true) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            bool bOptional = false; //default value
            string badgerInfo = "";
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);
            if (parsedArguments.Count > 5)
                badgerInfo = parsedArguments[5];
            parsedParameters.Add(new ChildObjectFactoryParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], bOptional, badgerInfo));
        }
    }
    public class StateVarRefParser : Parser
    {
        public StateVarRefParser() : base("STATE_VARIABLE", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new StateVarRefParameter(parsedArguments[1], parsedArguments[2]));
        }
    }
    public class ActionVarRefParser : Parser
    {
        public ActionVarRefParser() : base("ACTION_VARIABLE", false) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new ActionVarRefParameter(parsedArguments[1], parsedArguments[2]));
        }
    }
    public class MultiValueParameterParser : Parser
    {
        public MultiValueParameterParser() : base("MULTI_VALUE", true) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            bool bOptional = false; //default value
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);

            parsedParameters.Add(new MultiValueParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], bOptional));
        }
    }
    public class MultiValueFactoryParameterParser : Parser
    {
        public MultiValueFactoryParameterParser() : base("MULTI_VALUE", true) { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            bool bOptional = false; //default value
            if (parsedArguments.Count > 4)
                bOptional = bool.Parse(parsedArguments[4]);

            parsedParameters.Add(new MultiValueFactoryParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], bOptional));
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
    public class StateVariableParser: WorldParser
    { 
        public StateVariableParser() : base(WorldParser.WorldParameterType.StateVariable, "addStateVariable") { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new WorldParameter(WorldParser.WorldParameterType.StateVariable
                ,parsedArguments[0], double.Parse(parsedArguments[2]), double.Parse(parsedArguments[3]),parsedArguments[1]));
        }
    }
    public class ActionVariableParser : WorldParser
    {
        public ActionVariableParser() : base(WorldParser.WorldParameterType.ActionVariable, "addActionVariable") { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new WorldParameter(WorldParser.WorldParameterType.ActionVariable
                , parsedArguments[0], double.Parse(parsedArguments[2]), double.Parse(parsedArguments[3]), parsedArguments[1]));
        }
    }
    public class ConstantParser : WorldParser
    {
        public ConstantParser() : base(WorldParser.WorldParameterType.StateVariable, "addConstant") { }
        public override void addParameter(ref List<Parameter> parsedParameters)
        {
            parsedParameters.Add(new WorldParameter(WorldParser.WorldParameterType.Constant
                , parsedArguments[0]));
        }
    }
}
