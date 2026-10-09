

// author: Programmer Intern Jordan

#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include <sqlite3.h>

#include "Sim_Math.h"


double power(double a, int b){
	if(b == 0){
		return 1;
	}

	if(a == 0){
		return 0;
	}

	// If b is negative, we flip the sign now and then later
	// we return 1.0 divided by the "normally" computed answer for a^b
	bool needs_inversion = false;
	if(b < 0){
		needs_inversion = true;
		b = -b;
	}
	
	// The recursive version did these same multiplications after returning.
	// A loop avoids the function calls and keeps the floating-point order.
	// Source: original function and ChatGPT assistance
	double ans = a;
	for(int i = 1; i < b; i++){
		ans = a * ans;
	}

	if(needs_inversion){
		ans = 1.0 / ans;
	}

	return ans;
}






int poisson(int lambda){ // lambda is the technical term, this value is the desired average
	// Knuth's algorithm for Poisson random numbers
	double L = power(e, -lambda);
	int k = 0;
	double p = 1.0;

	while(p > L){
		k = k + 1;
		double u = (double)rand() / RAND_MAX; // gives a uniformly random number between 0 and 1
		p *= u;
	}

	return k-1;
}


int binomial(int n, double p){
	// Binomial distribution, the number of successful trials in n Bernoulli events
	// Bernoulli events are simple:
		// each event is either a success or failure
		// all events are independent
		// all events have the same probability of success (p)
	// n gives the number of trials, in this case the number of cars
	// p gives the probability of success, in this case the probability an individual car exits

	int count = 0;
	for (int i = 0; i < n; i++){ // for each car
		double u = (double)rand() / RAND_MAX; // generate a uniform random number between 0 and 1
		if(u < p){ // if that random number is less than the probability of success, the car leaves
			count = count + 1; // count the cars that leave
		}
	}
	
	return count; // return the number of cars that left
}

int percentage(int portion, int overall){
	// returns portion / overall as a percentage
	// for example, if portion = 16 and overall = 82
	// then this function returns 19% = .019 = 16/82
	double per = (portion / static_cast<double>(overall)) * 100;
	return static_cast<int>(per);
}


bool crash_occurs(double prob_of_crash_today){
	if( ((double)rand() / RAND_MAX) < prob_of_crash_today ){
		return true;
	}
	return false;
}


int day_of_week_code(TimeCode cur_time){
	int cur_hour = cur_time.GetHours();
	cur_hour = cur_hour % 168;
	if(cur_hour <= 24){
		return 0; // Sunday
	} else if(cur_hour <= 48){
		return 1; // Monday
	} else if(cur_hour <= 72){
		return 2; // Tuesday
	} else if(cur_hour <= 96){
		return 3; // Wednesday
	} else if(cur_hour <= 120){
		return 4; // Thursday
	} else if(cur_hour <= 144){
		return 5; // Friday
	} else {
		return 6; // Saturday
	}
}

std::string day_of_week_name(int code){
	switch(code){
		case 0: return "Sunday";
		case 1: return "Monday";
		case 2: return "Tuesday";
		case 3: return "Wednesday";
		case 4: return "Thursday";
		case 5: return "Friday";
		case 6: return "Saturday";
		default: 
			throw std::invalid_argument("Invalid day of week code: " + std::to_string(code));
	}
}


int prepare(sqlite3* db, const char* query, sqlite3_stmt** stmt){
	int rc = sqlite3_prepare_v2(db, query, -1, stmt, 0); // (db, query, ?, response/answer, ?)
	if(rc != SQLITE_OK){
		sqlite3_close(db);
		throw std::runtime_error("Could not prepare database query.");
	}
	return rc;
}


// Build the probability for each weekday from the current database.
std::unordered_map<std::string, double> build_crash_prob_map(){
	// Count rows in SQL instead of running a query for every crash and day.
	// The denominator includes ALL incidents, even those with no automobiles.
	// Sources: OpenAI Codex assistance, October 2026;
	// https://www.sqlite.org/lang_select.html (WHERE and GROUP BY)
	// https://www.sqlite.org/c3ref/finalize.html (release each statement)
	sqlite3* db = nullptr;
	int rc = sqlite3_open("CRASH_LANCASTER_2024.db", &db);
	if(rc != SQLITE_OK){
		sqlite3_close(db);
		throw std::runtime_error("Could not open CRASH_LANCASTER_2024.db database file!");
	}

	sqlite3_stmt* stmt = nullptr;
	double total_crashes = 0.0;
	prepare(db, "SELECT COUNT(*) FROM incidents;", &stmt);
	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW){
		total_crashes = static_cast<double>(sqlite3_column_int64(stmt, 0));
	}
	sqlite3_finalize(stmt);
	if(rc != SQLITE_ROW){
		sqlite3_close(db);
		throw std::runtime_error("Could not count incidents.");
	}

	// Start with all seven days so a day with no matching rows still has 0.
	// Keep the original insertion order so terminal output order stays the same.
	std::unordered_map<std::string, double> crash_prob_map;
	for(int dow = 1; dow <= 7; dow++){
		crash_prob_map[day_of_week_name(dow - 1)] = 0.0;
	}

	const char* query =
		"SELECT DAY_OF_WEEK, COUNT(*) FROM incidents "
		"WHERE AUTOMOBILE_COUNT != 0 GROUP BY DAY_OF_WEEK;";
	prepare(db, query, &stmt);
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
		int dow = sqlite3_column_int(stmt, 0);
		double count = static_cast<double>(sqlite3_column_int64(stmt, 1));
		if(dow >= 1 && dow <= 7 && total_crashes != 0){
			crash_prob_map[day_of_week_name(dow - 1)] += count / total_crashes;
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	if(rc != SQLITE_DONE){
		throw std::runtime_error("Could not count crashes by day.");
	}

	return crash_prob_map;
}
