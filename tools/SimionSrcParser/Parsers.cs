using System;
using System.Collections.Generic;

using System.Text.RegularExpressions;

namespace SimionSrcParser
{
    abstract public class Parser
    {
        protected List<string> parsedArguments;
        protected string templateArgument;
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
            string sPattern = m_inSrcTemplateName + @"\s*\(" + ParameterParser.extractTokenRegex + @"\)";

            foreach (Match match in Regex.Matches(content, sPattern))
            {
                string strippedArgumentValue;
                parsedArguments.Clear();

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
}
