
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <climits>

#include "Sim_Math.h"
#include "TimeCode_Tests.h"
#include "BigInteger_Tests.h"

using namespace std;

void power_tests(){
	cout << "\tpower tests..." ;
	assert(power(2, 0) == 1);
	assert(power(2, 1) == 2);
	assert(power(2, 2) == 4);
	assert(power(0, 0) == 1);
	assert(power(0, 1) == 0);
	assert(power(1, 0) == 1);
	assert(power(2, 15) == 32768);
	assert(power(2, 30) == 1073741824);
	assert(power(2, 46) == 70368744177664);

	assert(power(e, -3) == 0.049787068367863951);
	cout << " PASSED!" << endl;
}



void poisson_tests(){
	cout << "\tpoisson tests...";

	// The old test required 3 to be the most common value in just 100 samples.
	// That failed even in the unchanged starter on this Mac. Check the mean and
	// variance of a larger sample instead: both should be close to lambda = 3.
	// This only changes the test, not the simulation's random-number sequence.
	// Sources: ChatGPT:
	// https://www.itl.nist.gov/div898/handbook/eda/section3/eda366j.htm
	const int sample_count = 10000;
	double sum = 0.0;
	double squared_sum = 0.0;
	for(int i = 0; i < sample_count; i++){
		int sample = poisson(3);
		assert(sample >= 0);
		sum += sample;
		squared_sum += static_cast<double>(sample) * sample;
	}
	double average = sum / sample_count;
	double variance = squared_sum / sample_count - average * average;
	assert(average > 2.85 && average < 3.15);
	assert(variance > 2.7 && variance < 3.3);

	cout << " PASSED!" << endl;
}


void timecode_tests(){
	cout << "\ttimecode tests...";

	TestComponentsToSeconds();
	TestDefaultConstructor();
	TestComponentConstructor();
	TestGetComponents();
	TestGetTimeCodeAsSeconds();
	TestCopyConstructor();
	
	TestGetHours();
	TestGetMinutes();
	TestGetSeconds();
	
	TestSetHours();
	TestSetMinutes();
	TestSetSeconds();
	
	TestReset();
	
	TestAdd();
	TestSubtract();
	TestMultiply();
	TestDivide();

	
	TestEquals();
	TestLessThan();
	TestGreaterThan();
	
	TestAverage();

	cout << " PASSED!" << endl;
}


void biginteger_tests(){
	cout << "\tbiginteger tests..."; 
	
	TestConstructor();
	TestMakeNegative();
	CopyConstructorTests();
	AdditionTests();
	OverflowTests();

	cout << " PASSED!" << endl;
}


int main(){

	cout << "--- Running Tests ---" << endl;

	power_tests();
	poisson_tests();
	timecode_tests();
	biginteger_tests();

	cout << "--- ALL TESTS PASSED! ---" << endl;


	return 0;
}