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
	const char *m_name;
	unsigned int m_numAllocFeatures;

	void resize(unsigned int newSize, bool bKeepFeatures= true);
	int getFeaturePos(unsigned int index);
protected:
	bool m_bAddIfExists;
	bool m_bReplaceIfExists;
public:
	int m_type;
	CFeature* m_pFeatures;
	unsigned int m_numFeatures;

	CFeatureList(bool addIfExists= true, bool replaceIfExists= false);
	virtual ~CFeatureList();

	void setName(const char* name);
	void clear();
	void mult(double factor);
	double getFactor(unsigned int index) const;
	double innerProduct(const CFeatureList *inList);
	void copyMult(double factor,const CFeatureList *inList);
	virtual void addFeatureList(const CFeatureList *inList,double factor= 1.0);
	void add(unsigned int index, double value);

	//spawn: all features (indices and values) are spawned by those in inList
	//[2,3].spawn([1,2,3]) => [2*indexOffset + 1, 2*indexOffset + 2, 2*indexOffset+3
	//                        , 3*indexOffset+1, 3*indexOffset+2, 3*indexOffset+3]
	void spawn(const CFeatureList *inList, unsigned int indexOffset);

	//adds an offset to all feature indices in a feature list.
	void offsetIndices(int offset);

	//multiplies all indices by a factor
	void multIndices(int mult);

	void applyThreshold(double threshold);
	void normalize();
	void copy(const CFeatureList* inList);
};

