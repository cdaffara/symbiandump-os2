#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
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

use Net::Telnet;

$telnet = new Net::Telnet ( Timeout=>30, Errmode=>'die'); 
$telnet->open("192.168.0.3");
$telnet->login("any", "any");
@lines = $telnet->cmd("testexecute  z:\\testdata\\scripts\\TEFLoggerTest.script");
print @lines;
@lines = $telnet->cmd("copy c:\\logs\\testexecute\\tefloggertest.htm c:\\logs\\testexecute\\tefloggertestPIPSINI.htm ");
print @lines;
@lines = $telnet->cmd("copy c:\\backupTEFc.ini  c:\\system\\data\\testexecute.ini");
print @lines;
@lines = $telnet->cmd("copy c:\\backupTEFz.ini  z:\\system\\data\\testexecute.ini ");
print @lines;
@lines = $telnet->cmd("del c:\\backupTEF*.ini ");
print @lines;
