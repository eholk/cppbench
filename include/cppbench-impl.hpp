#pragma once

#include <iostream>

#include <boost/math/distributions/students_t.hpp>

using namespace std;
using namespace boost::math;

uint64_t nanotime() {
    uint64_t ns_per_s = 1000000000LL;
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * (uint64_t)ns_per_s + (uint64_t)ts.tv_nsec);
}


SimpleBenchmarkRunner::SimpleBenchmarkRunner(int num_trials)
	: mNumTrials(num_trials), mTotalTime(0)
{}

/**
   Runs a benchmark.

   It runs it a couple of times just to warm everything up, and then
   does num_trials iterations of the benchmark for time.
 */
void SimpleBenchmarkRunner::run(Benchmark &bench) {
	bench.setup();

	// Warm up.
	bench.run_iteration();
	bench.run_iteration();
	bench.finish_iteration();

	// okay, now for time.
	uint64_t start1 = nanotime();
	for(int i = 0; i < mNumTrials; ++i) {
		bench.finish_iteration();
	}
	uint64_t stop1 = nanotime();

	uint64_t start2 = nanotime();
	for(int i = 0; i < mNumTrials; ++i) {
		bench.run_iteration();
		bench.finish_iteration();
	}
	uint64_t stop2 = nanotime();

	mTotalTime = (stop2 - start2) - (stop1 - start1);
	
	bench.cleanup();
}

double SimpleBenchmarkRunner::timePerIterationMicros() const {
	return double(mTotalTime) / (mNumTrials * 1000);
}

double SimpleBenchmarkRunner::timePerIteration() const {
	return double(mTotalTime) / mNumTrials / 1e9;
}

void Benchmark::setup() {}
void Benchmark::cleanup() {}

void SimpleBenchmarkRunner::setNumTrials(int num) {
	mNumTrials = num;
}

int SimpleBenchmarkRunner::getNumTrials() const {
	return mNumTrials;
}

void AdvancedBenchmarkRunner::run(Benchmark &bench) {
    mTrials.clear();
	bench.setup();

	// Warm up.
	bench.run_iteration();
	bench.run_iteration();
	bench.finish_iteration();

	// okay, now for time.
	for(int i = 0; i < mNumTrials; ++i) {
        auto start = nanotime();
		bench.run_iteration();
        auto stop = nanotime();
		bench.finish_iteration();

        mTrials.push_back(stop - start);
	}
	
	bench.cleanup();
}

double AdvancedBenchmarkRunner::timePerIterationMicros() const {
    double total = 0;
    for(auto t : mTrials) {
        total += t;
    }
	return total / (mNumTrials * 1000);
}

double AdvancedBenchmarkRunner::timePerIteration() const {
	return timePerIterationMicros() / 1e6;
}

void AdvancedBenchmarkRunner::setNumTrials(int num) {
	mNumTrials = num;
}

int AdvancedBenchmarkRunner::getNumTrials() const {
	return mNumTrials;
}

double AdvancedBenchmarkRunner::getStdDev() const {
    auto mean = timePerIteration();

    double var = 0;
    for(auto t : mTrials) {
        auto x = (t / 1e9) - mean;
        var += x * x;
    }

    return sqrt(var) / getNumTrials();
}

double AdvancedBenchmarkRunner::getHalfWidth(double level) const {
	students_t dist(getNumTrials() - 1);

	auto alpha = (1.0 - level) / 2;
	
	double T = quantile(complement(dist, alpha));
	assert(T > 0);
	return T * getStdDev() / sqrt(double(getNumTrials()));
}

double AdvancedBenchmarkRunner::confidenceWidth(double level) const {
	return 2.0 * getHalfWidth(level);
}

std::pair<double, double>
AdvancedBenchmarkRunner::confidenceInterval(double level) const {
	auto w = getHalfWidth(level);
	auto mu = timePerIteration();

	return std::make_pair(mu - w, mu + w);
}
