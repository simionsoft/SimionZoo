# Class DynamicModel
> Source: world.cpp
## Methods
### ``size_t addStateVariable (const char* name, const char* units, double min, double max, bool bCircular)``
* **Summary**:  
  This method must be called from the constructor of DynamicModel subclasses to register state variables. Calls are parsed by the source code parser and listed beside the parameters of the class in the class definition file (config.xml)  
* **Parameters**:  
  * _name_: Name of the variable (i.e. "speed")
  * _units_: Metrical unit (i.e., "m/s")
  * _min_: Minimum value this variable may get. Below this, values are clamped
  * _max_: Maximum value this variable may get. Above this, values are clamped
  * _bCircular_: This flag indicates whether the variable is circular (as angles)
### ``size_t addActionVariable (const char* name, const char* units, double min, double max, bool bCircular)``
* **Summary**:  
  This method must be called from the constructor of DynamicModel subclasses to register action variables. Calls are parsed by the source code parser and listed beside the parameters of the class in the class definition file (config.xml)  
* **Parameters**:  
  * _name_: Name of the variable (i.e. "speed")
  * _units_: Metrical unit (i.e., "m/s")
  * _min_: Minimum value this variable may get. Below this, values are clamped
  * _max_: Maximum value this variable may get. Above this, values are clamped
  * _bCircular_: This flag indicates whether the variable is circular (as angles)
### ``void addConstant (const char* name, double value)``
* **Summary**:  
  This method can be called from the constructor of DynamicModel subclasses to register constants. These are also parsed  
* **Parameters**:  
  * _name_: Name of the constant
  * _value_: Literal value (i.e. 6.5). The parser will not recognise but literal values
### ``int getNumConstants ()``
* **Summary**:  
  Returns the number of constants defined in the current DynamicModel subclass  
### ``double getConstant (int i)``
* **Summary**:  
  Returns the value of the i-th constant  
* **Parameters**:  
  * _i_: Index of the constant to be retrieved
### ``double getConstant (const char* constantName)``
* **Summary**:  
  An alternative version of getConstant() that uses the name as input  
* **Parameters**:  
  * _constantName_: The name of the constant
### ``double getReward (const State *s, const Action *a, const State *s_p)``
* **Summary**:  
  This method calculates the reward associated with tuple {s,a,s_p}  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
