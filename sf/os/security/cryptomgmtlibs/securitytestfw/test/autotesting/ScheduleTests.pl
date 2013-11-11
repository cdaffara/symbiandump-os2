#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

use strict;

# This program simply schedules the tests to run every day at 9pm
# Currently only builds against Typhoon mainline 02129
for (;;)
{
	(my $hour, my $day) = (localtime)[2, 3];
	if ($hour == 20)
	{
		# for the moment, do unzip, perforce sync, build, and test, but using wins/udeb
		system("perl -w RunSecurityTests.pl -a");
	}

	(my $date = `date /t`) =~ s/\n//;
	(my $time = `time /t`) =~ s/\n//;
	print "Sleeping for 59 mins at $date $time\n";

	# now sleep for 59 minutes
	sleep 3500;
}
