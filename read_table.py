
import prediction, sys
from sklearn.neural_network import MLPClassifier

#prediction.setNumberOfTeams(10)
#prediction.printNumberOfTeams()
#prediction.addTeam("Flamengo", 0)

table = open(sys.argv[1])
l = table.readline()

teams = [ ]
results = [ ]

while len(l) > 1:

	cols = l.replace('\xe2\x80\x93', '-').replace('\xe2\x80\x94', '*').replace('\n', '').split('\t')
	#print cols

	team = cols[0].replace(' ','')
	teams.append(team)
	results.append(cols[1:])

	#print '[',l,']'
	l = table.readline()

prediction.setNumberOfTeams(len(teams))

for i in xrange(len(teams)):
	prediction.addTeam(teams[i], i)

roundNumber = 100
numberOfMatches = 0

clf = MLPClassifier(solver='lbgfs', alpha=1e-5, hidden_layer_sizes=(8,), random_state=1)

inCLF = [ ]
outCLF = [ ]

for i in xrange(len(teams)):
	z = 0
	for j in xrange(len(teams)):
		if results[i][j][0] != '*' and results[i][j][0] != 'R':
			r = results[i][j].split('-')
			#print r
			prediction.setMatchResult(i, j, int(r[0]), int(r[1]))
			z += 1

			inCLF.append([i, j])
			if int(r[0]) == int(r[1]):
				outCLF.append(0.)
			if int(r[0]) > int(r[1]):
				outCLF.append(-1.)
			if int(r[0]) < int(r[1]):
				outCLF.append(1.)
	
		elif results[i][j][0] == 'R':
			r = int(results[i][j].split('-')[1])
			roundNumber = min(r, roundNumber)

	numberOfMatches = roundNumber - 1

prediction.setNumberOfMatches(numberOfMatches)

prediction.initializeTables()
prediction.calculateGoalsTable()
prediction.calculateMeans()
prediction.calculateStrength()

clf.fit(inCLF, outCLF)

print 'next round =', roundNumber

for i in xrange(len(teams)):
	for j in xrange(len(teams)):
		if results[i][j][0] == 'R':
			r = int(results[i][j][2:])
			if r == roundNumber:
				#print 'Round ', results[i][j], ':', 
				if clf.predict([[i, j]])[0] == -1:
					print '# casa ganha (RN) # poisson: ',
				elif clf.predict([[i, j]])[0] == 0:
					print '# empate (RN) # poisson: ',
				else:
					print '# visitante ganha (RN) # poisson: ',
				prediction.calculateGoalsForMatch(i, j)
				
