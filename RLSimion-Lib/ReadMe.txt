Author: Borja Fernandez-Gauna


-This library is the core of RLSimion and implements all the classes and functionalities.

-There are four global objects, all configured using a single XML configuration file:

	-Logger
	-World: the environment. Dynamic model, set points, reward, ...
	-SimGod: the controller, be it an A/C learner or a regular controller (PID, LQR, Vidal, Boukhezzar, Jonkman, ...)
	-Experiment: the definition of the experiment itself and log information. Number of episodes, evaluation episodes, ....

-It depends on the 3-rd party tinyxml2 library