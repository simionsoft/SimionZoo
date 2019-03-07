# Class LinearStateVFA
> Source: vfa.cpp
## Methods
### ``void setInitValue(double initValue)``
* **Summary**
  Sets the initial value of the function
* **Parameters**
* * _initValue_: 
### ``void getFeatures(const State* s, FeatureList* outFeatures)``
* **Summary**
  Uses the state feature map to calculate the features of a state
### ``void getFeatureState(size_t feature, State* s)``
* **Summary**
  Given a feature, it uses the state feature map to return the state variable's value in s
### ``double get(const State *s)``
* **Summary**
  Evaluates V(s)
