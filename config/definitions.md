# GUI-editable parameters
## Regular classes
### CACLALearner
* *Alpha* (_NumericValue subclass_) : Learning gain [0..1]
* *Policy* (_Policy subclass_) : The policy to be learned
### IncrementalNaturalActorCritic
* *VFunction* (_LinearStateVFA_) : The Value-function
* *V-ETraces* (_ETraces_) : Traces used by the critic
* *Alpha-v* (_NumericValue subclass_) : Learning gain used by the critic
* *Alpha-r* (_NumericValue subclass_) : Learning gain used to average the reward
* *Alpha-u* (_NumericValue subclass_) : Learning gain used by the actor
* *Policy* (_Multiple Policy subclass_) : The policy
### OffPolicyActorCritic
* *VFunction* (_LinearStateVFA_) : The Value-function
* *ETraces* (_ETraces_) : Traces used by the critic and the actor
* *Alpha-v* (_NumericValue subclass_) : Learning gain used by the critic
* *Alpha-w* (_NumericValue subclass_) : Learning gain used to average the reward
* *Alpha-u* (_NumericValue subclass_) : Learning gain used by the actor
* *Policy* (_Multiple Policy subclass_) : The policy
### OffPolicyDeterministicActorCritic
* *QFunction* (_LinearStateActionVFA_) : The Q-function
* *Alpha-w* (_NumericValue subclass_) : Learning gain used by the critic
* *Alpha-theta* (_NumericValue subclass_) : Learning gain used by the actor
* *Policy* (_Multiple DeterministicPolicy subclass_) : The deterministic policy
### ActorCritic
* *Actor* (_Actor_) : The actor
* *Critic* (_ICritic subclass_) : The critic
### RegularPolicyGradientLearner
* *Alpha* (_NumericValue subclass_) : The learning gain
* *Policy* (_Policy subclass_) : The policy to be learned
### Actor
* *Base-Controller* (_Controller subclass_) : The base controller used to initialize the weights of the actor
* *Output* (_Multiple PolicyLearner subclass_) : The outputs of the actor. One for each output dimension
### SimionApp
* *Log* (_Logger_) : The logger class
* *World* (_World_) : The simulation environment and its parameters
* *Experiment* (_Experiment_) : The parameters of the experiment
* *SimGod* (_SimGod_) : The omniscient class that controls all aspects of the simulation process
### AsyncQLearning
* *i-async-update* (_int_) : Steps before the network's weights are updated by the optimizer with the accumulated DeltaTheta
* *i-target* (_int_) : Steps before the target network's weights are updated
* *neural-network* (_NN Definition_) : Neural Network Architecture
* *experience-replay* (_ExperienceReplay_) : Experience replay
* *Policy* (_DiscreteDeepPolicy subclass_) : The policy
* *Output-Action* (_Action_) : The output action variable
### LQRGain
* *Gain* (_double_) : The gain applied to the input state variable
* *Variable* (_State_) : The input state variable
### LQRController
* *Output-Action* (_Action_) : The output action
* *LQR-Gain* (_Multiple LQRGain_) : An LQR gain on an input state variable
### PIDController
* *KP* (_NumericValue subclass_) : Proportional gain
* *KI* (_NumericValue subclass_) : Integral gain
* *KD* (_NumericValue subclass_) : Derivative gain
* *Input-Variable* (_State_) : The input state variable
* *Output-Action* (_Action_) : The output action
### WindTurbineVidalController
* *A* (_NumericValue subclass_) : A parameter of the torque controller
* *K_alpha* (_NumericValue subclass_) : K_alpha parameter of the torque controller
* *KP* (_NumericValue subclass_) : Proportional gain of the pitch controller
* *KI* (_NumericValue subclass_) : Integral gain of the pitch controller
### WindTurbineBoukhezzarController
* *C_0* (_NumericValue subclass_) : C_0 parameter
* *KP* (_NumericValue subclass_) : Proportional gain of the pitch controller
* *KI* (_NumericValue subclass_) : Integral gain of the pitch controller
### WindTurbineJonkmanController
* *CornerFreq* (_double_) : Corner Freq. parameter
* *VS_SlPc* (_double_) : SIPc parameter
* *VS_Rgn2K* (_double_) : Rgn2K parameter
* *VS_Rgn2Sp* (_double_) : Rgn2Sp parameter
* *VS_CtInSp* (_double_) : CtlnSp parameter
* *VS_Rgn3MP* (_double_) : Rgn3MP parameter
* *PC_RefSpd* (_double_) : Pitch control reference speed
* *PC_KK* (_NumericValue subclass_) : Pitch angle were the the derivative of the...
* *PC_KP* (_NumericValue subclass_) : Proportional gain of the pitch controller
* *PC_KI* (_NumericValue subclass_) : Integral gain of the pitch controller
### Controller-Factory
* *Controller* (_Controller subclass_) : The specific controller to be used
### TDLambdaCritic
* *E-Traces* (_ETraces_) : Eligibility traces of the critic
* *Alpha* (_NumericValue subclass_) : Learning gain
* *V-Function* (_LinearStateVFA_) : The V-function to be learned
### TDCLambdaCritic
* *E-Traces* (_ETraces_) : Elilgibility traces of the critic
* *Alpha* (_NumericValue subclass_) : Learning gain of the critic
* *Beta* (_NumericValue subclass_) : Learning gain applied to the omega vector
* *V-Function* (_LinearStateVFA_) : The V-function to be learned
### TrueOnlineTDLambdaCritic
* *E-Traces* (_ETraces_) : Eligibility traces of the critic
* *Alpha* (_NumericValue subclass_) : Learning gain of the critic
* *V-Function* (_LinearStateVFA_) : The V-function to be learned
### VLearnerCritic
* *V-Function* (_LinearStateVFA_) : The V-function to be learned
### ICritic-Factory
* *Critic* (_ICritic subclass_) : Critic type
### DDPG
* *Tau* (_double_) : The rate by which the target weights approach the online weights
* *Learning-Rate* (_double_) : The learning rate at which the agent learns
* *Critic-Network* (_NN Definition_) : Neural Network for the Critic -a Q function-
* *Actor-Network* (_NN Definition_) : Neural Network for the Actor -deterministic policy-
* *Exploration-Noise* (_Noise subclass_) : Noise added to the output of the policy
* *Input-State* (_Multiple State_) : Set of state variables used as input
* *Output-Action* (_Multiple Action_) : The output action variable
### DiscreteEpsilonGreedyDeepPolicy
* *epsilon* (_NumericValue subclass_) : Epsilon
### DiscreteSoftmaxDeepPolicy
* *temperature* (_NumericValue subclass_) : Tempreature
### DiscreteDeepPolicy-Factory
* *Policy* (_DiscreteDeepPolicy subclass_) : The policy type
### DQN
* *Num-Action-Steps* (_int_) : Number of discrete values used for the output action
* *Learning-Rate* (_double_) : The learning rate at which the agent learns
* *neural-network* (_NN Definition_) : Neural Network Architecture
* *Policy* (_DiscreteDeepPolicy subclass_) : The policy
* *Output-Action* (_Action_) : The output action variable
* *Input-State* (_Multiple State_) : Set of variables used as input of the QNetwork
### DoubleDQN
* *Num-Action-Steps* (_int_) : Number of discrete values used for the output action
* *Learning-Rate* (_double_) : The learning rate at which the agent learns
* *neural-network* (_NN Definition_) : Neural Network Architecture
* *Policy* (_DiscreteDeepPolicy subclass_) : The policy
* *Output-Action* (_Action_) : The output action variable
* *Input-State* (_Multiple State_) : Set of variables used as input of the QNetwork
### ETraces
* *Threshold* (_double_) : Threshold applied to trace factors
* *Lambda* (_double_) : Lambda parameter
* *Replace* (_bool_) : Replace existing traces? Or add?
### ExperienceReplay
* *Buffer-Size* (_int_) : Size of the buffer used to store experience tuples
* *Update-Batch-Size* (_int_) : Number of tuples used each time-step in the update
### Experiment
* *Random-Seed* (_int_) : Random seed used to generate random sequences of numbers
* *Num-Episodes* (_int_) : Number of episodes. Zero if we only want to run one evaluation episode
* *Eval-Freq* (_int_) : Evaluation frequency (in episodes). If zero then only training episodes will be run
* *Progress-Update-Freq* (_double_) : Progress update frequency (seconds)
* *Episode-Length* (_double_) : Length of an episode(seconds)
### TileCodingFeatureMap
* *Num-Tiles* (_int_) : Number of tile layers of the grid
* *Tile-Offset* (_double_) : Offset of each tile relative to the previous one. It is scaled by the value range of the input variable
### FeatureMap
* *Num-Features-Per-Dimension* (_int_) : Number of features per input variable
### StateFeatureMap
* *Feature-Mapper* (_FeatureMapper subclass_) : The feature calculator used to map/unmap features
* *Input-State* (_Multiple State_) : State variables used as input of the feature map
* *Num-Features-Per-Dimension* (_int_) : Number of features per input variable
### ActionFeatureMap
* *Feature-Mapper* (_FeatureMapper subclass_) : The feature calculator used to map/unmap features
* *Input-Action* (_Multiple Action_) : Action variables used as input of the feature map
* *Num-Features-Per-Dimension* (_int_) : Number of features per input variable
### FeatureMapper-Factory
* *Type* (_FeatureMapper subclass_) : Feature map type
### Logger
* *Num-Functions-Logged* (_int_) : How many times per experiment save logged functions
* *Log-Freq* (_double_) : Log frequency. Simulation time in seconds.
* *Log-Eval-Episodes* (_bool_) : Log evaluation episodes?
* *Log-Training-Episodes* (_bool_) : Log training episodes?
* *Log-Functions* (_bool_) : Log functions learned?
### GaussianNoise
* *Sigma* (_double_) : Width of the gaussian bell
* *Alpha* (_double_) : Low-pass first-order filter's gain [0...1]. 1=no filter
* *Scale* (_NumericValue subclass_) : Scale factor applied to the noise signal before adding it to the policy's output
### SinusoidalNoise
* *Time-Frequency* (_double_) : Frequency of the signal in 1/simulation seconds
* *Amplitude-Scale* (_NumericValue subclass_) : Scaling factor applied to the sinusoidal
### OrnsteinUhlenbeckNoise
* *Mu* (_double_) : Mean value of the generated noise
* *Sigma* (_double_) : Degree of volatility around it caused by shocks
* *Theta* (_double_) : Rate by which noise shocks dissipate and the variable reverts towards the mean
* *Scale* (_NumericValue subclass_) : Scale factor applied to the noise signal before adding it to the policy's output
### Noise-Factory
* *Noise* (_Noise subclass_) : Noise type
### ConstantValue
* *Value* (_double_) : Constant value
### SimpleEpisodeLinearSchedule
* *Initial-Value* (_double_) : Value at the beginning of the experiment
* *End-Value* (_double_) : Value at the end of the experiment
### InterpolatedValue
* *Start-Offset* (_double_) : Normalized time from which the schedule will begin [0...1]
* *End-Offset* (_double_) : Normalized time at which the schedule will end and only return the End-Value [0...1]
* *Pre-Offset-Value* (_double_) : Output value before the schedule begins
* *Initial-Value* (_double_) : Output value at the beginning of the schedule
* *End-Value* (_double_) : Output value at the end of the schedule
* *Evaluation-Value* (_double_) : Output value during evaluation episodes
* *Interpolation* (_Interpolation_) : Interpolation type
* *Time-reference* (_TimeReference_) : The time-reference type
### BhatnagarSchedule
* *Alpha-0* (_double_) : Alpha-0 parameter in Bhatnagar's schedule
* *Alpha-c* (_double_) : Alpha-c parameter in Bhatnagar's schedule
* *Time-Exponent* (_double_) : Time exponent in Bhatnagar's schedule
* *Evaluation-Value* (_double_) : Output value during evaluation episodes
* *Time-reference* (_TimeReference_) : The time reference
### WireConnection
* *Wire* (_Wire_) : Wire connection from which the value comes
### NumericValue-Factory
* *Schedule* (_NumericValue subclass_) : Schedule-type
### PolicyLearner
* *Policy* (_Policy subclass_) : The policy to be learned
### PolicyLearner-Factory
* *Policy-Learner* (_PolicyLearner subclass_) : The algorithm used to learn the policy
### QEGreedyPolicy
* *Epsilon* (_NumericValue subclass_) : The epsilon parameter that balances exploitation and exploration
### QSoftMaxPolicy
* *Tau* (_NumericValue subclass_) : Temperature parameter
### GreedyQPlusNoisePolicy
* *Noise* (_Multiple Noise subclass_) : Noise signal added to the typical greedy action selection. The number of noise signals should match the number of actions in the action feature amp
### QLearningCritic
* *Q-Function* (_LinearStateActionVFA_) : The parameterization of the Q-Function
* *E-Traces* (_ETraces_) : E-Traces
* *Alpha* (_NumericValue subclass_) : The learning gain [0-1]
### QLearning
* *Policy* (_QPolicy subclass_) : The policy to be followed
* *Q-Function* (_LinearStateActionVFA_) : The parameterization of the Q-Function
* *E-Traces* (_ETraces_) : E-Traces
* *Alpha* (_NumericValue subclass_) : The learning gain [0-1]
### DoubleQLearning
* *Policy* (_QPolicy subclass_) : The policy to be followed
* *Q-Function* (_LinearStateActionVFA_) : The parameterization of the Q-Function
* *E-Traces* (_ETraces_) : E-Traces
* *Alpha* (_NumericValue subclass_) : The learning gain [0-1]
### SARSA
* *Policy* (_QPolicy subclass_) : The policy to be followed
* *Q-Function* (_LinearStateActionVFA_) : The parameterization of the Q-Function
* *E-Traces* (_ETraces_) : E-Traces
* *Alpha* (_NumericValue subclass_) : The learning gain [0-1]
### QPolicy-Factory
* *Policy* (_QPolicy subclass_) : The exploration policy used to learn
### SimGod
* *Target-Function-Update-Freq* (_int_) : Update frequency at which target functions will be updated. Only used if Freeze-Target-Function=true
* *Gamma* (_double_) : Gamma parameter
* *Freeze-Target-Function* (_bool_) : Defers updates on the V-functions to improve stability
* *Use-Importance-Weights* (_bool_) : Use sample importance weights to allow off-policy learning -experimental-
* *State-Feature-Map* (_StateFeatureMap_) : The state feature map
* *Action-Feature-Map* (_ActionFeatureMap_) : The state feature map
* *Experience-Replay* (_ExperienceReplay_) : The experience replay parameters
* *Simion* (_Multiple Simion subclass_) : Simions: learning agents and controllers
### Simion-Factory
* *Type* (_Simion subclass_) : The Simion class
### Policy
* *Output-Action* (_Action_) : The output action variable
### DeterministicPolicy
* *Output-Action* (_Action_) : The output action variable
### StochasticPolicy
* *Output-Action* (_Action_) : The output action variable
### DeterministicPolicyGaussianNoise
* *Deterministic-Policy-VFA* (_LinearStateVFA_) : The parameterized VFA that approximates the function
* *Exploration-Noise* (_Noise subclass_) : Parameters of the noise used as exploration
* *Output-Action* (_Action_) : The output action variable
### StochasticGaussianPolicy
* *Mean-VFA* (_LinearStateVFA_) : The parameterized VFA that approximates the function
* *Sigma-VFA* (_LinearStateVFA_) : The parameterized VFA that approximates variance(s)
* *Output-Action* (_Action_) : The output action variable
### Policy-Factory
* *Policy* (_Policy subclass_) : The policy type
### LinearStateVFA
* *Init-Value* (_double_) : The initial value given to the weights on initialization
### LinearStateActionVFA
* *Init-Value* (_double_) : The initial value given to the weights on initialization
### World
* *Num-Integration-Steps* (_int_) : The number of integration steps performed each simulation time-step
* *Delta-T* (_double_) : The delta-time between simulation steps
* *Dynamic-Model* (_DynamicModel subclass_) : The dynamic model
### DynamicModel-Factory
* *Model* (_DynamicModel subclass_) : The world
## Worlds
### BalancingPole
#### State variables
* x
* x_dot
* theta
* theta_dot
#### Action variables
* force
### DoublePendulum
#### State variables
* theta_1
* theta_1-dot
* theta_2
* theta_2-dot
#### Action variables
* torque_1
* torque_2
### FASTWindTurbine
#### State variables
* T_a
* P_a
* P_s
* P_e
* E_p
* v
* omega_r
* d_omega_r
* E_omega_r
* omega_g
* d_omega_g
* E_omega_g
* beta
* d_beta
* T_g
* d_T_g
* E_int_omega_r
* E_int_omega_g
* theta
#### Action variables
* beta
* T_g
### MountainCar
#### State variables
* position
* velocity
* height
* angle
#### Action variables
* pedal
### PitchControl
#### State variables
* setpoint-pitch
* attack-angle
* pitch
* pitch-rate
* control-deviation
#### Action variables
* pitch
### PullBox1
#### State variables
* target-x
* target-y
* robot1-x
* robot1-y
* box-x
* box-y
* robot1-theta
* box-theta
* box-to-target-x
* box-to-target-y
* robot1-to-box-x
* robot1-to-box-y
#### Action variables
* robot1-v
* robot1-omega
### PullBox2
#### State variables
* target-x
* target-y
* robot1-x
* robot1-y
* robot2-x
* robot2-y
* box-x
* box-y
* robot1-theta
* robot2-theta
* box-theta
* box-to-target-x
* box-to-target-y
* robot1-to-box-x
* robot1-to-box-y
* robot2-to-box-x
* robot2-to-box-y
#### Action variables
* robot1-v
* robot1-omega
* robot2-v
* robot2-omega
### PushBox1
#### State variables
* target-x
* target-y
* robot1-x
* robot1-y
* box-x
* box-y
* robot1-to-box-x
* robot1-to-box-y
* box-to-target-x
* box-to-target-y
* robot1-theta
* box-theta
#### Action variables
* robot1-v
* robot1-omega
### PushBox2
#### State variables
* target-x
* target-y
* robot1-x
* robot1-y
* robot2-x
* robot2-y
* box-x
* box-y
* robot1-to-box-x
* robot1-to-box-y
* robot2-to-box-x
* robot2-to-box-y
* box-to-target-x
* box-to-target-y
* robot1-theta
* robot2-theta
* box-theta
#### Action variables
* robot1-v
* robot1-omega
* robot2-v
* robot2-omega
### RainCar
#### State variables
* position
* velocity
* position-deviation
#### Action variables
* acceleration
### RobotControl
#### State variables
* target-x
* target-y
* robot1-x
* robot1-y
* robot1-theta
#### Action variables
* robot1-v
* robot1-omega
### SwingupPendulum
#### State variables
* angle
* angular-velocity
#### Action variables
* torque
### UnderwaterVehicle
#### State variables
* v-setpoint
* v
* v-deviation
#### Action variables
* u-thrust
### WindTurbine
#### State variables
* T_a
* P_a
* P_s
* P_e
* E_p
* v
* omega_r
* d_omega_r
* E_omega_r
* omega_g
* d_omega_g
* E_omega_g
* beta
* d_beta
* T_g
* d_T_g
* E_int_omega_r
* E_int_omega_g
* theta
#### Action variables
* beta
* T_g
