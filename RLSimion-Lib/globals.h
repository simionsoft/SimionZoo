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
#define CHILD_CLASS(variable,name,comment,window,className,...) variable= new className(pParameters->getChild(name),__VA_ARGS__);
#define CHILD_CLASS_FACTORY(variable,name,comment,window,className,...) variable= className::getInstance(pParameters->getChild(name),__VA_ARGS__);

#define CHOICE(name,comment) CParameters* pChild = pParameters->getChild(name)->getChild();const char* type = pChild->getName();
#define CHOICE_XML(name,loadXML,comment) CParameters* pChild = pParameters->getChild(name)->getChild();const char* type = pChild->getName();
#define END_CHOICE()

#define CHOICE_ELEMENT_XML(checkLiteral,className,XMLFilename,comment) if(!strcmp(type,checkLiteral)) return new className(pChild,XMLFilename);
#define CHOICE_ELEMENT(checkLiteral,className,comment) if(!strcmp(type,checkLiteral)) return new className(pChild);
#define CHOICE_ELEMENT_FACTORY(checkLiteral, className,comment) if(!strcmp(type,checkLiteral)) return className::getInstance(pChild);

#define NUMERIC_VALUE(variable,parameterName,comment) variable= CNumericValue::getInstance(pParameters->getChild(parameterName));
#define CONST_INTEGER_VALUE(variable,parameterName,defaultValue,comment) variable= pParameters->getConstInteger(parameterName,defaultValue);
#define CONST_STRING_VALUE(variable,parameterName,defaultValue,comment) variable= (char*)pParameters->getConstString(parameterName,defaultValue);
#define CONST_DOUBLE_VALUE(variable,parameterName,defaultValue,comment) variable= pParameters->getConstDouble(parameterName,defaultValue);
#define DIR_PATH_VALUE(variable,variableName,defaultValue,comment) variable= (char*)pParameters->getConstString(variableName,defaultValue);
#define FILE_PATH_VALUE(variable,variableName,defaultValue,comment) variable= (char*)pParameters->getConstString(variableName,defaultValue);

#define MULTI_VALUED(indexedVariable,name,comment,className,...) indexedVariable= new className(__VA_ARGS__);
#define MULTI_VALUED_FACTORY(indexedVariable,name,comment,className,...) indexedVariable= className::getInstance(__VA_ARGS__);

#define MULTI_VALUED_STRING(variable,parameterName,comment,defaultValue,parameterNode) variable= parameterNode->getConstString(defaultValue);
#define MULTI_VALUED_DOUBLE(variable,parameterName,comment,defaultValue,parameterNode) variable= parameterNode->getConstDouble(defaultValue);
#define MULTI_VALUED_INTEGER(variable,parameterName,comment,defaultValue,parameterNode) variable= parameterNode->getConstInteger(defaultValue);
#define MULTI_VALUED_FILE_PATH(variable,parameterName,comment,defaultValue,parameterNode) variable= parameterNode->getConstString(defaultValue);
#define MULTI_VALUED_STATE_VAR_REF(variable,parameterName,defaultValue,comment,parameterNode) variable= CWorld::getDynamicModel()->getStateDescriptor()->getVarIndex(parameterNode->getConstString(variableName));
#define MULTI_VALUED_ACTION_VAR_REF(variable,parameterName,defaultValue,comment,parameterNode) CWorld::getDynamicModel()->getActionDescriptor()->getVarIndex(parameterNode->getConstString(variableName));

#define STATE_VARIABLE_REF(variable,variableName,comment) variable= CWorld::getDynamicModel()->getStateDescriptor()->getVarIndex(pParameters->getConstString(variableName));
#define ACTION_VARIABLE_REF(variable,variableName,comment) variable= CWorld::getDynamicModel()->getActionDescriptor()->getVarIndex(pParameters->getConstString(variableName));

#define ENUMERATION(typeName,...) const char* typeName[]= {__VA_ARGS__};
#define ENUM_VALUE(variable,typeName,parameterName,defaultValue,comment) variable= pParameters->getConstString(parameterName,defaultValue);



#endif