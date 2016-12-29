

namespace Simion
{
    public class XMLConfig
    {
        public const string experimentConfigVersion = "1.0.0.0";
        public const string BadgerConfigVersion = "1.0.0.0";
        public const string BatchConfigVersion = "1.0.0.0";

        public const string experimentExtension = "experiment";
        public const string experimentBatchExtension = "exp-batch";
        public const string badgerExtension = "badger";

        public const string definitionNodeTag = "DEFINITIONS";
        public const string classDefinitionNodeTag = "CLASS";
        public const string enumDefinitionNodeTag = "ENUMERATION";
        public const string worldDefinitionNodeTag = "WORLD";

        //App-level tags: APP, PRE, EXE, INCLUDE
        public const string badgerNodeTag = "BADGER";
        public const string experimentNodeTag = "EXPERIMENT";
        public const string batchNodeTag = "EXPERIMENT-BATCH";
        public const string appNodeTag = "APP";

        //Class-level tags: BRANCH, CHOICE, CHOICE-ELEMENT, DOUBLE-VALUE, INTEGER-VALUE, STRING-VALUE
        //FILE-PATH-VALUE, DIR-PATH-VALUE, ENUM-VALUE, MULTI-VALUED, XML-NODE-REF
        public const string branchNodeTag = "BRANCH";
        public const string choiceNodeTag = "CHOICE";
        public const string choiceElementNodeTag = "CHOICE-ELEMENT";
        public const string doubleNodeTag = "DOUBLE-VALUE";
        public const string boolNodeTag = "BOOL-VALUE";
        public const string integerNodeTag = "INTEGER-VALUE";
        public const string stringNodeTag = "STRING-VALUE";
        public const string filePathNodeTag = "FILE-PATH-VALUE";
        public const string dirPathNodeTag = "DIR-PATH-VALUE";
        public const string enumNodeTag = "ENUM-VALUE";
        public const string multiValuedNodeTag = "MULTI-VALUED";
        public const string xmlRefNodeTag = "XML-NODE-REF";
        public const string forkTag = "FORK";
        public const string forkedNodeTag = "FORKED-NODE";
        public const string forkValueTag = "FORK-VALUE";
        public const string stateVarRefTag = "STATE-VAR-REF";
        public const string actionVarRefTag = "ACTION-VAR-REF";
        public const string stateVarTag = "STATE-VAR";
        public const string actionVarTag = "ACTION-VAR";
        public const string constantTag = "CONSTANT";
        //Attributes
        public const string nameAttribute = "Name";
        public const string commentAttribute = "Comment";
        public const string classAttribute = "Class";
        public const string windowAttribute = "Window";
        public const string defaultAttribute = "Default";
        public const string optionalAttribute = "Optional";
        public const string hangingFromAttribute = "HangingFrom";
        public const string versionAttribute = "FileVersion";
        public const string pathAttribute = "Path";
        public const string badgerMetadataAttribute = "BadgerMetadata";
        public const string minValueAttribute = "Min";
        public const string maxValueAttribute = "Max";
        public const string unitAttribute = "Unit";
        public const string valueAttribute = "Value";
        public const string worldAttribute = "World";
        //Attribute special values
        public const string newWindowValue = "New";
        public const string exeNodeTag = "EXE";
        public const string preNodeTag = "PRE";
        public const string includeNodeTag = "INCLUDE";

        //the name of the configuration class used in the C++ source code
        public const string srcConfigClassName = "CConfigNode";
        //Text colors
        public const string colorDefaultValue = "DarkOrange";
        public const string colorValidValue = "Gray";
        public const string colorInvalidValue = "Red";
    }
}
