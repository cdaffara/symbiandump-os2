This directory contains the scripts to use with the tcertstore test harness

cancel1.txt
	This file tests the cancellation of the initialization and deletion of 
	a CUnifiedCertStore. The number of opened stores is used to check if the 
	operation succeeded.

oom1.txt
	This file tests the opening and deletion of a CUnifiedCertStore in OOM 
	conditions. The number of opened stores is used to check if the 
	operation succeeded.

oom2.txt
	This file tests the addition, listing, and deletion of 
	one single certificate in OOM conditions.