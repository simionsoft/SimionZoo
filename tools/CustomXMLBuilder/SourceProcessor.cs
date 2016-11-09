using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;


namespace CustomXMLBuilder
{
    public class classReference : IEquatable<classReference>
    {
        public string className;
        public string file;
        public int line;
        public classReference(string c, string f, int l)
        { 
            className = c;
            file = f;
            line = l;
        }
        public bool Equals(classReference c)
        {
            if (className == c.className)
                return true;
            return false;
        }
    }
    class Checker
    {
        private List<classReference> m_classDefinitions;
        private List<classReference> m_classReferences;
        private List<classReference> m_enumDefinitions;
        private List<classReference> m_enumReferences;
        private int m_numErrors= 0;
        private int m_numWarnings = 0;

        public void addClassDefinition(classReference className)
        {
            m_classDefinitions.Add(className);}
        public void addClassReference(classReference className)
        {
            m_classReferences.Add(className);}
        public void addEnumDefinition(classReference enumName)
        { 
            m_enumDefinitions.Add(enumName); }
        public void addEnumReference(classReference enumName)
        {
            m_enumReferences.Add(enumName); }
        public Checker()
        {
            m_classDefinitions = new List<classReference>();
            m_classReferences = new List<classReference>();
            m_enumDefinitions = new List<classReference>();
            m_enumReferences = new List<classReference>();
        }
        public int checkClassReferences()
        {
            foreach (classReference classRef in m_classReferences)
            {
                if (classRef.className!="" && !m_classDefinitions.Contains(classRef))
                {
                    Console.WriteLine("../config/definitions.xml : error: Reference to undefined class found: " + classRef.className);
                    //m_classReferences.RemoveAll(c => c==classRef);
                    m_numErrors++;
                }
            }
            foreach (classReference enumRef in m_enumReferences)
            {
                if (!m_enumDefinitions.Contains(enumRef))
                {
                    Console.WriteLine("../config/definitions.xml : error : Reference to undefined enumerated type found: " + enumRef.className);
                    //m_classReferences.RemoveAll(c => c==classRef);
                    m_numErrors++;
                }
            }
            foreach (classReference classDef in m_classDefinitions)
            {
                if (!m_classReferences.Contains(classDef))
                    Console.WriteLine("../config/definitions.xml : warning: Class {0} defined but not referenced ",classDef.className);
                m_numWarnings++;
            }
            return m_numErrors;
        }
    }
    class SourceProcessor
    {
        string m_currentFile;
        public Checker m_checker = new Checker();
        public int numCharsProcessed = 0;
        string extractTokenRegex = @"(?>[^()]+|\((?<open>)|\)(?<-open>))*";
        string extractFuncRegex = @"(.+?)\((.*)\)$";
        string extractArgsRegex = @"(?:[^,()]+((?:\((?>[^""()]+|\((?<open>)|\)(?<-open>))*\)))*)+";
            //@"(?:[^,()]+((?:\(\s+(?>[^()]+|\((?<open>)|\)(?<-open>))*\)))*)+";
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
            m_currentFile = filename;
            string fileContents = File.ReadAllText(filename, Encoding.UTF8);
            fileContents = fileContents.Replace('\r', ' ');//'\n');
            fileContents = fileContents.Replace('\n', ' ');

            numCharsProcessed += fileContents.Length;

            //first, enumerations to avoid references to yet undefined enumerations
            string parsedXMLFile= parseEnumerations(fileContents);
            //then classes
            parsedXMLFile += parseClasses(fileContents);

            return parsedXMLFile;
        }

        public string resolveInlineClassRefs(string text)
        {

            string input = text;
            //We remove inline classes with no child-class name (EXTENDS)
            //
            //<BRANCH Name="Inline" Class="CVFAPolicyLearner"/>
            // -> <CLASS Name="CVFAPolicyLearner" ...> **** </CLASS>
            string sPattern = @"\s*<BRANCH Name=""Inline"".*?Class=""([^""]+)""(.*?)/>\s*";
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
                string arguments = functionArgumentsMatch.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<ENUMERATION Name=\"" + parameterMatches[0].Value.Trim() + "\">\n";
                m_checker.addEnumDefinition(new classReference(parameterMatches[0].Value.Trim(),m_currentFile,0));
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
            //#define CHOICE_ELEMENT(checkLiteral,className,comment)
            //#define CHOICE_ELEMENT_FACTORY(checkLiteral, className,comment)
            //-> <CHOICE Name="" Comment =""><CHOICE-ELEMENT Name="".../></CHOICE>

            string sPattern = @"(CHOICE_ELEMENT|CHOICE_ELEMENT_FACTORY|CHOICE_ELEMENT_INLINE|CHOICE_ELEMENT_INLINE_FACTORY)\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = functionArgumentsMatch.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                string referencedClass;
                if (match.Groups[1].Value.Trim(' ').EndsWith("_FACTORY"))
                    referencedClass = parameterMatches[1].Value.Trim(' ') + "-Factory";
                else referencedClass = parameterMatches[1].Value.Trim(' ');
                
                m_checker.addClassReference(new classReference(referencedClass,m_currentFile,0));
                parsedXML += getLevelIndent() + "<CHOICE-ELEMENT Name=" + parameterMatches[0].Value.Trim(' ') + " Class=\""
                        + referencedClass + "\" Comment=" + parameterMatches[2].Value.Trim(' ') + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseChoiceElementsLoadXML(string text)
        {
            increaseIndent();
            //#define CHOICE_ELEMENT_XML(checkLiteral,className,XMLFilename,comment)
            string sPattern = @"CHOICE_ELEMENT_XML\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = functionArgumentsMatch.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(new classReference(parameterMatches[1].Value.Trim(' '),m_currentFile,0));
                parsedXML += getLevelIndent() + "<CHOICE-ELEMENT Name=" + parameterMatches[0].Value.Trim(' ') + " Class=\""
                        + parameterMatches[1].Value.Trim(' ') + "\" XML=" + parameterMatches[2].Value.Trim(' ')
                        + " Comment=" + parameterMatches[3].Value.Trim(' ') + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseChoices(string text)
        {
            //#define CHOICE(name,comment)
            //#define CHOICE_XML(name,loadXML,comment)
            //#define END_CHOICE()
            increaseIndent();

            string sPattern = @"(CHOICE|CHOICE_INLINE)\s*\(" + extractTokenRegex + @"\)(.*?)END_CHOICE";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, @"(CHOICE|CHOICE_INLINE)\(" + extractTokenRegex + @"\)");
                string arguments = functionArgumentsMatch.Groups[0].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                parsedXML += getLevelIndent() + "<CHOICE Name=" + parameterMatches[1].Value;

                parsedXML+=" Comment=" + parameterMatches[2].Value;
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
            //#define ENUM_VALUE(variable,typeName,parameterName,defaultValue,comment)
            // -> <ENUM_VALUE Name="parameterName" Class="typeName" Comment=""/>
            string sPattern = @"ENUM_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                m_checker.addEnumReference(new classReference(parameterMatches[1].Value.Trim(' '),m_currentFile,0));

                parsedXML += getLevelIndent() + "<ENUM-VALUE Name=" + parameterMatches[2].Value.Trim(' ')
                    + " Class=\"" + parameterMatches[1].Value.Trim(' ') 
                    + "\" Default=" + parameterMatches[3].Value.Trim(' ')
                    + " Comment=" + parameterMatches[4].Value.Trim(' ')
                    + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseConstantParameters(string text)
        {
            increaseIndent();
            //#define CONST_INTEGER_VALUE(variable,parameterName,defaultValue,comment)
            //#define CONST_DOUBLE_VALUE(variable,parameterName,defaultValue,comment)
            //...
            string sPattern = @"(CONST_INTEGER|CONST_DOUBLE|CONST_STRING|FILE_PATH|DIR_PATH)_VALUE\s*?\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

               
                string outputClassName;
                switch (match.Groups[1].Value)
                {
                    case "CONST_INTEGER": outputClassName = "INTEGER-VALUE"; break;
                    case "CONST_DOUBLE": outputClassName = "DOUBLE-VALUE"; break;
                    case "CONST_STRING": outputClassName = "STRING-VALUE"; break;
                    case "FILE_PATH": outputClassName = "FILE-PATH-VALUE"; break;
                    case "DIR_PATH": outputClassName = "DIR-PATH-VALUE"; break;
                    default: outputClassName = ""; break;
                }
                parsedXML += getLevelIndent() + "<" + outputClassName + " Name=" + parameterMatches[1].Value.Trim(' ');
                if (outputClassName == "INTEGER-VALUE" || outputClassName == "DOUBLE-VALUE")
                    parsedXML += " Default=\"" + parameterMatches[2].Value.Trim(' ') + "\"";
                else parsedXML += " Default=" + parameterMatches[2].Value.Trim(' ');
                    
                parsedXML+= " Comment="  + parameterMatches[3].Value.Trim(' ') + "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
      
        string parseVariableRefs(string text)
        {
            increaseIndent();
            //#define STATE_VARIABLE_REF(variable,variableName,comment)
            //#define ACTION_VARIABLE_REF(variable,variableName,comment)
            // ->     <XML-NODE-REF Name="Variable" XMLFile="WORLD-DEFINITION" HangingFrom="State/Action"/>

            string sPattern = @"(STATE|ACTION)_VARIABLE_REF\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                string hangingFrom;
                if (match.Groups[1].Value == "STATE")
                    hangingFrom = "State";
                else
                    hangingFrom = "Action";

                parsedXML += getLevelIndent() + "<XML-NODE-REF Name=" + parameterMatches[1].Value.Trim(' ')
                    + " XMLFile=\"WORLD-DEFINITION\" HangingFrom=\"" + hangingFrom + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
  
        string parseMultiBasicType(string text)
        {
            increaseIndent();
            //#define MULTI_VALUED_STRING(variable,parameterName,comment,defaultValue,parameterNode) 
            //#define MULTI_VALUED_DOUBLE(variable,parameterName,comment,defaultValue,parameterNode)
            //#define MULTI_VALUED_INTEGER(variable,parameterName,comment,defaultValue,parameterNode)
            //#define MULTI_VALUED_FILE_PATH(variable,parameterName,comment,defaultValue,parameterNode)
            //#define MULTI_VALUED_STATE_VAR_REF(variable,parameterName,comment,parameterNode)
            //#define MULTI_VALUED_ACTION_VAR_REF(variable,parameterName,comment,parameterNode)

            // -> <MULTI-VALUED Name="RBF-State-Grid-Dimension" Class="STRING-VALUE"/>
            string sPattern = @"MULTI_VALUED_(STRING|DOUBLE|INTEGER|FILE_PATH|STATE_VAR_REF|ACTION_VAR_REF)\(" 
                + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                string referencedBasicType,HangingFrom="";
                switch (match.Groups[1].Value.Trim(' '))
                {
                    case "STRING": referencedBasicType = "STRING-VALUE"; break;
                    case "DOUBLE": referencedBasicType = "DOUBLE-VALUE"; break;
                    case "INTEGER": referencedBasicType = "INTEGER-VALUE"; break;
                    case "FILE_PATH": referencedBasicType = "FILE-PATH-VALUE"; break;
                    case "STATE_VAR_REF": referencedBasicType = "XML-NODE-REF"; HangingFrom = "State"; break;
                    case "ACTION_VAR_REF": referencedBasicType = "XML-NODE-REF"; HangingFrom="Action";break;
                    default: referencedBasicType = ""; break;
                }

                parsedXML += getLevelIndent() + "<MULTI-VALUED Name=" + parameterMatches[1].Value.Trim(' ')
                        + " Class=\"" + referencedBasicType + "\" Comment=" + parameterMatches[2].Value.Trim(' ');
                if (HangingFrom != "") parsedXML += " HangingFrom=\"" + HangingFrom + "\"";
                else
                {
                    if (referencedBasicType=="STRING-VALUE" || referencedBasicType=="FILE-PATH-VALUE")
                        parsedXML += " Default=" + parameterMatches[3].Value.Trim(' ');
                    else parsedXML += " Default=\"" + parameterMatches[3].Value.Trim(' ') + "\"";
                }
                 parsedXML += "/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseMultiValues(string text)
        {
            increaseIndent();
            //#define MULTI_VALUE(indexedVariable,name,comment,className,...)
            //#define MULTI_VALUE_FACTORY(indexedVariable,name,comment,className,...)
            // -> <MULTI-VALUED Name="RBF-State-Grid-Dimension" Class="RBF-STATE-GRID-DIMENSION"/>
            string sPattern = @"(MULTI_VALUED_FACTORY|MULTI_VALUED)\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                string referencedClass;
                if (match.Groups[1].Value.Trim(' ') == "MULTI_VALUED")
                    referencedClass = parameterMatches[3].Value.Trim(' ');
                else referencedClass = parameterMatches[3].Value.Trim(' ') + "-Factory";

                m_checker.addClassReference(new classReference(referencedClass,m_currentFile,0));
                
                parsedXML += getLevelIndent() + "<MULTI-VALUED Name=" + parameterMatches[1].Value.Trim(' ')
                        + " Class=\"" + referencedClass+ "\" Comment=" + parameterMatches[2].Value.Trim(' ') +"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseChildClasses(string text)
        {
            increaseIndent();
            //#define CHILD_CLASS(variable,name,comment,optional,className,constructorParameters,...)
            //#define CHILD_CLASS_FACTORY(variable,name,comment,optional,className,constructorParameters,...)
            //#define CHILD_CLASS_INIT(variable,name,comment,optional,...)
            // -> <BRANCH Name="" Class=""/>
            string sPattern = @"(CHILD_CLASS|CHILD_CLASS_FACTORY|CHILD_CLASS_INIT)\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);

                string referencedClass;
                if (match.Groups[1].Value.Trim(' ') == "CHILD_CLASS" || match.Groups[1].Value.Trim(' ') == "CHILD_CLASS_INIT")
                    referencedClass= parameterMatches[4].Value.Trim(' ');
                else referencedClass= parameterMatches[4].Value.Trim(' ') + "-Factory";

                m_checker.addClassReference(new classReference(referencedClass,m_currentFile, 0));

                parsedXML += getLevelIndent() + "<BRANCH Name=" + parameterMatches[1].Value.Trim(' ')
                    + " Class=\"" + referencedClass + "\" Comment=" + parameterMatches[2].Value.Trim(' ');
                if (parameterMatches[3].Value.Trim(' ') != "\"\"") //Window
                    parsedXML += " Optional=\"" + parameterMatches[3].Value.Trim(' ') + "\"";
                parsedXML+= "/>\n";                
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseNumericValues(string text)
        {
            increaseIndent();
            //#define NUMERIC_VALUE(variable,parameterName,comment)
             string sPattern = @"NUMERIC_VALUE\s*\(" + extractTokenRegex + @"\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var parameters = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(new classReference("CNumericValue-Factory", m_currentFile, 0));

                parsedXML += getLevelIndent() + "<BRANCH Name=" + parameterMatches[1].Value.Trim(' ')
                    + " Class=\"CNumericValue-Factory\"" + " Comment=" + parameterMatches[2].Value.Trim(' ') + "/>\n";
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
                string arguments = parameters.Groups[2].Value;

                var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                m_checker.addClassReference(new classReference(parameterMatches[0].Value.Trim(' '),m_currentFile,0));
                parsedXML += getLevelIndent() + "<BRANCH Name=\"Inline\" Class=\""  + parameterMatches[0].Value.Trim(' ') + "\"/>\n";
            }
            decreaseIndent();
            return parsedXML;
        }
        string parseClasses(string text)
        {
            //#define CLASS_FACTORY(name,...) name* name::getInstance(CParameters* pParameters,__VA_ARGS__)
            //#define CLASS_CONSTRUCTOR(name,...) name::name(CParameters* pParameters,__VA_ARGS__)
            //#define CLASS_INIT(name,...) void name::init(CParameters* pParameters,__VA_ARGS__)

            //#define CLASS_FACTORY_NEW_WINDOW(name,...) name* name::getInstance(CParameters* pParameters,__VA_ARGS__)
            //#define CLASS_CONSTRUCTOR_NEW_WINDOW(name,...) name::name(CParameters* pParameters,__VA_ARGS__)
            //#define CLASS_INIT_NEW_WINDOW(name,...) void name::init(CParameters* pParameters,__VA_ARGS__)
            string sPattern = @"(APP_CLASS|CLASS_CONSTRUCTOR|CLASS_FACTORY|CLASS_INIT|CLASS_FACTORY_NEW_WINDOW|CLASS_CONSTRUCTOR_NEW_WINDOW|CLASS_INIT_NEW_WINDOW)(.*?)END_CLASS\(\)";

            string parsedXML = "";
            foreach (Match match in Regex.Matches(text, sPattern))
            {
                //Console.WriteLine(match.Value);
                var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, @"(APP_CLASS|CLASS_CONSTRUCTOR|CLASS_FACTORY|CLASS_INIT|CLASS_FACTORY_NEW_WINDOW|CLASS_CONSTRUCTOR_NEW_WINDOW|CLASS_INIT_NEW_WINDOW)\s*\(" + extractTokenRegex + @"\)");
                string header = functionArgumentsMatch.Groups[0].Value;

                var functionMatches = Regex.Match(header, extractFuncRegex);

                var parameterMatches = Regex.Match(functionMatches.Groups[2].Value, extractArgsRegex);
                string definedClass;
                if (functionMatches.Groups[1].Value.Trim(' ') != "CLASS_FACTORY"
                    && functionMatches.Groups[1].Value != "CLASS_FACTORY_NEW_WINDOW")
                    definedClass = parameterMatches.Groups[0].Value;
                else definedClass = parameterMatches.Groups[0].Value + "-Factory";

                //if the class defines an App we don't want to check whether is referenced or not
                if (functionMatches.Groups[1].Value.Trim(' ') != "APP_CLASS")
                    m_checker.addClassDefinition(new classReference(definedClass,m_currentFile,0));

                parsedXML += getLevelIndent() + "<CLASS Name=\"" + definedClass + "\"";

                if (functionMatches.Groups[1].Value == "CLASS_FACTORY_NEW_WINDOW"
                    || functionMatches.Groups[1].Value == "CLASS_CONSTRUCTOR_NEW_WINDOW" 
                    || functionMatches.Groups[1].Value == "CLASS_INIT_NEW_WINDOW")
                    parsedXML += " Window=\"New\"";
                parsedXML += ">\n";

                string classDefinition = match.Groups[2].Value;
                parsedXML += parseVariableRefs(classDefinition);

                parsedXML += parseConstantParameters(classDefinition);

                parsedXML += parseEnumValues(classDefinition);
                parsedXML += parseMultiValues(classDefinition);
                parsedXML += parseMultiBasicType(classDefinition);

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
