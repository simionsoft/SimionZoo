

namespace Badger.Simion
{
    public class XMLConfig
    {
        public const string experimentConfigVersion = "1.0.0.0";
        public const string BadgerProjectConfigVersion = "1.0.0.0";
        public const string BatchConfigVersion = "1.0.0.0";

        public const string definitionNodeTag = "DEFINITIONS";
        public const string classDefinitionNodeTag = "CLASS";
        public const string enumDefinitionNodeTag = "ENUMERATION";
        public const string worldDefinitionNodeTag = "WORLD";

        //App-level tags: APP, PRE, EXE, INCLUDE
        public const string badgerNodeTag = "BADGER-PROJECT";
        public const string experimentNodeTag = "EXPERIMENT"; //a experiment which can have forks within
        public const string experimentalUnitNodeTag = "EXPERIMENTAL-UNIT"; //an experiment within a batch without any forks
        public const string experimentBatchNodeTag = "EXPERIMENT-BATCH";
        public const string appNodeTag = "App";
        public const string statisticsFileTag = "STATISTICS";

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
        public const string neuralNetworkNodeTag = "NEURAL-NETWORK";
        public const string dirPathNodeTag = "DIR-PATH-VALUE";
        public const string enumNodeTag = "ENUM-VALUE";
        public const string multiValuedNodeTag = "MULTI-VALUED";
        public const string xmlRefNodeTag = "XML-NODE-REF";
        public const string forkTag = "FORK";
        public const string forkedNodeTag = "FORKED-NODE";
        public const string forkValueTag = "FORK-VALUE";
        public const string linkedNodeTag = "LINKED-NODE";
        public const string linkOriginNodeTag = "LINK-ORIGIN";
        public const string stateVarRefTag = "STATE-VAR-REF";
        public const string actionVarRefTag = "ACTION-VAR-REF";
        public const string stateVarTag = "STATE-VAR";
        public const string actionVarTag = "ACTION-VAR";
        public const string constantTag = "CONSTANT";
        public const string statVariableTag = "VARIABLE";
        public const string statVariableItemTag = "STATISTIC-ITEM";
        public const string PrerequisiteTag = "PREREQUISITE";
        //Wires
        public const string WireTag = "Wire";
        //Attributes
        public const string nameAttribute = "Name";
        public const string aliasAttribute = "Alias";
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
        public const string groupIdAttribute = "Group";
        public const string trackIdAttribute = "Track";
        public const string originNodeAttribute = "LinkOriginNode"; // name of the origin node in the link target node tag
        public const string ExeFileNameAttr = "ExeFile";
        //Attribute special values
        public const string newWindowValue = "New";

        //the name of the configuration class used in the C++ source code
        public const string srcConfigClassName = "ConfigNode";
        //Text colors
        public const string colorDefaultValue = "DarkOrange";
        public const string colorValidValue = "Gray";
        public const string colorInvalidValue = "Red";

        //Parameter sorting values
        public const int sortingValueOptional = 150;
        public const int sortingValueSimpleParameter = 0;
        public const int sortingValueChildClass = 50;
        public const int sortingValueMultiParameter = 100;
        public const int sortingValueWorldVariable = 200;

        //Log descriptors/files
        public const string descriptorRootNodeName = "ExperimentLogDescriptor";
        public const string descriptorStateVarNodeName = "State-variable";
        public const string descriptorActionVarNodeName = "Action-variable";
        public const string descriptorRewardVarNodeName = "Reward-variable";
        public const string descriptorStatVarNodeName = "Stat-variable";
    }
}
