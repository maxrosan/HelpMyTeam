#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <random>
#include <cassert>
#include <cmath>
//#include <numeric>

int numberOfTeams, numberOfMatches;
std::vector<int> goalsForHome, goalsAgainstHome, goalsForAway, goalsAgainstAway;
std::vector<double> meanGoalsForHome, meanGoalsAgainstHome, meanGoalsForAway, meanGoalsAgainstAway;
double meanGoalsForHomeTotal, meanGoalsAgainstHomeTotal, meanGoalsForAwayTotal, meanGoalsAgainstAwayTotal;
std::vector<std::vector<std::pair<int, int> > > results;
std::map<std::string, int> mapTeamNameId;
std::map<int, std::string> mapIdTeamName;
std::vector<double> attackStrengthHome, defensStrengthHome, attackStrengthAway, defensStrengthAway;
double factorial[10];

void initializeTables() {

	int i;

	factorial[0] = 1.;

	for (i = 1; i < 10; i++) {
		factorial[i] = i * factorial[i-1];
	}

}

double poisson(double lambda, int x) {
	assert(x >= 0 && x <= 10);
	return (pow(lambda, x) / (factorial[x] * exp(lambda)));
}

double calculateHomeGoalsExpectancy(int teamA, int teamB) {
	return (attackStrengthHome[teamA] * defensStrengthAway[teamB] * meanGoalsForHomeTotal);
}

double calculateAwayGoalsExpectancy(int teamA, int teamB) {
	return (attackStrengthAway[teamB] * defensStrengthHome[teamA] * meanGoalsForAwayTotal);
}

void calculateExpectedResult(int teamA, int teamB, int& resultA, int& resultB, double& probability) {

	double homeExp, awayExp;
	int i, j, iMax = 0, jMax = 0;
	double possibleResults[10][10];

	//std::cout << "S: " << attackStrengthHome[teamA] << " " << defensStrengthAway[teamB] << " " << meanGoalsForHomeTotal << std::endl;

	homeExp = calculateHomeGoalsExpectancy(teamA, teamB);
	awayExp = calculateAwayGoalsExpectancy(teamA, teamB);

	//std::cout << homeExp << " " << awayExp << std::endl;

	for (i = 0; i < 10; i++)
		for (j = 0; j < 10; j++)
			possibleResults[i][j] = 0;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {

			possibleResults[i][j] = poisson(homeExp, i) * poisson(awayExp, j) * 100.;

			//std::cout << possibleResults[i][j] << " (" <<  possibleResults[iMax][jMax] << " )";

			if (possibleResults[i][j] > possibleResults[iMax][jMax]) {
				iMax = i;
				jMax = j;
			}
		}

		//std::cout << std::endl;
	}

	resultA = iMax;
	resultB = jMax;
	probability = possibleResults[iMax][jMax];

}

void calculateStrength() {

	int i;

	//std::cout << "strength = " << meanGoalsForHome.size() << std::endl;

	for (i = 0; i < numberOfTeams; i++) {

		std::cout << "A: " << meanGoalsForHome[i] << " " << meanGoalsForHomeTotal << std::endl;

		attackStrengthHome.push_back(meanGoalsForHome[i] / meanGoalsForHomeTotal);
		defensStrengthHome.push_back(meanGoalsAgainstHome[i] / meanGoalsAgainstHomeTotal);
		attackStrengthAway.push_back(meanGoalsForAway[i] / meanGoalsForAwayTotal);
		defensStrengthAway.push_back(meanGoalsAgainstAway[i] / meanGoalsAgainstAwayTotal);

	}

}

void calculateMeans() {

	int i;

	std::cout << "Calculating means" << std::endl;

	meanGoalsForHomeTotal = std::accumulate(goalsForHome.begin(), goalsForHome.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfMatches * numberOfTeams / 2);

	meanGoalsAgainstHomeTotal = std::accumulate(goalsAgainstHome.begin(), goalsAgainstHome.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfTeams * numberOfTeams / 2);

	meanGoalsForAwayTotal = std::accumulate(goalsForAway.begin(), goalsForAway.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfTeams * numberOfTeams / 2);

	meanGoalsAgainstAwayTotal = std::accumulate(goalsAgainstAway.begin(), goalsAgainstAway.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfTeams * numberOfTeams / 2);

	std::cout << "means " << meanGoalsForHomeTotal << " " << meanGoalsAgainstHomeTotal << " " << meanGoalsForAwayTotal << " " << meanGoalsAgainstAwayTotal << std::endl;

	for (i = 0; i < numberOfTeams; i++) {
		meanGoalsForHome.push_back(((double) goalsForHome[i]) / ( numberOfMatches / 2 ));
		meanGoalsAgainstHome.push_back(((double) goalsAgainstHome[i]) /  ( numberOfMatches / 2));
		meanGoalsForAway.push_back(((double) goalsForAway[i]) / ( numberOfMatches / 2 ));
		meanGoalsAgainstAway.push_back(((double) goalsAgainstAway[i]) / (numberOfMatches / 2));
	}

}

void calculateGoalsTable() {

	int i, j, goalsForAwayValue, goalsAgainstAwayValue;

	/*for (i = 0; i < numberOfTeams; i++) {
		for (j = 0; j < numberOfTeams; j++) {
			std::cout << results[i][j].first << "x" << results[i][j].second << " "; 
		}
		std::cout << std::endl;
	}*/

	for (i = 0; i < numberOfTeams; i++) {

		std::cout << "Team " << i << std::endl;

		goalsForHome.push_back(std::accumulate(results[i].begin(), results[i].end(), 0, [](int acc, std::pair<int, int> result) {
			return (acc + result.first);
		}));

		goalsAgainstHome.push_back(std::accumulate(results[i].begin(), results[i].end(), 0, [](int acc, std::pair<int, int> result) {
			return (acc + result.second);
		}));

		goalsAgainstAwayValue = 0;
		goalsForAwayValue = 0;

		for (j = 0; j < numberOfTeams; j++) {

			goalsForAwayValue += results[j][i].second;
			goalsAgainstAwayValue += results[j][i].first;
			
		}

		goalsForAway.push_back(goalsForAwayValue);
		goalsAgainstAway.push_back(goalsAgainstAwayValue);

		//std::cout << goalsForHome[i] << " " << goalsAgainstHome[i] << " " << goalsForAwayValue << " " << goalsAgainstAwayValue << std::endl;


	}

	//std::cout << a << " " << b << " " << c << " " << d << std::endl;
}

void setNumberOfMatches(int _numberOfMatches) {
	numberOfMatches = _numberOfMatches;
}

void setNumberOfTeams(int _numberOfTeams) {
	int i, j;

	numberOfTeams = _numberOfTeams;

	for (i = 0; i < numberOfTeams; i++) {
		results.push_back(std::vector<std::pair<int, int> >(numberOfTeams));

		for (j = 0; j < numberOfTeams; j++) {
			results[i][j] = (std::pair<int, int>(0, 0));
		}
	}
}

void printNumberOfTeams() {
	std::cout << "number of teams " << numberOfTeams << std::endl;
}

void addTeam(std::string teamName, int index) {

	std::cout << teamName << " " << index << std::endl;

	mapTeamNameId[teamName] = index;
	mapIdTeamName[index] = teamName;
}

void setMatchResult(int a, int b, int scoreA, int scoreB) {

	std::cout << mapIdTeamName[a] << " " << mapIdTeamName[b] << " " << scoreA << " " << scoreB << std::endl;

	results[a][b] = (std::pair<int, int>(scoreA, scoreB));
}

void calculateGoalsForMatch(int a, int b) {
	double probability;
	int scoreA, scoreB;

	//std::cout << "calc " << mapIdTeamName[a] << " " << mapIdTeamName[b] << std::endl;

	calculateExpectedResult(a, b, scoreA, scoreB, probability);

	std::cout << mapIdTeamName[a] << " " << scoreA << " x " << scoreB << " " << mapIdTeamName[b] << " " << probability << "%" << std::endl;

}

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(prediction)
{
	def("setNumberOfTeams", setNumberOfTeams);
	def("setNumberOfMatches", setNumberOfMatches);
	def("printNumberOfTeams", printNumberOfTeams);
	def("addTeam", addTeam);
	def("setMatchResult", setMatchResult);

	def("initializeTables", initializeTables);
	def("calculateGoalsTable", calculateGoalsTable);
	def("calculateMeans", calculateMeans);
	def("calculateStrength", calculateStrength);
	def("calculateGoalsForMatch", calculateGoalsForMatch);

	//def("calculateGoalsForMatches", calculateGoalsForMatches);
}
