#pragma once


//Feature////////////////////////////////////////////////
//////////////////////////////////////////////////////////
struct Feature
{
	Feature(){}
	Feature(unsigned int index, double factor){m_index= index; m_factor= factor;};

	size_t m_index;
	double m_factor;
};

//FeatureList/////////////////////////////////////////////
///////////////////////////////////////////////////////////

enum OverwriteMode{Replace,Add,AllowDuplicates};

class FeatureList
{
	const char *m_name;
	size_t m_numAllocFeatures;

	void resize(size_t newSize, bool bKeepFeatures= true);

protected:
	OverwriteMode m_overwriteMode;
public:
	Feature* m_pFeatures;
	size_t m_numFeatures;

	FeatureList(const char* pName,OverwriteMode overwriteMode=OverwriteMode::AllowDuplicates);
	virtual ~FeatureList();

	long long getFeaturePos(size_t index);

	void setName(const char* name);
	const char* getName();
	void clear();
	void mult(double factor);
	double getFactor(size_t index) const;
	double innerProduct(const FeatureList *inList);
	void copyMult(double factor,const FeatureList *inList);
	void addFeatureList(const FeatureList *inList,double factor= 1.0);
	void add(size_t index, double value);

	//Returns the index of the feature with the highest activation factor
	//If there are no features, it returns -1
	long long maxFactorFeature();

	//spawn: all features (indices and values) are spawned by those in inList
	//[2,3].spawn([1,2,3]) => [2*indexOffset + 1, 2*indexOffset + 2, 2*indexOffset+3
	//                        , 3*indexOffset+1, 3*indexOffset+2, 3*indexOffset+3]
	void spawn(const FeatureList *inList, size_t indexOffset);

	//adds an offset to all feature indices in a feature list.
	void offsetIndices(size_t offset);

	void split(FeatureList *outList1, FeatureList *outList2, size_t splitOffset) const;

	//multiplies all indices by a factor
	void multIndices(int mult);

	void applyThreshold(double threshold);
	void normalize();
	void copy(const FeatureList* inList);
};

