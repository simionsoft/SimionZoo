#pragma once


//Feature////////////////////////////////////////////////
//////////////////////////////////////////////////////////
struct Feature
{
	Feature(){}
	Feature(unsigned int index, double factor){m_index= index; m_factor= factor;};

	unsigned int m_index;
	double m_factor;
};

//FeatureList/////////////////////////////////////////////
///////////////////////////////////////////////////////////

enum OverwriteMode{Replace,Add,AllowDuplicates};

class FeatureList
{
	const char *m_name;
	unsigned int m_numAllocFeatures;

	void resize(unsigned int newSize, bool bKeepFeatures= true);
	int getFeaturePos(unsigned int index);
protected:
	OverwriteMode m_overwriteMode;
public:
	Feature* m_pFeatures;
	unsigned int m_numFeatures;

	FeatureList(const char* pName,OverwriteMode overwriteMode=OverwriteMode::Add);
	virtual ~FeatureList();

	void setName(const char* name);
	const char* getName();
	void clear();
	void mult(double factor);
	double getFactor(unsigned int index) const;
	double innerProduct(const FeatureList *inList);
	void copyMult(double factor,const FeatureList *inList);
	void addFeatureList(const FeatureList *inList,double factor= 1.0);
	void add(unsigned int index, double value);

	//spawn: all features (indices and values) are spawned by those in inList
	//[2,3].spawn([1,2,3]) => [2*indexOffset + 1, 2*indexOffset + 2, 2*indexOffset+3
	//                        , 3*indexOffset+1, 3*indexOffset+2, 3*indexOffset+3]
	void spawn(const FeatureList *inList, unsigned int indexOffset);

	//adds an offset to all feature indices in a feature list.
	void offsetIndices(int offset);

	void split(FeatureList *outList1, FeatureList *outList2, unsigned int splitOffset) const;

	//multiplies all indices by a factor
	void multIndices(int mult);

	void applyThreshold(double threshold);
	void normalize();
	void copy(const FeatureList* inList);
};

