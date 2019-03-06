# Class DiscreteSoftmaxDeepPolicy
> Source: deep-vfa-policy.cpp
## Methods
### ``int selectAction (const std::vector<double>& values)``
* **Summary**:  
  Deep-RL version of the Soft-Max action selection policy  
* **Parameters**:  
  * _values_: Estimated Q(s,a) for each discrete action. Size should equal the number of discrete actions
