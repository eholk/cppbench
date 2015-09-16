#pragma once

#include <inttypes.h>
#include <time.h>

#include <vector>

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

/**
   This benchmark runner can computer statistics like standard
   deviation and confidence interval.
 */
class AdvancedBenchmarkRunner : public IBenchmarkRunner {
public:
    AdvancedBenchmarkRunner(int num_trials = 10) : mNumTrials(num_trials) {}
    
    virtual void run(Benchmark &bench);

    virtual double timePerIteration() const;
	virtual double timePerIterationMicros() const;

	virtual void setNumTrials(int num_trials);
	virtual int getNumTrials() const;

	/// Return the sample standard deviation
    double getStdDev() const;

	double confidenceWidth(double level = 0.95) const;
	std::pair<double, double> confidenceInterval(double level = 0.95) const;
	
protected:
    int mNumTrials;

    /// The time for each trial in nanoseconds.
    std::vector<double> mTrials;

	double getHalfWidth(double level) const;
};

#include "cppbench-impl.hpp"
