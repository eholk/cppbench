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
	virtual void setup();
	virtual void cleanup();

	// These are the things that each benchmark needs to implement.	
	virtual void run_iteration() = 0;
	virtual void finish_iteration() {};
};

class IBenchmarkRunner {
public:
	virtual void run(Benchmark &bench) = 0;

    virtual double timePerIteration() const = 0;
	virtual double timePerIterationMicros() const = 0;

	virtual void setNumTrials(int num_trials) = 0;
	virtual int getNumTrials() const = 0;
};

class SimpleBenchmarkRunner : public IBenchmarkRunner {
public:
    SimpleBenchmarkRunner(int num_trials = 10);

    virtual void run(Benchmark &bench);

    virtual double timePerIteration() const;
	virtual double timePerIterationMicros() const;

	virtual void setNumTrials(int num_trials);
	virtual int getNumTrials() const;

protected:
    int mNumTrials;
	int64_t mTotalTime;
};

#include "cppbench-impl.hpp"
