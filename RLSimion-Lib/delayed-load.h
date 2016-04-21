#pragma once

//this class is used to defer time-consuming initialization code
//just by calling the constructor from the subclasses constructor,
//the object registers itself in the list hold by CApp::SimGod
//The member funcion deferredLoadStep() is called after construction
//of all the objects
//MOTIVATION: be able to load quickly an experiment and be able to get the input/output files without loading matrices from file
class CDeferredLoad
{
public:
	CDeferredLoad();
	virtual ~CDeferredLoad();
	virtual void deferredLoadStep() = 0;
};