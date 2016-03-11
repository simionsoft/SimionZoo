using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;


namespace CustomXMLBuilder
{
    class Checker
    {
        private List<string> m_classDefinitions;
        private List<string> m_classReferences;
        private int m_numErrors= 0;

        public void addClassDefinition(string className){m_classDefinitions.Add(className);}
        public void addClassReference(string className){m_classReferences.Add(className);}
        public Checker()
        {
            m_classDefinitions = new List<string>();
            m_classReferences = new List<string>();
        }
        public void checkClassReferences()
        {
            foreach (string classRef in m_classReferences)
            {
                if (!m_classDefinitions.Contains(classRef))
                {
                    Console.WriteLine("ERROR: Reference to undefined class found: " + classRef);
                    //m_classReferences.RemoveAll(c => c==classRef);
                    m_numErrors++;
                }
            }
        }
    }
    class CPPProcessor
    {
        public Checker m_checker = new Checker();
        public int numCharsProcessed = 0;
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
        public string processFile(string filename)
        {
            m_level = 1;
            string fileContents = File.ReadAllText(filename, Encoding.UTF8);
            fileContents = fileContents.Replace('\r', ' ');//'\n');
            fileContents = fileContents.Replace('\n', ' ');

            numCharsProcessed += fileContents.Length;

            return parseClasses(fileContents);
        }
        //public MatchCollection parseMacroFunction(string text,string functionTag, string functionCloseTag)
        //{
        //    string sPattern;
        //    if (functionCloseTag!="") sPattern= functionTag + @"\(" + extractTokenRegex + @"\)";
        //    else sPattern = functionTag + @"\(" + extractTokenRegex + @"\)(?<choiceDefinition>.+)" + functionCloseTag + @"\(\)"
            
        //    MatchCollection functionMatches= Regex.Matches(text, sPattern);
        //    return functionMatches;
        //}
        //public MatchCollection parseParameters(string text, Match functionMatch)
        //{
        //        //Console.WriteLine(match.Value);
        //        var parameters = Regex.Match(text, extractFuncRegex);
        //        string arguments = parameters.Groups[1].Value;

        //        var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
        //        return parameterMatches;
        //}
        //public string parseChoiceElements(string text)
        //{
        //    var functionMatches = parseMacroFunction(text, @"(CHOICE_ELEMENT|CHOICE_ELEMENT_FACTORY)", "");
        //    foreach (var functionMatch in functionMatches)
        //    {
        //        var parameterMatches = parseParameters(extractFuncRegex, functionMatch.Groups[0].Value);
        //    }
        //}
        public string parseChoiceElements(string text)
        {
            increaseIndent();
            //#define CHOICE_ELEMENT(checkVariable, checkLiteral, className, ...)
            //#define CHOICE_ELEMENT_FACTORY(checkVariable, checkLiteral, className, ...) 
            string sPattern = @"(CHOICE_ELEMENT|CHOICE_ELEMENT_FACTORY)\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = functionArgumentsMatch.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(parameterMatches[2].Value.Trim(' '));
                parsedXML += getLevelIndent() + "<CHOICE-ELEMENT Name=" + parameterMatches[1].Value.Trim(' ') + " Class=\""
                        + parameterMatches[2].Value.Trim(' ') + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseChoiceElementsLoadXML(string text)
        {
            increaseIndent();
            //define CHOICE_ELEMENT_XML(checkVariable,checkLiteral,className,XMLFilename, ...)
            string sPattern = @"CHOICE_ELEMENT_XML\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = functionArgumentsMatch.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(parameterMatches[2].Value.Trim(' '));
                parsedXML += getLevelIndent() + "<CHOICE-ELEMENT Name=" + parameterMatches[1].Value.Trim(' ') + " Class=\""
                        + parameterMatches[2].Value.Trim(' ') + "\" LoadXML=" + parameterMatches[3].Value.Trim(' ') + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseChoices(string text)
        {
            //#define CHOICE(name)
            //#define CHOICE_XML(name,loadXML)
            //#define END_CHOICE()
            increaseIndent();

            string sPattern = @"(CHOICE|CHOICE_XML)\s*\(" + extractTokenRegex + @"\)(.*?)END_CHOICE\(\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, @"(CHOICE|CHOICE_XML)\(" + extractTokenRegex + @"\)");
                string arguments = functionArgumentsMatch.Groups[0].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<CHOICE Name=" + parameterMatches[1].Value;
                if (parameterMatches.Count>2)
                    parsedXML += @" LoadXML=" + parameterMatches[2].Value;
                parsedXML += ">\n";

                parsedXML += parseChoiceElementsLoadXML(match.Groups[2].Value);
                parsedXML += parseChoiceElements(match.Groups[2].Value);

                parsedXML += getLevelIndent() + "</CHOICE>\n";

            }
            decreaseIndent();
            return parsedXML;
        }
     
        public string parseConstIntegerValues(string text)
        {
            increaseIndent();
            //#define CONST_INTEGER_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstInteger(parameterName,defaultValue);
            string sPattern = @"CONST_INTEGER_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<INTEGER-VALUE Name=" + parameterMatches[2].Value.Trim(' ')
                    + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                    + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseConstDoubleValues(string text)
        {
            increaseIndent();
            //#define CONST_DOUBLE_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstDouble(parameterName,defaultValue);
            string sPattern = @"CONST_DOUBLE_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<DOUBLE-VALUE Name=" + parameterMatches[2].Value.Trim(' ')
                    + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                     + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseConstBooleanValues(string text)
        {
            increaseIndent();
            //#define CONST_BOOLEAN_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstBoolean(parameterName,defaultValue);
            string sPattern = @"CONST_BOOLEAN_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<BOOLEAN-VALUE Name=" + parameterMatches[2].Value.Trim(' ') 
                    + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                    + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseConstStringValues(string text)
        {
            increaseIndent();
            //#define CONST_STRING_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstString(parameterName,defaultValue);
            string sPattern = @"CONST_STRING_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<STRING-VALUE Name=" + parameterMatches[2].Value.Trim(' ') 
                    + " Default=" + parameterMatches[3].Value.Trim(' ') + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseFilePathValues(string text)
        {
            increaseIndent();
            //#define FILE_PATH_VALUE(variable,parameterNode,variableName,default) variable= (char*)parameterNode->getConstString(variableName);
            string sPattern = @"FILE_PATH_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<FILE-PATH-VALUE Name=" + parameterMatches[2].Value.Trim(' ') 
                    + " Default=" + parameterMatches[3].Value.Trim(' ')                     + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseDirPathValues(string text)
        {
            increaseIndent();
            // #define DIR_PATH_VALUE(variable,parameterNode,variableName,default) variable= (char*)parameterNode->getConstString(variableName);

            string sPattern = @"DIR_PATH_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<DIR-PATH-VALUE Name=" + parameterMatches[2].Value.Trim(' ')
                    + " Default=" + parameterMatches[3].Value.Trim(' ') + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseStateVariableRefs(string text)
        {
            increaseIndent();
            //#define STATE_VARIABLE_REF(variable,parameterNode,variableName) 
            // ->     <XML-NODE-REF Name="Variable" XMLFile="WORLD-DEFINITION" HangingFrom="State"/>

            string sPattern = @"STATE_VARIABLE_REF\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<XML-NODE-REF Name=" + parameterMatches[2].Value.Trim(' ')
                    + " XMLFile=\"WORLD-DEFINITION\" HangingFrom=\"State\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseActionVariableRefs(string text)
        {
            increaseIndent();
            //#define ACTION_VARIABLE_REF(variable,parameterNode,variableName) 
            // ->     <XML-NODE-REF Name="Output-Action" XMLFile="WORLD-DEFINITION" HangingFrom="Action"/>

            string sPattern = @"STATE_VARIABLE_REF\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<XML-NODE-REF Name=" + parameterMatches[2].Value.Trim(' ')
                    + " XMLFile=\"WORLD-DEFINITION\" HangingFrom=\"Action\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseMultiValues(string text)
        {
            increaseIndent();
            //#define MULTI_VALUE(name,indexedVariable,className,parameters)
            //#define MULTI_VALUE_FACTORY(name,indexedVariable,className,parameters)
            // -> <MULTI-VALUED Name="RBF-State-Grid-Dimension" Class="RBF-STATE-GRID-DIMENSION"/>
            string sPattern = @"(MULTI_VALUED_FACTORY|MULTI_VALUED)\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(parameterMatches[2].Value.Trim(' '));
                if (match.Groups[1].Value=="MULTI_VALUED")
                    parsedXML += getLevelIndent() + "<MULTI-VALUED Name=" + parameterMatches[1].Value.Trim(' ')
                        + " Class=\"" + parameterMatches[2].Value.Trim(' ') + "\"/>\n";
                else
                    parsedXML += getLevelIndent() + "<MULTI-VALUED Name=" + parameterMatches[1].Value.Trim(' ')
                       + " Class=\"" + parameterMatches[2].Value.Trim(' ') + "-Factory\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseChildClasses(string text)
        {
            increaseIndent();
            //#define CHILD_CLASS(variable,className,constructorParameters,...) variable= new className(constructorParameters,__VA_ARGS__);
            //#define CHILD_CLASS_FACTORY(variable,className,constructorParameters,...) variable= className::getInstance(constructorParameters,__VA_ARGS__);
            // -> <BRANCH Name="" Class=""/>
            string sPattern = @"(CHILD_CLASS|CHILD_CLASS_FACTORY)\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(parameterMatches[2].Value.Trim(' '));
                parsedXML += getLevelIndent() + "<BRANCH Name=" + parameterMatches[1].Value.Trim(' ') + " Class=\""  + parameterMatches[2].Value.Trim(' ') + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        public string parseNumericValues(string text)
        {
            increaseIndent();
            //#define NUMERIC_VALUE(variable,parameterNode,parameterName)
            string sPattern = @"NUMERIC_VALUE\s*\((?<variable>\w+),(?<parameterNode>\w+),""(?<parameterName>\w+)""\)";
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
           string sPattern = @"(CLASS_CONSTRUCTOR|CLASS_FACTORY)(.*?)END_CLASS\(\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, @"(CLASS_CONSTRUCTOR|CLASS_FACTORY)\s*\(" + extractTokenRegex + @"\)");
                string header = functionArgumentsMatch.Groups[0].Value;

                var parameterMatches = Regex.Matches(header, extractArgsRegex);
                m_checker.addClassDefinition(parameterMatches[1].Value.Trim(' '));

                if (parameterMatches[0].Value == "CLASS_CONSTRUCTOR")
                    parsedXML += getLevelIndent() + "<CLASS Name=\"" + parameterMatches[1].Value + "\">\n";
                else
                    parsedXML += getLevelIndent() + "<CLASS Name=\"" + parameterMatches[1].Value + "-Factory\">\n";

                string classDefinition= match.Groups[2].Value;
                parsedXML += parseChildClasses(classDefinition);
                parsedXML += parseChoices(classDefinition);
                parsedXML += parseMultiValues(classDefinition);
                parsedXML += parseNumericValues(classDefinition);
                parsedXML += parseConstIntegerValues(classDefinition);
                parsedXML += parseConstBooleanValues(classDefinition);
                parsedXML += parseConstStringValues(classDefinition);
                parsedXML += parseConstDoubleValues(classDefinition);
                parsedXML += parseStateVariableRefs(classDefinition);
                parsedXML += parseActionVariableRefs(classDefinition);
                parsedXML += parseFilePathValues(classDefinition);
                parsedXML += parseDirPathValues(classDefinition);

                parsedXML += getLevelIndent() + "</CLASS>\n\n";      
            }
            return parsedXML;
        }
    }
}
