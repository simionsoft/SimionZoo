#ifndef __GLOBALS_H__
#define __GLOBALS_H__

class CLogger;
class CWorld;
class CExperiment;
class CSimGod;
class CParameters;
class CParameterFile;

namespace RLSimion
{
	extern CLogger *g_pLogger;
	extern CWorld *g_pWorld;
	extern CExperiment *g_pExperiment;
	extern CSimGod *g_pSimGod;

	void init(int argc, char* argv[]);
	void shutdown();
};

//MACROS USED TO PRODUCE THE CONFIGURATION FILES
#define CLASS_FACTORY(name,...) name* name::getInstance(CParameters* pParameters,__VA_ARGS__)
#define CLASS_CONSTRUCTOR(name,...) name::name(CParameters* pParameters,__VA_ARGS__)
#define END_CLASS()

//The superclass' parameters are embedded inline within the subclass' definition, so the superclass' parameters
//should not include any hierarchy (no getChild)
#define EXTENDS(className,...) className(__VA_ARGS__)

//The child class is given a name according to the context from the parent class, so the parameter node should include
//the appropriate hierarchy (i.e., pParameters->getChild("VFA"))
#define CHILD_CLASS(variable,name,className,pParameterNode,...) variable= new className(pParameterNode->getChild(name),__VA_ARGS__);
#define CHILD_CLASS_FACTORY(variable,name,className,pParameterNode,...) variable= className::getInstance(pParameterNode->getChild(name),__VA_ARGS__);

#define CHOICE(name) CParameters* pChild = pParameters->getChild(name)->getChild();const char* type = pChild->getName();
#define CHOICE_XML(name,loadXML) CParameters* pChild = pParameters->getChild(name)->getChild();const char* type = pChild->getName();
#define END_CHOICE()

#define CHOICE_ELEMENT_XML(checkLiteral,className,XMLFilename) if(!strcmp(type,checkLiteral)) return new className(pChild,XMLFilename);
#define CHOICE_ELEMENT(checkLiteral,className) if(!strcmp(type,checkLiteral)) return new className(pChild);
#define CHOICE_ELEMENT_FACTORY(checkLiteral, className) if(!strcmp(type,checkLiteral)) return className::getInstance(pChild);

#define NUMERIC_VALUE(variable,parameterNode,parameterName) variable= CNumericValue::getInstance(parameterNode->getChild(parameterName));
#define CONST_INTEGER_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstInteger(parameterName,defaultValue);
//#define CONST_BOOLEAN_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstBoolean(parameterName,defaultValue);
#define CONST_STRING_VALUE(variable,parameterNode,parameterName,defaultValue) variable= (char*)parameterNode->getConstString(parameterName,defaultValue);
//#define CONST_STRING_VALUE_OPTIONAL(variable,parameterNode,parameterName) if(parameterNode->getChild(parameterName)) {variable= (char*)parameterNode->getChild(parameterName)->getConstString(parameterName);} else {variable= 0;}
#define CONST_DOUBLE_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstDouble(parameterName,defaultValue);

#define MULTI_VALUED(indexedVariable,name,className,...) indexedVariable= new className(__VA_ARGS__);
#define MULTI_VALUED_FACTORY(indexedVariable,name,className,...) indexedVariable= className::getInstance(__VA_ARGS__);

#define MULTI_VALUED_STRING(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstString(defaultValue);
#define MULTI_VALUED_DOUBLE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstDouble(defaultValue);
#define MULTI_VALUED_INTEGER(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstInteger(defaultValue);
#define MULTI_VALUED_FILE_PATH(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstString(defaultValue);
#define MULTI_VALUED_STATE_VAR_REF(variable,parameterNode,parameterName,defaultValue) variable= CWorld::getDynamicModel()->getStateDescriptor()->getVarIndex(parameterNode->getConstString(variableName));
#define MULTI_VALUED_ACTION_VAR_REF(variable,parameterNode,parameterName,defaultValue) CWorld::getDynamicModel()->getActionDescriptor()->getVarIndex(parameterNode->getConstString(variableName));

#define STATE_VARIABLE_REF(variable,parameterNode,variableName) variable= CWorld::getDynamicModel()->getStateDescriptor()->getVarIndex(parameterNode->getConstString(variableName));
#define ACTION_VARIABLE_REF(variable,parameterNode,variableName) variable= CWorld::getDynamicModel()->getActionDescriptor()->getVarIndex(parameterNode->getConstString(variableName));

#define DIR_PATH_VALUE(variable,parameterNode,variableName,defaultValue) variable= (char*)parameterNode->getConstString(variableName,defaultValue);
#define FILE_PATH_VALUE(variable,parameterNode,variableName,defaultValue) variable= (char*)parameterNode->getConstString(variableName,defaultValue);

#define ENUMERATION(typeName,...) const char* typeName[]= {__VA_ARGS__};
#define ENUM_VALUE(variable,typeName,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstString(parameterName,defaultValue);



#endif