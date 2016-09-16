// sutton-mountaincar-sarsa.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*
This is an example program for reinforcement learning with linear
function approximation.  The code follows the psuedo-code for linear,
gradient-descent Sarsa(lambda) given in Figure 8.8 of the book
"Reinforcement Learning: An Introduction", by Sutton and Barto.
One difference is that we use the implementation trick mentioned on
page 189 to only keep track of the traces that are larger
than "min-trace".

Before running the program you need to obtain the tile-coding
software, available at http://envy.cs.umass.edu/~rich/tiles.C and tiles.h
(see http://envy.cs.umass.edu/~rich/tiles.html for documentation).

The code below is in three main parts: 1) Mountain Car code, 2) General
RL code, and 3) top-level code and misc.

Written by Rich Sutton 12/19/00
*/

#include <iostream>
#include "tiles.h"
#include "stdio.h"
#include "stdlib.h"
#include <fcntl.h>
#include <string.h>


//////////     Part 1: Mountain Car code     //////////////

// Global variables:
float mcar_position, mcar_velocity;

#define mcar_min_position -1.2
#define mcar_max_position 0.6
#define mcar_max_velocity 0.07            // the negative of this in the minimum velocity
#define mcar_goal_position 0.5

#define POS_WIDTH (1.7 / 8)               // the tile width for position
#define VEL_WIDTH (0.14 / 8)              // the tile width for velocity

// Profiles
void MCarInit();                              // initialize car state
void MCarStep(int a);                         // update car state for given action
bool MCarAtGoal();                           // is car at goal?

void MCarInit()
// Initialize state of Car
{
	mcar_position = -0.5;
	mcar_velocity = 0.0;
}

void MCarStep(int a)
// Take action a, update state of car
{
	mcar_velocity += (a - 1)*0.001 + cos(3 * mcar_position)*(-0.0025);
	if (mcar_velocity > mcar_max_velocity) mcar_velocity = mcar_max_velocity;
	if (mcar_velocity < -mcar_max_velocity) mcar_velocity = -mcar_max_velocity;
	mcar_position += mcar_velocity;
	if (mcar_position > mcar_max_position) mcar_position = mcar_max_position;
	if (mcar_position < mcar_min_position) mcar_position = mcar_min_position;
	if (mcar_position == mcar_min_position && mcar_velocity<0) mcar_velocity = 0;
}

bool MCarAtGoal()
// Is Car within goal region?
{
	return mcar_position >= mcar_goal_position;
}


//////////     Part 2: Semi-General RL code     //////////////

#define MEMORY_SIZE 10000                        // number of parameters to theta, memory size
#define NUM_ACTIONS 3                            // number of actions
#define NUM_TILINGS 10

// Global RL variables:
float Q[NUM_ACTIONS];                            // action values
float theta[MEMORY_SIZE];                        // modifyable parameter vector, aka memory, weights
float e[MEMORY_SIZE];                            // eligibility traces
int F[NUM_ACTIONS][NUM_TILINGS];                 // sets of features, one set per action

// Standard RL parameters:
#define epsilon 0.0                    // probability of random action
#define alpha 0.5                      // step size parameter
#define lambda 0.9                     // trace-decay parameters
#define gamma 1.0                      // discount-rate parameters

// Global stuff for efficient eligibility trace implementation:
#define MAX_NONZERO_TRACES 1000
int nonzero_traces[MAX_NONZERO_TRACES];
int num_nonzero_traces = 0;
int nonzero_traces_inverse[MEMORY_SIZE];
float minimum_trace = 0.01;

// Profiles:
int episode(int max_steps);                      // do one episode, return length
void LoadQ();                                    // compute action values for current theta, F
void LoadQ(int a);                               // compute one action value for current theta, F
void LoadF();                                    // compute feature sets for current state
int argmax(float Q[NUM_ACTIONS]);              // compute argmax action from Q
bool WithProbability(float p);                   // helper - true with given probability
void ClearTrace(int f);                          // clear or zero-out trace, if any, for given feature
void ClearExistentTrace(int f, int loc);         // clear trace at given location in list of nonzero-traces
void DecayTraces(float decay_rate);              // decay all nonzero traces
void SetTrace(int f, float new_trace_value);     // set trace to given value
void IncreaseMinTrace();                         // increase minimal trace value, forcing more to 0, making room for new ones
void WriteTheta(char *filename);                 // write weights (theta) to a file
void ReadTheta(char *filename);                  // reads them back in

int episode(int max_steps)
// Runs one episode of at most max_steps, returning episode length; see Figure 8.8 of RLAI book
{
	MCarInit();                                                  // initialize car's state
	DecayTraces(0.0);                                            // clear all traces
	LoadF();                                                     // compute features
	LoadQ();                                                     // compute action values
	int action = argmax(Q);                                      // pick argmax action
	if (WithProbability(epsilon)) action = rand() % NUM_ACTIONS; // ...or maybe pick action at random
	int step = 0;
	do {
		step++;                                                  // now do a bunch of steps
		DecayTraces(gamma*lambda);                               // let traces fall
		for (int a = 0; a<NUM_ACTIONS; a++)                        // optionally clear other traces
			if (a != action)
				for (int j = 0; j<NUM_TILINGS; j++) ClearTrace(F[a][j]);
		for (int j = 0; j<NUM_TILINGS; j++) SetTrace(F[action][j], 1.0); // replace traces
		MCarStep(action);                                        // actually take action
		float reward = -1;
		float delta = reward - Q[action];
		LoadF();                                                 // compute features new state
		LoadQ();                                                 // compute new state values
		action = argmax(Q);
		if (WithProbability(epsilon)) action = rand() % NUM_ACTIONS;
		if (!MCarAtGoal())
			delta += gamma * Q[action];
		else
			printf("goal");
		float temp = (alpha / NUM_TILINGS)*delta;
		for (int i = 0; i<num_nonzero_traces; i++)                 // update theta (learn)
		{
			int f = nonzero_traces[i];
			theta[f] += temp * e[f];
		}
		LoadQ(action);
	} while (!MCarAtGoal() && step<max_steps);                     // repeat until goal or time limit
	return step;
}                                                // return episode length

void LoadQ()
// Compute all the action values from current F and theta
{
	for (int a = 0; a<NUM_ACTIONS; a++)
	{
		Q[a] = 0;
		for (int j = 0; j<NUM_TILINGS; j++) Q[a] += theta[F[a][j]];
	}
}

void LoadQ(int a)
// Compute an action value from current F and theta
{
	Q[a] = 0;
	for (int j = 0; j<NUM_TILINGS; j++) Q[a] += theta[F[a][j]];
}

void LoadF()
// Compute feature sets for current car state
{
	float state_vars[2];
	state_vars[0] = mcar_position / POS_WIDTH;
	state_vars[1] = mcar_velocity / VEL_WIDTH;
	for (int a = 0; a<NUM_ACTIONS; a++)
		GetTiles(&F[a][0], NUM_TILINGS, state_vars, 2, MEMORY_SIZE, a);
}

int argmax(float Q[NUM_ACTIONS])
// Returns index (action) of largest entry in Q array, breaking ties randomly
{
	int best_action = 0;
	float best_value = Q[0];
	int num_ties = 1;                    // actually the number of ties plus 1
	for (int a = 1; a<NUM_ACTIONS; a++)
	{
		float value = Q[a];
		if (value >= best_value)
			if (value > best_value)
			{
				best_value = value;
				best_action = a;
			}
			else
			{
				num_ties++;
				if (0 == rand() % num_ties)
				{
					best_value = value;
					best_action = a;
				}
			}
	};
	return best_action;
}

bool WithProbability(float p)
// Returns TRUE with probability p
{
	return p > ((float)rand()) / RAND_MAX;
}

// Traces code:

/* Below is the code for selectively working only with traces >= minimum_trace.
Other traces are forced to zero.  We keep a list of which traces are nonzero
so that we can work only with them.  This list is implemented as the array
"nonzero_traces" together with its length "num_nonzero_traces".  When a trace
falls below minimum_trace and is forced to zero, we remove it from the list by
decrementing num_nonzero_traces and moving the last element into the "hole"
in nonzero_traces made by this one that we are removing.  A final complication
arises because sometimes we want to clear (set to zero and remove) a trace
but we don't know its position within the list of nonzero_traces.  To avoid
havint to search through the list we keep inverse pointers from each trace
back to its position (if nonzero) in the nonzero_traces list.  These inverse
pointers are in the array "nonzero_traces_inverse".

Global stuff (really at top of file) for efficient eligibility trace implementation:
#define MAX_NONZERO_TRACES 1000
int nonzero_traces[MAX_NONZERO_TRACES];
int num_nonzero_traces = 0;
int nonzero_traces_inverse[MEMORY_SIZE];
float minimum_trace = 0.01;  */

void ClearTrace(int f)
// Clear any trace for feature f
{
	if (!(e[f] == 0.0))
		ClearExistentTrace(f, nonzero_traces_inverse[f]);
}

void ClearExistentTrace(int f, int loc)
// Clear the trace for feature f at location loc in the list of nonzero traces
{
	e[f] = 0.0;
	num_nonzero_traces--;
	nonzero_traces[loc] = nonzero_traces[num_nonzero_traces];
	nonzero_traces_inverse[nonzero_traces[loc]] = loc;
}

void DecayTraces(float decay_rate)
// Decays all the (nonzero) traces by decay_rate, removing those below minimum_trace
{
	for (int loc = num_nonzero_traces - 1; loc >= 0; loc--)      // necessary to loop downwards
	{
		int f = nonzero_traces[loc];
		e[f] *= decay_rate;
		if (e[f] < minimum_trace) ClearExistentTrace(f, loc);
	}
}

void SetTrace(int f, float new_trace_value)
// Set the trace for feature f to the given value, which must be positive
{
	if (e[f] >= minimum_trace) e[f] = new_trace_value;         // trace already exists
	else {
		while (num_nonzero_traces >= MAX_NONZERO_TRACES) IncreaseMinTrace(); // ensure room for new trace
		e[f] = new_trace_value;
		nonzero_traces[num_nonzero_traces] = f;
		nonzero_traces_inverse[f] = num_nonzero_traces;
		num_nonzero_traces++;
	}
}

void IncreaseMinTrace()
// Try to make room for more traces by incrementing minimum_trace by 10%, 
// culling any traces that fall below the new minimum
{
	minimum_trace += 0.1 * minimum_trace;
	printf( "Changing minimum_trace to %d \n" , minimum_trace);
	for (int loc = num_nonzero_traces - 1; loc >= 0; loc--)      // necessary to loop downwards
	{
		int f = nonzero_traces[loc];
		if (e[f] < minimum_trace) ClearExistentTrace(f, loc);
	};
}

void WriteTheta(char *filename)
// writes parameter vector theta to a file as binary data; writes over any old file
{
	FILE* file = fopen(filename, "bw");
	fwrite((char *)theta, sizeof(float), MEMORY_SIZE, file);
	fclose(file);
}

void ReadTheta(char *filename)
// reads parameter vector theta from a file as binary data
{
	FILE* file = fopen(filename, "br");
	fread((char *)theta, sizeof(float), MEMORY_SIZE, file);
	fclose(file);
}


//////////     Part 3: Top-Level code     //////////////
// (outer loops, data collection, print statements, frequently changed stuff)

// Profiles
void open_record_file(char *filename);
void start_record();
void end_record();

#define NUM_RUNS 10
#define NUM_EPISODES 1000

	FILE *record_file;

int main()
// The main program just does a bunch of runs, each consisting of some episodes.
{

	open_record_file("test");
	for (int i = 0; i<MEMORY_SIZE; i++) e[i] = 0.0;         // traces must start at 0
	start_record();
	for (int run = 0; run<NUM_RUNS; run++)
	{
		printf("Starting run #%d\n",run);
		for (int i = 0; i<MEMORY_SIZE; i++) theta[i] = 0.0; // clear memory at start of each run
		fprintf(record_file, "\n  (");
		for (int episode_num = 0; episode_num<NUM_EPISODES; episode_num++)
			fprintf(record_file, "%d ", episode(10000));
		fprintf(record_file, ") ");
	}
	end_record();
	fclose(record_file);
	return 0;
}


///////////   Misc newish stuff

void open_record_file(char *filename)
{

	record_file = fopen(filename, "r");
	if (!record_file)
	{
		record_file = fopen(filename, "a");
		fprintf(record_file, "(:record-fields :MEMORY_SIZE :NUM_ACTIONS :runs :episodes :alg :updating :alpha :lambda :epsilon :gamma :data)\n");
	}
	else
	{
		fclose(record_file);
		record_file = fopen(filename, "a");
	}
}

void start_record()
{
	fprintf(record_file, "(%d %d %d %d :SARSA :ONLINE %f %f %f %f (",
		MEMORY_SIZE, NUM_ACTIONS, NUM_RUNS, NUM_EPISODES, alpha, lambda, epsilon, gamma);
}

void end_record()
{
	fprintf(record_file, "))\n");
}
