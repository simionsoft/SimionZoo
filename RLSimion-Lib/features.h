#pragma once


//CFeature////////////////////////////////////////////////
//////////////////////////////////////////////////////////
struct CFeature
{
	CFeature(){}
	CFeature(unsigned int index, double factor){m_index= index; m_factor= factor;};

	unsigned int m_index;
	double m_factor;
};

//CFeatureList/////////////////////////////////////////////
///////////////////////////////////////////////////////////

#define LIST_UNSORTED 0
#define LIST_SORTED_BY_VALUE 1
#define LIST_SORTED_BY_ABS_VALUE 2
#define LIST_SORTED_BY_INDEX 3

class CFeatureList
{
	unsigned int m_numAllocFeatures;

	void resize(unsigned int newSize, bool bKeepFeatures= true);
	int getFeaturePos(unsigned int index);
public:
	int m_type;
	CFeature* m_pFeatures;
	unsigned int m_numFeatures;

	static void swapFeatureLists(CFeatureList** pList1,CFeatureList** pList2);

	CFeatureList();//(int type= LIST_UNSORTED);
	~CFeatureList();

	void clear();
	void mult(double factor);
	double getFactor(unsigned int index);
	double innerProduct(CFeatureList *inList);
	void copyMult(double factor,CFeatureList *inList);
	void addFeatureList(CFeatureList *inList,double factor=1.0, bool bAddIfExists= false, bool bReplaceIfExists= false);
	void add(unsigned int index, double value, bool bAddIfExists= false, bool bReplaceIfExists= false);

	//spawn: all features (indices and values) are spawned by those in inList
	void spawn(CFeatureList *inList, unsigned int indexOffset);

	void applyThreshold(double threshold);
	void normalize();
	void copy(CFeatureList* inList);
};
