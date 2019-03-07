# Class FeatureList
> Source: features.cpp
## Methods
### ``void resize(size_t newSize, bool bKeepFeatures)``
* **Summary**
  Resizes the feature list, allocating more memory
* **Parameters**
  * _newSize_: New size
  * _bKeepFeatures_: true if we want to preserve the features on the list
### ``void mult(double factor)``
* **Summary**
  Multiplies all the features by a factor
* **Parameters**
  * _factor_: The factor value
### ``double getFactor(size_t index)``
* **Summary**
  Returns the factor of a given feature on the list
* **Parameters**
  * _index_: The index of the feature
* **Return Value**
  Returns the factor of a given feature on the list
### ``double innerProduct(const FeatureList *inList)``
* **Summary**
  Implements an inner-product operation
* **Parameters**
  * _inList_: Second operand of the multiply operation
* **Return Value**
  Implements an inner-product operation
### ``void copyMult(double factor, const FeatureList *inList)``
* **Summary**
  Copies a list multiplied by a factor on this feature list. Resizes the list if needed
* **Parameters**
  * _factor_: The factor to multiply by
  * _inList_: The feature list to copy
### ``void addFeatureList(const FeatureList *inList, double factor)``
* **Summary**
  Adds feature to this list, multiplied by a factor
* **Parameters**
  * _inList_: Feature list to be added
  * _factor_: Factor used to multiply
### ``void add(size_t index, double value)``
* **Summary**
  Adds a single feature
* **Parameters**
  * _index_: The index of the feature
  * _value_: The value of the feature
### ``void spawn(const FeatureList *inList, size_t indexOffset)``
* **Summary**
  All features (indices and values) are spawned by those in inList. This means that this list contains 2 features a 5-feature space, and inList contains 3 features a 6-feature space, after this operation, this list will contain 2*3= 6 features from a 5*6= 30 feature space
* **Parameters**
  * _inList_: Second list used as an operand
  * _indexOffset_: Feature-index offset used for the second list
### ``void applyThreshold(double threshold)``
* **Summary**
  Removes any feature with an activation factor under the threshold
* **Parameters**
  * _threshold_: Threshold value
### ``void normalize()``
* **Summary**
  Normalizes features so that the sum of all the activation factors are 1
### ``void copy(const FeatureList* inList)``
* **Summary**
  Copies in this list the given one
* **Parameters**
  * _inList_: Source feature list to copy
### ``void offsetIndices(size_t offset)``
* **Summary**
  Adds an offset to all the feature indices
* **Parameters**
  * _offset_: Offset value
### ``void split(FeatureList *outList1, FeatureList *outList2, size_t splitOffset)``
* **Summary**
  Splits this feature list in two lists: features with an index below splitOffset go to the first output list, and those above go to the second output list
* **Parameters**
  * _outList1_: Output list 1
  * _outList2_: Output list 2
  * _splitOffset_: Index used to split the feature list
### ``void multIndices(int mult)``
* **Summary**
  Multiplies all the feature indices by mult
* **Parameters**
  * _mult_: Value used to multiply
