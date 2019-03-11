# Class LinearStateActionVFA
> Source: vfa.cpp
## Methods
### `void getFeatures(const State* s, const Action* a, FeatureList* outFeatures)`
* *Summary*
  Given a state-action pair, it calculates the features for each feature map separately (state and action) and then combines using spawn() and offsetIndices() so that the resultant features belong to the full state-action feature space
* *Parameters*
  * _s_: State
  * _a_: Action
  * _outFeatures_: Output feature list
### `void getFeatureStateAction(size_t feature, State* s, Action* a)`
* *Summary*
  Given a feature index, it returns in s and a the values of the variables to which the feature corresponds
* *Parameters*
  * _feature_: Index of the feature
  * _s_: Output state
  * _a_: Output action
### `double get(const State *s, const Action* a)`
* *Summary*
  Evaluates Q(s,a)
### `void argMax(const State *s, Action* a, bool bSolveTiesRandomly)`
* *Summary*
  Calculates the action a that maximizes Q(s,a)
* *Parameters*
  * _s_: State
  * _a_: Output action that maximizes Q(s,a)
  * _bSolveTiesRandomly_: In case of tie, this flag sets whether return a random action or the first one
### `double max(const State* s, bool bUseFrozenWeights)`
* *Summary*
  Calculates the maximum value of Q(s,a) for state s
* *Parameters*
  * _s_: State
  * _bUseFrozenWeights_: If set and it makes sense, will use the target function
* *Return Value*
  Calculates the maximum value of Q(s,a) for state s
### `void getActionValues(const State* s,double *outActionValues)`
* *Summary*
  Returns an array with the values for each action feature
* *Parameters*
  * _s_: State
  * _outActionValues_: Output action values, one for every feature in the action feature map
