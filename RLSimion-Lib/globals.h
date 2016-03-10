#pragma once

class CLogger;
class CWorld;
class CExperiment;
class CSimGod;
class CParameters;

namespace RLSimion
{
	extern CLogger *g_pLogger;
	extern CWorld *g_pWorld;
	extern CExperiment *g_pExperiment;
	extern CSimGod *g_pSimGod;

	void init(CParameters* pParameters);
	void shutdown();
}

//MACROS USED TO PRODUCE THE CONFIGURATION FILES
#define CLASS_FACTORY(name) name
#define CLASS_CONSTRUCTOR(name) name::name
#define END_CLASS()


#define EXTENDS(className,...) className(__VA_ARGS__)

#define CHILD_CLASS(variable,name,className,constructorParameters,...) variable= new className(constructorParameters,__VA_ARGS__);
#define CHILD_CLASS_FACTORY(variable,name,className,constructorParameters,...) variable= className::getInstance(constructorParameters,__VA_ARGS__);

#define CHOICE(name)
#define END_CHOICE()
#define CHOICE_ELEMENT(checkVariable, checkLiteral,className, ...) if(!strcmp(checkVariable,checkLiteral)) return new className(__VA_ARGS__);
#define CHOICE_ELEMENT_FACTORY(checkVariable, checkLiteral, className, ...) if(!strcmp(checkVariable,checkLiteral)) return className::getInstance(__VA_ARGS__);

#define NUMERIC_VALUE(variable,parameterNode,parameterName) variable= parameterNode->getNumericHandler(parameterName);
#define CONST_INTEGER_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstInteger(parameterName,defaultValue);
#define CONST_BOOLEAN_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstBoolean(parameterName,defaultValue);
#define CONST_STRING_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstString(parameterName,defaultValue);
#define CONST_DOUBLE_VALUE(variable,parameterNode,parameterName,defaultValue) variable= parameterNode->getConstDouble(parameterName,defaultValue);

#define MULTI_VALUED(indexedVariable,name,className,...) indexedVariable= new className(__VA_ARGS__);
#define MULTI_VALUED_FACTORY(indexedVariable,name,className,...) indexedVariable= className::getInstance(__VA_ARGS__);

#define STATE_VARIABLE_REF(variable,parameterNode,variableName) variable= RLSimion::g_pWorld->getStateDescriptor()->getVarIndex(parameterNode->getConstString(variableName));
#define ACTION_VARIABLE_REF(variable,parameterNode,variableName) variable= RLSimion::g_pWorld->getActionDescriptor()->getVarIndex(parameterNode->getConstString(variableName));