#pragma once

#include <inttypes.h>
#include <time.h>

uint64_t nanotime();

/**
   Base class for benchmarks. Includes stuff for automatically
   running, making statistics, etc.
 */
class Benchmark {
public:
	Benchmark(int num_trials = 10);
	
	void run();

	virtual void setup();
	virtual void cleanup();

	double timePerIteration() const;
	double timePerIterationMicros() const;

	void setNumTrials(int num_trials);
	int getNumTrials() const;
	
protected:
	// These are the things that each benchmark needs to implement.
	
	virtual void run_iteration() = 0;
	virtual void finish_iteration() {};

private:
	int mNumTrials;
	int64_t mTotalTime;
};

#include "cppbench-impl.hpp"
