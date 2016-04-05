
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

int numberOfTeams, numberOfMatches;
std::vector<int> goalsForHome, goalsAgainstHome, goalsForAway, goalsAgainstAway;
std::vector<double> meanGoalsForHome, meanGoalsAgainstHome, meanGoalsForAway, meanGoalsAgainstAway;
double meanGoalsForHomeTotal, meanGoalsAgainstHomeTotal, meanGoalsForAwayTotal, meanGoalsAgainstAwayTotal;
std::vector<std::vector<std::pair<int, int> > > results;
std::map<std::string, int> mapTeamNameId;
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

	//std::cout << attackStrengthHome[teamA] << " " << defensStrengthAway[teamB] << " " << meanGoalsForHomeTotal << std::endl;

	homeExp = calculateHomeGoalsExpectancy(teamA, teamB);
	awayExp = calculateAwayGoalsExpectancy(teamA, teamB);

	//std::cout << homeExp << " " << awayExp << std::endl;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {

			possibleResults[i][j] = poisson(homeExp, i) * poisson(awayExp, j) * 100.;

			//std::cout << possibleResults[i][j] << " ";

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

	for (i = 0; i < numberOfTeams; i++) {

		attackStrengthHome.push_back(meanGoalsForHome[i] / meanGoalsForHomeTotal);
		defensStrengthHome.push_back(meanGoalsAgainstHome[i] / meanGoalsAgainstHomeTotal);
		attackStrengthAway.push_back(meanGoalsForAway[i] / meanGoalsForAwayTotal);
		defensStrengthAway.push_back(meanGoalsAgainstAway[i] / meanGoalsAgainstAwayTotal);

	}

}

void calculateMeans() {

	int i;

	meanGoalsForHomeTotal = std::accumulate(goalsForHome.begin(), goalsForHome.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfMatches * numberOfTeams);

	meanGoalsAgainstHomeTotal = std::accumulate(goalsAgainstHome.begin(), goalsAgainstHome.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfTeams * numberOfTeams);

	meanGoalsForAwayTotal = std::accumulate(goalsForAway.begin(), goalsForAway.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfTeams * numberOfTeams);

	meanGoalsAgainstAwayTotal = std::accumulate(goalsAgainstAway.begin(), goalsAgainstAway.end(), 0.0, [](double acc, int result) {
			return (acc + result);
	}) / (numberOfTeams * numberOfTeams);

	//std::cout << meanGoalsForHomeTotal << " " << meanGoalsAgainstHomeTotal << " " << meanGoalsForAwayTotal << " " << meanGoalsAgainstAwayTotal << std::endl;

	for (i = 0; i < numberOfTeams; i++) {
		meanGoalsForHome.push_back(((double) goalsForHome[i]) / numberOfMatches);
		meanGoalsAgainstHome.push_back(((double) goalsAgainstHome[i]) / numberOfMatches);
		meanGoalsForAway.push_back(((double) goalsForAway[i]) / numberOfMatches);
		meanGoalsAgainstAway.push_back(((double) goalsAgainstAway[i]) / numberOfMatches);
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

void calculateGoalsForMatches() {

	int numberOfMatches, idTeamA, idTeamB, scoresA, scoresB;
	std::string teamA, teamB, separetor;
	double probability;

	std::cin >> numberOfMatches;

	while (numberOfMatches--) {
	
		std::cin >> teamA >> separetor >> teamB;

		assert(mapTeamNameId.find(teamA) != mapTeamNameId.end());
		assert(mapTeamNameId.find(teamB) != mapTeamNameId.end());

		std::cout << "Expected result for " << teamA << " x " << teamB << std::endl;

		calculateExpectedResult(mapTeamNameId[teamA], mapTeamNameId[teamB], scoresA, scoresB, probability);
		
		std::cout << scoresA << " v " << scoresB << " (" << probability << "%)" << std::endl;

	}	

}

int main(int argc, char **argv) {

	std::string teamName, result;
	int position, wins, draws, loses, goalsFor, goalsAgainst, goalsDifference, points;
	int i, j, goalsForHomeValue, goalsAgainstHomeValue, goalsForAwayValue, goalsAgainstAwayValue, x, y;

	std::cin >> numberOfTeams;

	for (i = 0; i < numberOfTeams; i++) {
		std::cin >> position >> teamName >> numberOfMatches >> wins >> draws >> loses >> goalsFor >> goalsAgainst >> points >> goalsDifference;
		std::cout << position << " " << teamName << " " << numberOfMatches << " " << wins << " " << draws << " " << loses << " " << goalsFor << " " << goalsAgainst << " " << points << " " << goalsDifference << std::endl;

		mapTeamNameId[teamName] = i;

		results.push_back(std::vector<std::pair<int, int> >(numberOfTeams));

		for (j = 0; j < numberOfTeams; j++) {

			std::cin >> result;
			std::cout << result << std::endl;

			if (result[0] >= '0' && result[0] <= '9') {
				sscanf(result.c_str(), "%dx%d", &x, &y);
				std::cout << x << " " << y << std::endl;

				results[i][j] = (std::pair<int, int>(x, y));
			} else {
				results[i][j] = (std::pair<int, int>(0, 0));
			}
		}

	}

	initializeTables();
	calculateGoalsTable();
	calculateMeans();
	calculateStrength();
	calculateGoalsForMatches();

	return EXIT_SUCCESS;
}
