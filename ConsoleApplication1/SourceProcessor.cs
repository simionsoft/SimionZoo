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
        private List<string> m_enumDefinitions;
        private List<string> m_enumReferences;
        private int m_numErrors= 0;

        public void addClassDefinition(string className){m_classDefinitions.Add(className);}
        public void addClassReference(string className){m_classReferences.Add(className);}
        public void addEnumDefinition(string enumName) { m_enumDefinitions.Add(enumName); }
        public void addEnumReference(string enumName) { m_enumReferences.Add(enumName); }
        public Checker()
        {
            m_classDefinitions = new List<string>();
            m_classReferences = new List<string>();
            m_enumDefinitions = new List<string>();
            m_enumReferences = new List<string>();
        }
        public int checkClassReferences()
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
            foreach (string enumRef in m_enumReferences)
            {
                if (!m_enumDefinitions.Contains(enumRef))
                {
                    Console.WriteLine("ERROR: Reference to undefined enumerated type found: " + enumRef);
                    //m_classReferences.RemoveAll(c => c==classRef);
                    m_numErrors++;
                }
            }
            return m_numErrors;
        }
    }
    class SourceProcessor
    {
        public Checker m_checker = new Checker();
        public int numCharsProcessed = 0;
        string extractTokenRegex = @"(?>[^()]+|\((?<open>)|\)(?<-open>))*";
        string extractFuncRegex = @"\b[^()]+\((.*)\)$";
        string extractArgsRegex = @"(?:[^,()]+((?:\(\s+(?>[^()]+|\((?<open>)|\)(?<-open>))*\)))*)+";
        int m_level = 0;
        void increaseIndent() { m_level++; }
        void decreaseIndent() { m_level--; }
        public string getLevelIndent()
        {
            string indentation = "";
            for (int i = 0; i < m_level; i++)
                indentation += "  ";
            return indentation;
        }
        public string processCPPFile(string filename)
        {
            m_level = 1;
            string fileContents = File.ReadAllText(filename, Encoding.UTF8);
            fileContents = fileContents.Replace('\r', ' ');//'\n');
            fileContents = fileContents.Replace('\n', ' ');

            numCharsProcessed += fileContents.Length;

            string parsedXMLFile= parseClasses(fileContents);

            parsedXMLFile += parseEnumerations(fileContents);

            return parsedXMLFile;
        }

        public string resolveInlineClassRefs(string text)
        {
            //<BRANCH Name="Inline" Class="CVFAPolicyLearner"/>
            // -> <CLASS Name="CVFAPolicyLearner" ...> **** </CLASS>

            string input = text;
            string sPattern = @"\s*<BRANCH Name=""Inline"".*?Class=""(\w+)""/>\s*";
            string sPattern2;

            foreach (Match match in Regex.Matches(text, sPattern))
            {
                string referencedClass = match.Groups[1].Value;

                sPattern2 = @"<CLASS Name=""" + referencedClass + @""">(.*?)</CLASS>";
                var definitionMatch = Regex.Match(text, sPattern2,RegexOptions.Singleline);
                string classDefinition = definitionMatch.Groups[1].Value;

                input= input.Replace(match.Groups[0].Value, classDefinition);

            }

            return input;
        }
        string parseEnumerations(string text)
        {

            //#define ENUMERATED_TYPE(name) enum name
            string sPattern = @"ENUMERATION\s*\((.*?)\)";

            string parsedXML = "";

            foreach (Match match in Regex.Matches(text, sPattern))
            {
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = functionArgumentsMatch.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<ENUMERATION Name=\"" + parameterMatches[0].Value.Trim() + "\">\n";
                m_checker.addEnumDefinition(parameterMatches[0].Value.Trim());
                increaseIndent();
                for (int i = 1; i < parameterMatches.Count; i++ )
                {
                    parsedXML += getLevelIndent() + "<VALUE>" + parameterMatches[i].Value.Trim('"',' ') + "</VALUE>\n";
                }
                decreaseIndent();
                parsedXML += getLevelIndent() + "</ENUMERATION>\n";
            }

            return parsedXML;
        }
        string parseChoiceElements(string text)
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

                string referencedClass;
                if (match.Groups[1].Value.Trim(' ') == "CHOICE_ELEMENT")
                    referencedClass = parameterMatches[2].Value.Trim(' ');
                else referencedClass = parameterMatches[2].Value.Trim(' ') + "-Factory";
                m_checker.addClassReference(referencedClass);
                parsedXML += getLevelIndent() + "<CHOICE-ELEMENT Name=" + parameterMatches[1].Value.Trim(' ') + " Class=\""
                        + referencedClass + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseChoiceElementsLoadXML(string text)
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
                        + parameterMatches[2].Value.Trim(' ') + "\" XML=" + parameterMatches[3].Value.Trim(' ') + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseChoices(string text)
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
        string parseEnumValues(string text)
        {
            increaseIndent();
            //#define ENUM_VALUE(variable,typeName,parameterNode,parameterName,defaultValue)
            // -> <ENUM_VALUE Name="parameterName" Class="typeName"/>
            string sPattern = @"ENUM_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                m_checker.addEnumReference(parameterMatches[1].Value.Trim(' '));

                parsedXML += getLevelIndent() + "<ENUM-VALUE Name=" + parameterMatches[3].Value.Trim(' ')
                    + " Class=\"" + parameterMatches[1].Value.Trim(' ') 
                    + "\" Default=" + parameterMatches[4].Value.Trim(' ')
                    + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseConstIntegerValues(string text)
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
        string parseConstDoubleValues(string text)
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
        string parseConstBooleanValues(string text)
        {
            increaseIndent();
            //#define CONST_BOOLEAN_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstBoolean(parameterName,defaultValue);
            // -> <ENUM-VALUE Class="BOOLEAN" Name="Log-eval-episodes" Default="true" Comment="Log evaluation episodes?"/>
            string sPattern = @"CONST_BOOLEAN_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<ENUM-VALUE Class=\"BOOLEAN\" Name=" + parameterMatches[2].Value.Trim(' ') 
                    + " Default=\"" + parameterMatches[3].Value.Trim(' ')
                    + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseConstStringValues(string text)
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
        string parseFilePathValues(string text)
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
        string parseDirPathValues(string text)
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
        string parseStateVariableRefs(string text)
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
        string parseActionVariableRefs(string text)
        {
            increaseIndent();
            //#define ACTION_VARIABLE_REF(variable,parameterNode,variableName) 
            // ->     <XML-NODE-REF Name="Output-Action" XMLFile="WORLD-DEFINITION" HangingFrom="Action"/>

            string sPattern = @"ACTION_VARIABLE_REF\s*\(" + extractTokenRegex + @"\)";

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
        string parseMultiValues(string text)
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

                string referencedClass;
                if (match.Groups[1].Value.Trim(' ') == "MULTI_VALUED")
                    referencedClass = parameterMatches[2].Value.Trim(' ');
                else referencedClass = parameterMatches[2].Value.Trim(' ') + "-Factory";

                m_checker.addClassReference(referencedClass);
                
                parsedXML += getLevelIndent() + "<MULTI-VALUED Name=" + parameterMatches[1].Value.Trim(' ')
                        + " Class=\"" + referencedClass+ "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseChildClasses(string text)
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

                string referencedClass;
                if (match.Groups[1].Value.Trim(' ') == "CHILD_CLASS")
                    referencedClass= parameterMatches[2].Value.Trim(' ');
                else referencedClass= parameterMatches[2].Value.Trim(' ') + "-Factory";

                m_checker.addClassReference(referencedClass);
                parsedXML += getLevelIndent() + "<BRANCH Name=" + parameterMatches[1].Value.Trim(' ') + " Class=\""  + referencedClass + "\"/>\n";
                
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseNumericValues(string text)
        {
            increaseIndent();
            //#define NUMERIC_VALUE(variable,parameterNode,parameterName)
             string sPattern = @"NUMERIC_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<BRANCH Name=" + parameterMatches[2].Value.Trim(' ')
                    + " Class=\"CNumericValue-Factory\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseClassExtensions(string text)
        {
            increaseIndent();
            //#define EXTENDS(className,...)
            // -> <BRANCH Name="" Class=""/>
            string sPattern = @"(EXTENDS)\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[1].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(parameterMatches[0].Value.Trim(' '));
                parsedXML += getLevelIndent() + "<BRANCH Name=\"Inline\" Class=\""  + parameterMatches[0].Value.Trim(' ') + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseClasses(string text)
        {
            string sPattern = @"(CLASS_CONSTRUCTOR|CLASS_FACTORY)(.*?)END_CLASS\(\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, @"(CLASS_CONSTRUCTOR|CLASS_FACTORY)\s*\(" + extractTokenRegex + @"\)");
                string header = functionArgumentsMatch.Groups[0].Value;

                var parameterMatches = Regex.Matches(header, extractArgsRegex);

                string definedClass;
                if (parameterMatches[0].Value.Trim(' ') == "CLASS_CONSTRUCTOR")
                    definedClass= parameterMatches[1].Value;
                else definedClass= parameterMatches[1].Value + "-Factory";

                m_checker.addClassDefinition(definedClass);
                parsedXML += getLevelIndent() + "<CLASS Name=\"" + definedClass + "\">\n";
                
                string classDefinition= match.Groups[2].Value;
                parsedXML += parseStateVariableRefs(classDefinition);
                parsedXML += parseActionVariableRefs(classDefinition);
                parsedXML += parseConstIntegerValues(classDefinition);
                parsedXML += parseConstDoubleValues(classDefinition);
                parsedXML += parseConstStringValues(classDefinition);
                parsedXML += parseDirPathValues(classDefinition);
                parsedXML += parseFilePathValues(classDefinition);

                parsedXML += parseConstBooleanValues(classDefinition);

                parsedXML += parseEnumValues(classDefinition);
                parsedXML += parseMultiValues(classDefinition);

                parsedXML += parseChoices(classDefinition);
                parsedXML += parseChildClasses(classDefinition);

                parsedXML += parseNumericValues(classDefinition);





                parsedXML += parseClassExtensions(classDefinition);

                parsedXML += getLevelIndent() + "</CLASS>\n\n";      
            }
            return parsedXML;
        }
    }
}
