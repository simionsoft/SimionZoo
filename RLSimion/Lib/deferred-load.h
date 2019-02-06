#pragma once

//this class is used to defer time-consuming initialization code
//just by calling the constructor from the subclasses constructor,
//the object registers itself in the list hold by SimionApp::getSample()->SimGod
//The member funcion deferredLoadStep() is called after construction
//of all the objects
//MOTIVATION: be able to construct quickly the objects needed in an experiment
//and getSample the input/output files without loading matrices from file or doing any heavy-weight lifting

class DeferredLoad
{
public:
	//loadOrder is an integer that is meant to be used to order load calls when there is some dependency
	//
	//For example, Actor::deferredLoad() AFTER allocating the memory for
	//the weights in LinearStateVFA::deferredLoad()
	DeferredLoad(unsigned int loadOrder = 5);
	virtual ~DeferredLoad();
	virtual void deferredLoadStep() = 0;
};