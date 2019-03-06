# Class RewardFunction
> Source: reward.cpp
## Methods
### ``void addRewardComponent (IRewardComponent* rewardComponent)``
* **Summary**:  
  RewardFunction can use more than one scalar reward and they are added using this method. Scalar rewards must derive from IRewardComponent  
* **Parameters**:  
  * _rewardComponent_: The new scalar reward to be added
### ``double getReward (const State* s, const Action* a, const State* s_p)``
* **Summary**:  
  Calculates the total reward based on the different scalar rewards. If we only define one reward function, its value will be returned  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
### ``void initialize ()``
* **Summary**:  
  DynamicModel subclasses should call this initialization method after adding the reward functions  
### ``void override (double reward)``
* **Summary**:  
  If we want to override the final reward in some special states (i.e. a negative reward if FAST simulator crashed) we can call this method from the DynamicModel  
* **Parameters**:  
  * _reward_: The reward we want to give the agent
