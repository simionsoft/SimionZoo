using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;



namespace CustomXMLBuilder
{
    
    class CPPProcessor
    {
        public string processFile(string filename)
        {

            CPPParser cppParser= new CPPParser();
            string fileContents = File.ReadAllText(filename,Encoding.UTF8);
            fileContents= fileContents.Replace('\r',' ');//'\n');
            fileContents = fileContents.Replace('\n', ' ');

            return (cppParser.parseCPPFile(fileContents));
        }
    }

    class CPPParser
    {
        string extractTokenRegex = @"(?>[^()]+|\((?<open>)|\)(?<-open>))*";
        string extractFuncRegex = @"\b[^()]+\((.*)\)$";
        string extractArgsRegex = @"(?:[^,()]+((?:\(\s+(?>[^()]+|\((?<open>)|\)(?<-open>))*\)))*)+";
        int m_level = 0;
        void increaseIndent() { m_level++; }
        void decreaseIndent() { m_level--; }
        string getLevelIndent()
        {
            string indentation = "";
            for (int i = 0; i < m_level; i++)
                indentation += "  ";
            return indentation;
        }

        public string parseChoiceElements(string text)
        {
            increaseIndent();
            //#define CHOICE_ELEMENT(checkVariable, checkLiteral, className, ...)
            //#define CHOICE_ELEMENT_FACTORY(checkVariable, checkLiteral, className, ...) 
            string sPattern = @"(CHOICE_ELEMENT|CHOICE_ELEMENT_FACTORY)\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters= Regex.Match(match.Groups[0].Value,extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                parsedXML += getLevelIndent() + "<CHOICE-ELEMENT Name=" + parameterMatches[1].Value.Trim(' ') +" Class=\""
                        + parameterMatches[2].Value.Trim(' ') + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;

        }
        public string parseChoices(string text)
        {
            //#define CHOICE(name)
            //#define END_CHOICE()
            string sPattern = @"CHOICE\(""(?<name>[^,]+)""\)(?<choiceDefinition>.+)END_CHOICE\(\)";
            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                increaseIndent();
                parsedXML += getLevelIndent() + "<CHOICE Name=\"" + match.Groups["name"].Value + "\">\n";

                parsedXML += parseChoiceElements(match.Groups["choiceDefinition"].Value);

                parsedXML += getLevelIndent() + "</CHOICE>\n";
                decreaseIndent();
            }

            return parsedXML;
        }
        public string parseConstIntegerValues(string text)
        {
            increaseIndent();
            //#define CONST_INTEGER_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstInteger(parameterName,defaultValue);
            string sPattern = @"CONST_INTEGER_VALUE\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<INTEGER-VALUE Name=\"" + parameterMatches[1].Value.Trim(' ') + "\" Class="
                    + parameterMatches[2].Value.Trim(' ') + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                    + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseConstDoubleValues(string text)
        {
            increaseIndent();
            //#define CONST_DOUBLE_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstDouble(parameterName,defaultValue);
            string sPattern = @"CONST_DOUBLE_VALUE\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<DOUBLE-VALUE Name=\"" + parameterMatches[1].Value.Trim(' ') + "\" Class="
                     + parameterMatches[2].Value.Trim(' ') + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                     + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseConstBooleanValues(string text)
        {
            increaseIndent();
            //#define CONST_BOOLEAN_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstBoolean(parameterName,defaultValue);
            string sPattern = @"CONST_BOOLEAN_VALUE\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<BOOLEAN-VALUE Name=\"" + parameterMatches[1].Value.Trim(' ') + "\" Class="
                    + parameterMatches[2].Value.Trim(' ') + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                    + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseConstStringValues(string text)
        {
            increaseIndent();
            //#define CONST_STRING_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstString(parameterName,defaultValue);
            string sPattern = @"CONST_STRING_VALUE\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<STRING-VALUE Name=\"" + parameterMatches[1].Value.Trim(' ') + "\" Class="
                     + parameterMatches[2].Value.Trim(' ') + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                     + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseFilePathValues(string text)
        {
            increaseIndent();
            decreaseIndent();
            return "";
        }
        public string parseDirPathValues(string text)
        {
            increaseIndent();
            decreaseIndent();
            return "";
        }
        public string parseXMLNodeRefs(string text)
        {
            increaseIndent();
            decreaseIndent();
            return "";
        }
        public string parseMultiValues(string text)
        {
            increaseIndent();
            //#define MULTI_VALUE(name,indexedVariable,className,parameters)
            //#define MULTI_VALUE_FACTORY(name,indexedVariable,className,parameters)
            string sPattern = @"(MULTI_VALUED_FACTORY|MULTI_VALUED)\(""(?<name>[^,]+)"",(?<indexedVariable>[^,]+),(?<className>[^,]+),(?<parameters>.+)\)";
            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                parsedXML += getLevelIndent() + "<MULTI-VALUED Name=\"" + match.Groups["name"].Value 
                    + "\" Class=\"" + match.Groups["className"].Value +"\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseChildClasses(string text)
        {
            increaseIndent();
            //#define CHILD_CLASS(variable,className,constructorParameters,...) variable= new className(constructorParameters,__VA_ARGS__);
            //#define CHILD_CLASS_FACTORY(variable,className,constructorParameters,...) variable= className::getInstance(constructorParameters,__VA_ARGS__);

            string sPattern = @"(CHILD_CLASS|CHILD_CLASS_FACTORY)\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<BRANCH Name=" + parameterMatches[1].Value.Trim(' ') + " Class=\""
                    + parameterMatches[2].Value.Trim(' ') + "\" Default=" + parameterMatches[3].Value.Trim(' ')
                    + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseNumericValues(string text)
        {
            increaseIndent();
            //#define NUMERIC_VALUE(variable,parameterNode,parameterName)
            string sPattern = @"NUMERIC_VALUE\((?<variable>\w+),(?<parameterNode>\w+),""(?<parameterName>\w+)""\)";
            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                parsedXML += getLevelIndent() + "<BRANCH Name=\"" + match.Groups["parameterName"].Value + "\" Class=\"NUMERIC-VALUE\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseClasses(string text)
        {
            string sPattern = @"(CLASS_CONSTRUCTOR|CLASS_FACTORY)\((?<className>\w+)\)(?<classDefinition>.+)END_CLASS\(\)";
            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                parsedXML += getLevelIndent() + "<CLASS Name=\"" + match.Groups["className"].Value + "\">\n";
                increaseIndent();

                string classDefinition= match.Groups["classDefinition"].Value;
                parsedXML += getLevelIndent() + parseChildClasses(classDefinition);
                parsedXML += getLevelIndent() + parseChoices(classDefinition);
                parsedXML += getLevelIndent() + parseMultiValues(classDefinition);
                parsedXML += getLevelIndent() + parseNumericValues(classDefinition);
                parsedXML += getLevelIndent() + parseConstIntegerValues(classDefinition);
                parsedXML += getLevelIndent() + parseConstBooleanValues(classDefinition);
                parsedXML += getLevelIndent() + parseConstStringValues(classDefinition);
                parsedXML += getLevelIndent() + parseConstDoubleValues(classDefinition);
                parsedXML += getLevelIndent() + parseFilePathValues(classDefinition);
                parsedXML += getLevelIndent() + parseDirPathValues(classDefinition);

                decreaseIndent();
                parsedXML += getLevelIndent() + "</CLASS>\n";      
            }
            return parsedXML;
        }

        public string parseCPPFile( string text)
        {
            m_level = 0;
            return parseClasses(text);
        }
    }
}
