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

# which accompanies this distribution, and is available

#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

use Win32::API;
use StatAPI;

########################################
# Set up initial data
# You will want to change:
#  - def_connection
#  - def_codetype
#  - loglevel
########################################

my $result = 1;
my $dll = "stat.dll";
# For the connection, refer to the enumeration in stat.h.
my $def_connection = 2;
my $def_codetype = "COM1";
my $logfile = "C:\\apps\\stat\\LogFiles\\stat_perl.log";
# For log level, specify 2 for command line testing, specify 3 for automatic testing.
my $loglevel = 2;
my $senddir = "/";  # (must end in 'slash')
my $recvdir = "Recvd/";
my $commondir = "Common/";
my $hndl = 0;		# a handle to a remote device connection

########################################
# Set options from parameters
########################################

if (@ARGV > 4)
	{
		print "Usage : perl stat.pl [connection type <";
		print $def_connection;
		print ">] [connection data<";
		print $def_codetype;
		print ">] [log level<";
		print $loglevel;
		print ">]\n";
		exit 0;
	}

if (@ARGV > 0)
	{
		$def_connection = @ARGV[0];
	}

if (@ARGV > 1)
	{
		$def_codetype = @ARGV[1];
	}

if (@ARGV > 2)
	{
		$loglevel = @ARGV[2];
	}

########################################
# Create the argument list
########################################

my @arglist = ($dll, $def_connection, $def_codetype, $loglevel, $logfile , $senddir, $recvdir, $commondir);

########################################
# Create the connection
########################################

print "==========================\n";
print "STAT API Connectivity Test\n";
print "==========================\n\n";

# set folders
Symbian::StatAPI::set_dll_folder("\\epoc32\\tools\\stat");

# initialise
Symbian::StatAPI::initialise(@arglist) or goto scripterror;	# initialise the DLL

# connect to a device
$hndl = Symbian::StatAPI::connect_to_target();
$hndl or goto scripterror;

Symbian::StatAPI::set_connection_logging();

Symbian::StatAPI::set_command_logging($hndl);

########################################
# The script
########################################

Symbian::StatAPI::get_screenshot($hndl);

Symbian::StatAPI::basic_key_sequence($hndl,'b');

Symbian::StatAPI::get_device_info($hndl);

Symbian::StatAPI::start_application($hndl,'Z:\Sys\Bin\Contacts.app','Test');
Symbian::StatAPI::control_keystroke($hndl,'Menu');
Symbian::StatAPI::control_keystroke($hndl,'RightArrow');
Symbian::StatAPI::basic_key_sequence($hndl,'1');
Symbian::StatAPI::basic_key_sequence($hndl,'Os');
Symbian::StatAPI::control_keystroke($hndl,'DownArrow');
Symbian::StatAPI::basic_key_sequence($hndl,'Symbian');
Symbian::StatAPI::combination_keystroke($hndl,'Ctrl+S');
Symbian::StatAPI::get_screenshot($hndl);
Symbian::StatAPI::combination_keystroke($hndl,'Ctrl+E');

Symbian::StatAPI::start_application($hndl,'Z:\Sys\Bin\Agenda.app','Test');
Symbian::StatAPI::combination_keystroke($hndl,'Shift+Ctrl+E');
Symbian::StatAPI::control_keystroke($hndl,'Enter');
Symbian::StatAPI::basic_key_sequence($hndl,'My Birthday');
Symbian::StatAPI::control_keystroke($hndl,'DownArrow');
Symbian::StatAPI::control_keystroke($hndl,'DownArrow');
Symbian::StatAPI::control_keystroke($hndl,'DownArrow');
Symbian::StatAPI::control_keystroke($hndl,'LeftArrow');
Symbian::StatAPI::control_keystroke($hndl,'Enter');
Symbian::StatAPI::get_screenshot($hndl);
Symbian::StatAPI::combination_keystroke($hndl,'Ctrl+E');

Symbian::StatAPI::start_application($hndl,'Z:\Sys\Bin\TimeW.app');
Symbian::StatAPI::combination_keystroke($hndl,'Ctrl+Shift+K');
Symbian::StatAPI::get_screenshot($hndl);
Symbian::StatAPI::combination_keystroke($hndl,'Ctrl+E');

Symbian::StatAPI::get_screenshot($hndl);

Symbian::StatAPI::copy_testfile_to_target($hndl,'StdAfx.sbr','C:\System\StdAfx.sb_');
Symbian::StatAPI::copy_from_target($hndl,'C:\System\StdAfx.sb_');
Symbian::StatAPI::remove_file($hndl,'C:\System\StdAfx.sb_');

goto endscript;

########################################
# Error handling routine
########################################
scripterror:
print "\nScript aborted due to error:\n" . Symbian::StatAPI::get_last_error($hndl) . "\n";
$result = 0;

########################################
# Finish
########################################
endscript:
$hndl and Symbian::StatAPI::disconnect_from_target($hndl) and print "Disconnected OK\n";
Symbian::StatAPI::close_connection_logging();		# close log file

exit ($result);

