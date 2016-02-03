#pragma once

class CParameter;

#define MAX_PARAMETER_NAME_SIZE 512
#define MAX_NUM_PARAMETERS 512

#define MAX_LINE_LENGTH (MAX_PARAMETER_NAME_SIZE + MAX_STRING_SIZE + 3)
#define MAX_NUM_CHILDREN 64



class CParameters
{
	int m_numParameters;
	CParameter *m_pParameters;

	int m_numChildren;
	CParameters** m_pChildren;

	char m_name[MAX_PARAMETER_NAME_SIZE];

	void saveParametersRecursive(void* pFile, char* prefix);

	static CParameters* m_pRootNode;
	CParameters* m_pParent;

	void allocate();
public:

	//GLOBAL METHODS to be used on the parent node
	CParameters();
	CParameters(const char* parameterFile);
	~CParameters();
	void reset();

	char* getName();
	void setName(const char* name);
	
	//This method parses a line from a parameter file and returns it in "parameter"
	//public just to be used from badger when we read the experiment file
	static bool parseLine(char* line, CParameter& parameter);

	//This method reads a parameter file and loads it into a tree structure (if bLoadAsTree) or a table (else)
	//doesn't reset the existing parameter structure
	void loadParameters(const char *parameterFile, bool bLoadAsTree= true); 
	void saveParameters(const char *parameterFile);

	//Find (and create if bCreateIfNotExist is true) the node corresponding to the full path of a parameter
	//If succesful, the stripped parameter's name is returned in outStrippedName
	//This is the only method that takes a full path as a parameter
	CParameters* findParent(const char* fullParameterName, char* outStrippedName, bool bCreateIfNotExists);

	CParameters* getRootNode(){ return m_pRootNode; }

	int getNumParameters(){ return m_numParameters; }

	CParameter* getParameter(int i);
	CParameter* getParameter(const char* parameterName);
	CParameter* setParameter(const CParameter& parameter);
	CParameter* setParameter(const CParameter* parameter);
	CParameter* addParameter(const CParameter& parameter);
	CParameter* addParameter(const CParameter* parameter);
	
	bool exists(const char* parameterName);


	//CHILDREN

	CParameters* getParent(){ return m_pParent; }
	void setParent(CParameters* pParent){ m_pParent = pParent; }
	//Returns the number of children
	int getNumChildren(){ return m_numChildren; };
	int getNumChildrenByTag(const char* tag);
	//Returns a pointer to the i-th child
	CParameters* getChild(int i);
	//Returns a pointer to a child, or NULL if not found. Name doesn't contain a path, but only the node's name
	CParameters* getChild(const char* name);
	//Returns a pointer to a child, or NULL if not found. Name doesn't contain a path, but only the node's name
	CParameters* getChildByTag(const char* tag, int i);
	//Returns a pointer to a child, which is created if doesn't already exist. Name doesn't contain a path, but only the node's name
	CParameters* addChild(const char* name);
};

