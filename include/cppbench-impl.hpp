#pragma once

#include <iostream>

using namespace std;

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
