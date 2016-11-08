#ifndef __GLOBALS_H__
#define __GLOBALS_H__






//MACROS USED TO PRODUCE THE CONFIGURATION FILES
#define APP_CLASS(name,...) name::name(int argc, char* argv[],__VA_ARGS__): CApp(argc,argv)
#define CLASS_FACTORY(name,...) name* name::getInstance(CParameters* pParameters,__VA_ARGS__)
#define CLASS_CONSTRUCTOR(name,...) name::name(CParameters* pParameters,__VA_ARGS__)
#define CLASS_INIT(name,...) void name::init(CParameters* pParameters,__VA_ARGS__)

#define END_CLASS()

//The superclass' parameters are embedded inline within the subclass' definition, so the superclass' parameters
//should not include any hierarchy (no getChild)
#define EXTENDS(className,...) className(__VA_ARGS__)

//The child class is given a name according to the context from the parent class, so the parameter node should include
//the appropriate hierarchy (i.e., pParameters->getChild("VFA"))
#define CHILD_CLASS(variable,name,comment,optional,className,...) if (!optional || pParameters->getChild(name)) variable= new className(pParameters->getChild(name),__VA_ARGS__); else variable= new className((CParameters*)0,__VA_ARGS__);
#define CHILD_CLASS_FACTORY(variable,name,comment,optional,className,...) if (!optional || pParameters->getChild(name)) variable= className::getInstance(pParameters->getChild(name),__VA_ARGS__); else variable= className::getInstance((CParameters*)0,__VA_ARGS__);
#define CHILD_CLASS_INIT(variable,name,comment,optional,className,...) if (!optional || pParameters->getChild(name)) variable.init(pParameters->getChild(name),__VA_ARGS__); else variable.init(0,__VA_ARGS__);

#define CHOICE(name,comment) if (!pParameters) return 0; CParameters* pChild = pParameters->getChild(name);
#define END_CHOICE() return 0;

#define CHOICE_INLINE(name,comment) CParameters* pChild= 0;if (pParameters) pChild= pParameters->getChild(name);
#define END_CHOICE_INLINE()


#define CHOICE_ELEMENT_XML(checkLiteral,className,XMLFilename,comment,...) if(pChild->getChild(checkLiteral)) return new className(pChild->getChild(checkLiteral),XMLFilename,__VA_ARGS__);
#define CHOICE_ELEMENT(checkLiteral,className,comment) if (pChild->getChild(checkLiteral)) return new className(pChild->getChild(checkLiteral));
#define CHOICE_ELEMENT_FACTORY(checkLiteral, className,comment,...) if(pChild->getChild(checkLiteral)) return className::getInstance(pChild->getChild(checkLiteral),__VA_ARGS__);
#define CHOICE_ELEMENT_INLINE(checkLiteral,className,comment,variable,value) if (pChild && pChild->getChild(checkLiteral)) variable= value;
#define CHOICE_ELEMENT_INLINE_FACTORY(checkLiteral,className,comment,variable,value) if (pChild && pChild->getChild(checkLiteral)) variable= value;

#define NUMERIC_VALUE(variable,parameterName,comment) variable= CNumericValue::getInstance(pParameters->getChild(parameterName));
#define CONST_INTEGER_VALUE(variable,parameterName,defaultValue,comment) variable= pParameters->getConstInteger(parameterName,defaultValue);
#define CONST_STRING_VALUE(variable,parameterName,defaultValue,comment) variable= (char*)pParameters->getConstString(parameterName,defaultValue);
#define CONST_DOUBLE_VALUE(variable,parameterName,defaultValue,comment) variable= pParameters->getConstDouble(parameterName,defaultValue);
#define DIR_PATH_VALUE(variable,variableName,defaultValue,comment) variable= (char*)pParameters->getConstString(variableName,defaultValue);
#define FILE_PATH_VALUE(variable,variableName,defaultValue,comment) variable= (char*)pParameters->getConstString(variableName,defaultValue);

#define MULTI_VALUED(indexedVariable,name,comment,className,...) indexedVariable= new className(__VA_ARGS__);
#define MULTI_VALUED_FACTORY(indexedVariable,name,comment,className,...) indexedVariable= className::getInstance(__VA_ARGS__);

#define MULTI_VALUED_STRING(variable,parameterName,defaultValue,comment,parameterNode) variable= parameterNode->getConstString(0,defaultValue);
#define MULTI_VALUED_DOUBLE(variable,parameterName,defaultValue,comment,parameterNode) variable= parameterNode->getConstDouble(0,defaultValue);
#define MULTI_VALUED_INTEGER(variable,parameterName,defaultValue,comment,parameterNode) variable= parameterNode->getConstInteger(0,defaultValue);
#define MULTI_VALUED_FILE_PATH(variable,parameterName,defaultValue,comment,parameterNode) variable= parameterNode->getConstString(0,defaultValue);
#define MULTI_VALUED_STATE_VAR_REF(variable,parameterName,defaultValue,comment,parameterNode) variable= CWorld::getDynamicModel()->getStateDescriptor()->getVarIndex(parameterNode->getConstString(variableName));
#define MULTI_VALUED_ACTION_VAR_REF(variable,parameterName,defaultValue,comment,parameterNode) CWorld::getDynamicModel()->getActionDescriptor()->getVarIndex(parameterNode->getConstString(variableName));

#define STATE_VARIABLE_REF(variable,variableName,comment) variable= CWorld::getDynamicModel()->getStateDescriptor()->getVarIndex(pParameters->getConstString(variableName));
#define ACTION_VARIABLE_REF(variable,variableName,comment) variable= CWorld::getDynamicModel()->getActionDescriptor()->getVarIndex(pParameters->getConstString(variableName));

#define ENUMERATION(typeName,...) const char* typeName[]= {__VA_ARGS__};
#define ENUM_VALUE(variable,typeName,parameterName,defaultValue,comment) variable= pParameters->getConstString(parameterName,defaultValue);



#endif