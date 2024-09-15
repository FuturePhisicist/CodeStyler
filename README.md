	clang++ -shared -fPIC -o libStyleCheckerPlugin.so CodeStyleCheckerMain.cpp CodeStyleChecker.cpp `llvm-config --cxxflags --ldflags --system-libs --libs all`
	clang -cc1 -load ./libStyleCheckerPlugin.so -plugin hello-world bad_code.cpp

Rule 1.1
F

Rule 1.2
F + P

Rule 2.1
F

Rule 2.2
F

Rule 2.3
F

Rule 3.1
P

Rule 3.2
P

Rule 3.3
P

Rule 3.4
P

Rule 3.5
P

Rule 3.6
P

Rule 3.7
N

Rule 4.1
F + P

Rule 4.2
F

Rule 4.3
F

Rule 4.4.1
P

Rule 4.4.2
F

Rule 4.4.3
S

Rule 4.4.4
P

Rule 4.4.5
P

Rule 4.5.1
F

Rule 4.5.2
P

Rule 4.5.3
P

Rule 4.6.1
F

Rule 4.6.2
F

Rule 4.6.3
F

Rule 4.6.4
F

Rule 4.6.5
DOESN'T EXIST!!!


Rule 4.6.6
F

Rule 4.6.7
F

Rule 4.6.8
F + P (check for break;)

Rule 4.6.9
F

Rule 4.6.10
P

Rule 4.6.11
F

Rule 4.6.12
?

Rule 4.6.13
F

Rule 4.7.1
F

Rule 4.7.2
F

Rule 4.7.3
F

Rule 4.7.4
F

Rule 4.7.5
F

Rule 4.7.6
F

Rule 4.7.7
F

Rule 4.7.8
P

Rule 4.7.9
?

Rule 5.1
P

Rule 5.2
P

Rule 5.3
P

Rule 5.4
S

Rule 5.5
S

Rule 5.6
S

Rule 5.7
P (-Wmain)

Rule 5.8
P

Rule 5.9
S

Rule 5.10
P

Rule 5.11
P

Rule 5.12
S

Rule 5.13
?

Rule 5.14
S

Rule 5.15
P

Rule 5.16
-Wvla

Rule 5.17
S

Rule 5.18
P

Rule 6.1
Rule 6.2
Rule 6.3
Rule 6.4
Rule 6.5
Rule 6.6