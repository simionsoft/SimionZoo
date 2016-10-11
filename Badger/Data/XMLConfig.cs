

namespace Simion
{
    public class XMLConfig
    {
        public const string definitionNodeTag = "DEFINITIONS";
        public const string classDefinitionNodeTag = "CLASS";
        public const string enumDefinitionNodeTag = "ENUMERATION";

        //App-level tags: APP, PRE, EXE, INCLUDE
        public const string appNodeTag = "APP";

        //Class-level tags: BRANCH, CHOICE, CHOICE-ELEMENT, DOUBLE-VALUE, INTEGER-VALUE, STRING-VALUE
        //FILE-PATH-VALUE, DIR-PATH-VALUE, ENUM-VALUE, MULTI-VALUED, XML-NODE-REF
        public const string branchNodeTag = "BRANCH";
        public const string choiceNodeTag = "CHOICE";
        public const string choiceElementNodeTag = "CHOICE-ELEMENT";
        public const string doubleNodeTag = "DOUBLE-VALUE";
        public const string integerNodeTag = "INTEGER-VALUE";
        public const string stringNodeTag = "STRING-VALUE";
        public const string filePathNodeTag = "FILE-PATH-VALUE";
        public const string dirPathNodeTag = "DIR-PATH-VALUE";
        public const string enumNodeTag = "ENUM-VALUE";
        public const string multiValuedNodeTag = "MULTI-VALUED";
        public const string xmlRefNodeTag = "XML-NODE-REF";

        //Attributes
        public const string nameAttribute = "Name";
        public const string commentAttribute = "Comment";
        public const string classAttribute = "Class";
        public const string windowAttribute = "Window";
        public const string defaultAttribute = "Default";
        public const string optionalAttribute = "Optional";
        public const string xmlDefinitionIdAttribute = "LoadXML";
        public const string loadXMLFileAttribute = "XML";
        public const string hangingFromAttribute = "HangingFrom";

        //Attribute special values
        public const string newWindowValue = "New";
        public const string exeNodeTag = "EXE";
        public const string preNodeTag = "PRE";
        public const string includeNodeTag = "INCLUDE";

        //Text colors
        public const string colorDefaultValue = "DarkOrange";
        public const string colorValidValue = "Gray";
        public const string colorInvalidValue = "Red";
    }
}
