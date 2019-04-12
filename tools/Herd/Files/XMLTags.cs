/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/



namespace Herd.Files
{
    public class XMLTags
    {
        public const string ExperimentConfigVersion = "1.0.0.0";
        public const string BadgerProjectConfigVersion = "1.0.0.0";
        public const string BatchConfigVersion = "1.0.0.0";

        public const string DefinitionNodeTag = "DEFINITIONS";
        public const string ClassDefinitionNodeTag = "CLASS";
        public const string EnumDefinitionNodeTag = "ENUMERATION";
        public const string WorldDefinitionNodeTag = "WORLD";

        //App-level tags: APP, PRE, EXE, INCLUDE
        public const string BadgerNodeTag = "BADGER-PROJECT";
        public const string ExperimentNodeTag = "EXPERIMENT"; //a experiment which can have forks within
        public const string ExperimentalUnitNodeTag = "EXPERIMENTAL-UNIT"; //an experiment within a batch without any forks
        public const string ExperimentBatchNodeTag = "EXPERIMENT-BATCH";
        public const string ReportNodeTag = "REPORT";
        public const string QueryNodeTag = "QUERY";
        public const string PlotNodeTag = "PLOT";
        public const string AppNodeTag = "App";
        public const string StatisticsFileTag = "STATISTICS";

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
        public const string LineSeriesTag = "LINE-SERIES";
        public const string DataPointTag = "DATA-POINT";
        public const string DataPointXTag = "X";
        public const string DataPointYTag = "Y";
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
        public const string BatchFilenameAttr = "Batch";
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
        public const string sampleFiledescriptorRootNodeName = "SampleFileDescriptor";
        public const string sampleFileNumSamplesAttr = "NumSamples";

        public const string descriptorRootNodeName = "ExperimentLogDescriptor";
        public const string descriptorBinaryDataFile = "BinaryDataFile";
        public const string descriptorFunctionsDataFile = "FunctionsDataFile";
        public const string descriptorSceneFile = "SceneFile";

        public const string descriptorStateVarNodeName = "State-variable";
        public const string descriptorActionVarNodeName = "Action-variable";
        public const string descriptorRewardVarNodeName = "Reward-variable";
        public const string descriptorStatVarNodeName = "Stat-variable";
    }
}
