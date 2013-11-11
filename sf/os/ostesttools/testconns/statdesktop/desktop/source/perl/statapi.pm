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

#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

package Symbian::StatAPI;
require 5.000;
use Win32::API;
use Win32::TieRegistry(Delimiter=>"/");
use IO::File;
use File::Basename;
use LWP::Simple;
use LWP::UserAgent;
use HTTP::Request;
use HTTP::Response;
use Exporter;
use vars qw (@ISA @EXPORT $VERSION);
@ISA		= qw(Exporter);
@EXPORT		= qw(
				&set_dll_folder
				&set_working_folder
				&initialise
				&get_version
				&set_connection_logging
				&close_connection_logging
				&get_device_log_file
				&set_screenshot_dir
				&get_last_error
				&connect_to_target
				&connect_to_target_multithreaded
				&disconnect_from_target
				&set_command_delay
				&set_image_verification
				&set_command_logging
				&get_screenshot
				&get_device_info
				&touch_screen
				&start_application
				&start_program
				&open_application
				&close_application
				&basic_key_sequence
				&control_keystroke
				&combination_keystroke
				&pause
				&start_eshell
				&stop_eshell
				&copy_commonfile_to_target
				&copy_testfile_to_target
				&copy_buildfile_to_target
				&move_from_target
				&copy_from_target
				&remove_unwanted_file
				&send_command_file
				&send_raw_command
				&hold_key
				&remove_file
				&create_folder
				&remove_folder);


##########################################################################
# AUTO-GENERATED BLOCK - DO NOT MODIFY
##########################################################################
$VERSION = 3.3;
##########################################################################


# global variables
my $FUNCTION;			# dll function entrypoint
my $dllname;			# dll to call
my $def_connection_type;	# default (global) connection type
my $def_codetype;		# default (global) code type
my $lasterror;			# last error thrown
my $imagedir;			# image retrieve location
my $logfile;			# log file to write to
my $senddir;			# temp send directory
my $recvdir;			# temp receive directory
my $workdir;			# stat working directory
my $statdir;			# stat dll location
my $loglevel;			# internal logging level

#TMS required vars
my $httpserver;  		# http server for downloading files
my $runno;			# needed to obtain build location to download files
my $commondir;			# common files temp dir
my $builddir;			# build files location


################################################################
# exported functions
# Come in 2 levels: Global - apply to all connections, 
#                   Local - apply to individual connections
################################################################

################################################################
# GLOBAL ENTRYPOINTS
# Apply to all connections
################################################################

################################################################
# 	set_dll_folder(<dll folder>);
#
# 	Required parameters:
#	$statdir 	e.g. "C:\epoc32\tools\STAT";
#
################################################################
sub set_dll_folder
{
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $dir = $cmdref->[0];

	$statdir = $dir;

	return 1;
}


################################################################
# 	set_working_folder(<working folder>);
#
# 	Required parameters:
#	$statdir 	e.g. "C:\apps\STAT";
#
################################################################
sub set_working_folder
{
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $dir = $cmdref->[0];

	$workdir = $dir;

	return 1;
}


################################################################
# Must be the first function called
# 	initialise(@ARGV);
# 	initialise(<dll name>, <connection type>, <code type>, <log level>, <log file>, <send dir>, <recv dir>, <common dir>, <httpserver>, <runno>, <builddir>);
#
# 	Required parameters:
#	$dllname 	e.g. STAT.DLL;
#
#	Needed only if connection/code types won't change throughout the script
#	$def_connection_type	e.g. 2 (Symbian Serial)
#	$def_codetype		e.g. COM2
#
#	Needed only for logging
#	$loglevel 	# internal logging level:
			# 0 = terse
			# 1 = noraml
			# 2 = verbose
			# 3 = Perl output redirected to log file
#	$logfile 	e.g. C:\Apps\Stat\LogFiles\StatPerl.log;
#
#	Needed only for sending/receiving files (must end in 'slash')
#	$senddir 	e.g. auto/12267/send/
#	$recvdir 	e.g. auto/12267/recv/
#
#	Needed only for sending common files to target (TMS only - must end in 'slash')
#	$commondir 	e.g. auto/12267/common/

#	Needed only for sending build files to target (TMS only)
#	$httpserver e.g. http://lon-adminpool01:8080/AutoTest
#	$runno	e.g. 150
#	$builddir 	e.g. auto/12267/build/	(must end in 'slash')
#
################################################################
sub initialise
{
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	$dllname = $cmdref->[0];
	$def_connection_type = $cmdref->[1];
	$def_codetype = $cmdref->[2];
	$loglevel = $cmdref->[3];
	$logfile = $cmdref->[4];
	$senddir = $cmdref->[5];
	$recvdir = $cmdref->[6];
	$commondir = $cmdref->[7];
	$httpserver = $cmdref->[8];
	$runno = $cmdref->[9];
	$builddir = $cmdref->[10];

	# required parameter
	if (!$dllname)
	{
		$lasterror = "DLL name not supplied!";
		Line(get_last_error());
		return 0;
	}

	# adjust log level if necessary
	if ($loglevel < 0)
	{
		$loglevel = 0;
	}
	if ($loglevel > 3)
	{
		$loglevel = 3;
	}
	
	# redirect output to log file if we've supplied one, else will go to screen
	if ($logfile and ($loglevel eq 3))
	{
		print "Redirecting output to [$logfile]...\n";
		open(STDOUT, ">> $logfile");
		open(STDERR, ">> $logfile");
	}
		
	# reverse '/' to '\\' in filenames
	$senddir =~ s/\//\\/g;
	$recvdir =~ s/\//\\/g;
	$commondir =~ s/\//\\/g;
	$builddir =~ s/\//\\/g;

	if (!$workdir)
	{
		$workdir = "C:\\apps\\stat";
	}
	Line("Working directory: $workdir\n");

	if (!$statdir)
	{
		$statdir = "C:\\epoc32\\tools\\STAT";
	}
	Line("Stat directory: $statdir\n");

	Line("Initialising STAT API:\n  DLLName [$dllname]\n  Default Connection [$def_connection_type]\n  Default Code Type [$def_codetype]\n  Log Level [$loglevel]\n  LogFile [$logfile]\n  Send [$senddir]\n  Recv [$recvdir]\n  Common [$commondir]\n  HTTPServer [$httpserver]\n  Run No [$runno]\n  Build [$builddir]\n");
	return 1;
}


################################################################
# Get the version of the dll
################################################################
sub get_version
{
	$FUNCTION = new Win32::API("$statdir/$dllname", "StdVersion", [V], P);
	if (defined($FUNCTION))
	{
		$ret = $FUNCTION->Call();
		if ($ret)
		{
			return $ret;
		}
		else
		{
			$lasterror = "Version could not be established\n";
		}
	}
	else
	{
		$lasterror = "Function StdVersion not found\n";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Set logging of stat connection attempts
# param0: logfile
################################################################
sub set_connection_logging
{
	Line("Setting connection logging...");
	if ($loglevel and $logfile)
	{
		$FUNCTION = new Win32::API("$statdir/$dllname", "StdSetConnectionLogging", [P], I);
		if (defined($FUNCTION))
		{
			Line("Logging to $logfile");
			$ret = $FUNCTION->Call($logfile);
			if ($ret)
			{
				return 1;
			}
			else
			{
				GetDllError(0);
			}
		}
		else
		{
			$lasterror = "Function StdSetConnectionLogging not found\n";
		}
	}
	else
	{
		$lasterror = "Logging disabled or logfile not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Set logging of stat connection attempts
# param0: logfile
################################################################
sub close_connection_logging
{
	Line("Closing connection logging...");
	if ($loglevel and $logfile)
	{
		$FUNCTION = new Win32::API("$statdir/$dllname", "StdCloseConnectionLogging", [], V);
		if (defined($FUNCTION))
		{
			Line("Stopping logging to $logfile");
			$FUNCTION->Call();
			return 1;
		}
		else
		{
			$lasterror = "Function StdCloseConnectionLogging not found\n";
		}
	}
	else
	{
		$lasterror = "Logging disabled or logfile not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Retrieve the device log file to our log and truncate it at that point on the device
################################################################
sub get_device_log_file
{
	Line("Retrieving log file...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];

	if ($loglevel and $logfile)
	{
		if ($hndl)
		{
			if (SendCommand($hndl, "<B><G><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Handle not supplied";
		}
	}
	else
	{
		$lasterror = "Logging disabled or logfile not supplied";
	}
	
	Line(get_last_error());
	return 0;
}


################################################################
# Set the output directory for screenshots
################################################################
sub set_screenshot_dir
{
	Line("Setting screenshot dir...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $dir = $cmdref->[0];
	if ($dir)
	{
		$imagedir = $dir;

		# reverse '/' to '\\' for DOS
		$imagedir =~ s/\//\\/g;
		
		Line("Directory ($imagedir)");
		return 1;
	}

	$lasterror = "Directory not supplied";
	Line(get_last_error());
	return 0;
}


################################################################
# Get the last error that occurred
################################################################
sub get_last_error
{
	# add prefix if not one already	
	if (!(substr($lasterror, 0, 8) =~ "ERROR : "))
	{
		$lasterror = "ERROR : $lasterror";
	}
	
	return $lasterror;
}


################################################################
# LOCAL ENTRYPOINTS
# Apply to individual connection
################################################################

################################################################
# Connect to the target device
################################################################
sub connect_to_target
{
	Line("Connecting to $statdir/$dllname...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $connection_type = $cmdref->[0];
	my $codetype = $cmdref->[1];

	# if we haven't supplied a connection type through here, maybe it was defined during initialise()
	if (!$connection_type)
	{
		$connection_type = $def_connection_type;
	}

	# if we haven't supplied a code type through here, maybe it was defined during initialise()
	if (!$codetype)
	{
		$codetype = $def_codetype;
	}
	
	if ($connection_type)
	{
		if ($codetype)
		{
			$FUNCTION = new Win32::API("$statdir/$dllname", "StdConnect", [I, P, P, P], I);
			if (defined($FUNCTION))
			{
				Line("Connecting ($connection_type, $codetype)...");
				my $hndl = $FUNCTION->Call($connection_type, $codetype, 0, 0);
				if ($hndl)
				{
					return $hndl;
				}
				else
				{
					GetDllError(0);
				}
			}
			else
			{
				$lasterror = "Function StdConnect not found";
			}
		}
		else
		{
			$lasterror = "Code type not supplied";
		}
	}
	else
	{
		$lasterror = "Connection type not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Connect to the target device in a multithreaded environment
################################################################
sub connect_to_target_multithreaded
{
	Line("Connecting to $statdir/$dllname (" . get_version() . ")...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $connection_type = $cmdref->[0];
	my $codetype = $cmdref->[1];

	# if we haven't supplied a connection type through here, maybe it was defined during initialise()
	if (!$connection_type)
	{
		$connection_type = $def_connection_type;
	}

	# if we haven't supplied a code type through here, maybe it was defined during initialise()
	if (!$codetype)
	{
		$codetype = $def_codetype;
	}
	
	if ($connection_type)
	{
		if ($codetype)
		{
			$FUNCTION = new Win32::API("$statdir/$dllname", "StdConnectMT", [I, P], I);
			if (defined($FUNCTION))
			{
				Line("Connecting ($connection_type, $codetype)...");
				my $hndl = $FUNCTION->Call($connection_type, $codetype);
				if ($hndl)
				{
					return $hndl;
				}
				else
				{
					GetDllError(0);
				}
			}
			else
			{
				$lasterror = "Function StdConnect not found";
			}
		}
		else
		{
			$lasterror = "Code type not supplied";
		}
	}
	else
	{
		$lasterror = "Connection type not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Disconnect from the target device
################################################################
sub disconnect_from_target
{
	Line("Disconnecting from $statdir/$dllname...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];

	if ($hndl)
	{
		$FUNCTION = new Win32::API("$statdir/$dllname", "StdDisconnect", [I], I);
		if (defined($FUNCTION))
		{
			Line("Disconnecting $statdir/$dllname ($hndl)...");
			$ret = $FUNCTION->Call($hndl);
			if ($ret)
			{
				return 1;
			}
			else
			{
				GetDllError($hndl);
			}
		}
		else
		{
			$lasterror = "Function StdDisconnect not found";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Set logging of stat commands
# param0: logfile
# param1: log level
# param2: append or overwrite
# param3: reserved
# param4: reserved
################################################################
sub set_command_logging
{
	Line("Setting command logging...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	if (($loglevel > 1) and $logfile)
	{
		if ($hndl)
		{
			if ($logfile)
			{
				$FUNCTION = new Win32::API("$statdir/$dllname", "StdSetCommandLogging", [I, P, P, I, I, P, P], I);
				if (defined($FUNCTION))
				{
					Line("Logging to ($hndl, $logfile, $actual_level)");
					$ret = $FUNCTION->Call($hndl, $logfile, 0, $loglevel, 0, 0, 0);
					if ($ret)
					{
						return 1;
					}
					else
					{
						GetDllError($hndl);
					}
				}
				else
				{
					$lasterror = "Function StdSetCommandLogging not found\n";
				}
			}
			else
			{
				$lasterror = "Logfile not supplied";
			}
		}
		else
		{
			$lasterror = "Handle not supplied";
		}
	}
	else
	{
		$lasterror = "Logging level incorrect or logfile not supplied";
	}
	
	Line(get_last_error());
	return 0;
}


################################################################
# Set command delay
# param0: delay (in milliseconds).  Range 100 (default) -> 30,000
# The delay is inserted between complete commands, not between
# individual keystrokes.
################################################################
sub set_command_delay
{
	Line("Setting command delay...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $delay = $cmdref->[1];
	if ($hndl)
	{
		if ($delay)
		{
			$FUNCTION = new Win32::API("$statdir/$dllname", "StdSetCommandDelay", [I, I], I);
			if (defined($FUNCTION))
			{
				Line("Delay ($hndl, $delay)");
				$ret = $FUNCTION->Call($hndl, $delay);
				if ($ret)
				{
					return 1;
				}
				else
				{
					GetDllError($hndl);
				}
			}
			else
			{
				$lasterror = "Function StdSetCommandDelay not found\n";
			}
		}
		else
		{
			$lasterror = "Delay not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Set image verification
# param0: Ref Dir (string) - the directory where the reference 
# images are located
# param1: Remove old images (bool)
# param2: Fudge factor - what percentage (0 -> 100%) of mismatched 
# pixels are still considered to be a match
################################################################
sub set_image_verification
{
	Line("Setting image verification...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $refdir = $cmdref->[1];
	my $removeImages = $cmdref->[2];
	my $fudgeFactor = $cmdref->[3];
	if ($hndl)
	{
		if ($refdir)
		{
			$FUNCTION = new Win32::API("$statdir/$dllname", "StdSetImageVerification", [I, P, I, I], I);
			if (defined($FUNCTION))
			{
				Line("Reference Directory ($hndl, $refdir, $removeImages, $fudgeFactor)");
				$ret = $FUNCTION->Call($hndl, $refdir, $removeImages, $fudgeFactor);
				if ($ret)
				{
					return 1;
				}
				else
				{
					GetDllError($hndl);
				}
			}
			else
			{
				$lasterror = "Function StdSetImageVerification not found\n";
			}
		}
		else
		{
			$lasterror = "Reference Image Directory not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Get a target screenshot
################################################################
sub get_screenshot
{
	Line("Getting screenshot...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $name = $cmdref->[1];
	my $command;
	if ($hndl)
	{
		if ($name)
		{
			if ($imagedir)
			{
				$command = "<B><#image $imagedir/><S$name><E>";
			}
			else
			{
				$command = "<B><S$name><E>";
			}
		}	
		else
		{
			if ($imagedir)
			{
				$command = "<B><#image $imagedir/><S><E>";
			}
			else
			{
				$command = "<B><S><E>";
			}
		}

		if (SendCommand($hndl, $command))
		{
			return 1;
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Get Device Information
################################################################
sub get_device_info
{
	Line("Getting device info...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];

	if (SendCommand($hndl, "<B><D><E>"))
	{
		return 1;
	}

	Line(get_last_error());
	return 0;
}


################################################################
# 'Touch' the screen at specified co-ordinates
################################################################
sub touch_screen
{
	Line("Touching screen...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $xx = $cmdref->[1];
	my $yy = $cmdref->[2];
	if ($hndl)
	{
		if ($xx and $yy)
		{
			if (SendCommand($hndl, "<B><I$xx,$yy><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "X-Y co-ordinates not specified";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Start an application and create a new file
################################################################
sub start_application
{
	Line("Starting application...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $name = $cmdref->[1];
	my $doc = $cmdref->[2];
	if ($hndl)
	{
		if ($name)
		{
			my $command;
			if ($doc)
			{
				$command = "<B><A$name,$doc><E>";
			}
			else
			{
				$command = "<B><A$name><E>";
			}
	
			if (SendCommand($hndl, $command))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Application name not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Start a program with parameters
################################################################
sub start_program
{
	Line("Starting program...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $name = $cmdref->[1];
	my $param = $cmdref->[2];
	if ($hndl)
	{
		if ($name)
		{
			my $command;
			if ($param)
			{
				$command = "<B><J$name,$param><E>";
			}
			else
			{
				$command = "<B><J$name><E>";
			}
	
			if (SendCommand($hndl, $command))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Program name not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Start an application and open an existing new file
################################################################
sub open_application
{
	Line("Opening application...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $name = $cmdref->[1];
	my $doc = $cmdref->[2];
	if ($hndl)
	{
		if ($name and $doc)
		{
			if (SendCommand($hndl, "<B><F$name,$doc><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Application name and file not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Close the current application
################################################################
sub close_application
{
	Line("Closing application...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	if ($hndl)
	{
		if (SendCommand($hndl, "<B><C><E>"))
		{
			return 1;
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}
	
	Line(get_last_error());
	return 0;
}


################################################################
# Send a sequence of basic keystrokes
################################################################
sub basic_key_sequence
{
	Line("Sending basic key sequence...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $sequence = $cmdref->[1];
	if ($hndl)
	{
		if ($sequence)
		{
			if (SendCommand($hndl, "<B><K$sequence><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Key sequence not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Send a control keystroke
################################################################
sub control_keystroke
{
	Line("Sending control keystroke...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $keystroke = $cmdref->[1];
	if ($hndl)
	{
		if ($keystroke)
		{
			if (SendCommand($hndl, "<B><L$keystroke><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Control keystroke not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Send a combination keystroke
################################################################
sub combination_keystroke
{
	Line("Sending combination keystroke...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $combination = $cmdref->[1];
	if ($hndl)
	{
		if ($combination)
		{
			if (SendCommand($hndl, "<B><M$combination><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Combination keystroke not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Pause for milliseconds
################################################################
sub pause
{
	Line("Pausing...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $pause = $cmdref->[1];
	if ($hndl)
	{
		if ($pause)
		{
			if (SendCommand($hndl, "<B><P$pause><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Value not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Start ESHELL command interpreter
################################################################
sub start_eshell
{
	Line("Starting ESHELL...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	if ($hndl)
	{
		if (SendCommand($hndl, "<B><!><E>"))
		{
			return 1;
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Stop ESHELL command interpreter
################################################################
sub stop_eshell
{
	Line("Stopping ESHELL...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	if ($hndl)
	{
		if (SendCommand($hndl, "<B><!!><E>"))
		{
			return 1;
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Transfer (copy) common test file to target.  Copies a 'common'
# test file to the target board.
################################################################
sub copy_commonfile_to_target
{
	Line("Copying commonfile file to target...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $source = $cmdref->[1];
	my $dest = $cmdref->[2];
	if ($hndl)
	{
		if ($source and $dest)
		{
			if (SendCommand($hndl, "<B><T$commondir$source,$dest><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Source and/or target names not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Transfer (copy) test file to target.  Copies a specific test 
# file to the target board.
################################################################
sub copy_testfile_to_target
{
	Line("Copying testfile file to target...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $source = $cmdref->[1];
	my $dest = $cmdref->[2];
	if ($hndl)
	{
		if ($source and $dest)
		{
			if (SendCommand($hndl, "<B><T$senddir$source,$dest><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Source and/or target names not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Transfer (copy) build file to target.  Copies a file located
# within a build image to the target board
################################################################
sub copy_buildfile_to_target
{
	Line("Copying buildfile file to target...");
	
	if (!$httpserver or !$runno or !builddir)
	{
		Line("ERROR: TMS Server, run number, or build directory not defined - Build files cannot be transfered");
		return 0;
	}

	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $source = $cmdref->[1];
	my $dest = $cmdref->[2];
	my $codetype = $cmdref->[2];
	if ($hndl)
	{
		if ($source and $dest)
		{
			# find the location of the build
			$location = GetBuildLocation($codetype);
			if ($location)
			{		
				# get the desired file from the TMS
				if (DownloadAndSaveFile("$location/$source", "$workdir$builddir$source"))
				{			
					# send the file	to the target device
					if (SendCommand($hndl, "<B><T$builddir$source,$dest><E>"))
					{
						return 1;
					}
				}				
				else
				{
					$lasterror = "Could not download file $location/$file";
				}
			}
			else
			{
				$lasterror = "Build location could not be obtained";
			}
		}
		else
		{
			$lasterror = "Source and/or target names not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Retrieve (move) file from target
################################################################
sub move_from_target
{
	Line("Moving file from target...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $source = $cmdref->[1];
	if ($hndl)
	{
		if ($source)
		{
			if (SendCommand($hndl, "<B><X$source,$recvdir><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Source name not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Retrieve (copy) file from target
################################################################
sub copy_from_target
{
	Line("Copying file from target...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $source = $cmdref->[1];
	if ($hndl)
	{
		if ($source)
		{
			if (SendCommand($hndl, "<B><R$source,$recvdir><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Source name not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Delete unneeded file from target
################################################################
sub remove_file
{
	Line("Removing file...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $file = $cmdref->[1];
	
	if ($hndl)
	{
		if ($file)
		{
			if (SendCommand($hndl, "<B><U$file><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "File not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Creating a folder on the target
################################################################
sub create_folder
{
	Line("Creating folder...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $folder = $cmdref->[1];
	
	if ($hndl)
	{
		if ($folder)
		{
			if (SendCommand($hndl, "<B><N$folder><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Folder not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Delete unneeded folder from target
################################################################
sub remove_folder
{
	Line("Removing folder...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $folder = $cmdref->[1];
	
	if ($hndl)
	{
		if ($folder)
		{
			if (SendCommand($hndl, "<B><Z$folder><E>"))
			{
				return 1;
			}
		}
		else
		{
			$lasterror = "Folder not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Hold a key down for a specified time
################################################################
sub hold_key
{
	Line("Holding key down for specified time...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $key = $cmdref->[1];
	my $delaytime = $cmdref->[2];
	
	if ($hndl)
	{
		if ($key)
		{
			if ($delaytime)
			{
				if (SendCommand($hndl, "<B><H$key,$delaytime><E>"))
				{
					return 1;
				}
			}
			else
			{
				$lasterror = "Delay time not supplied";
			}
		}
		else
		{
			$lasterror = "Key not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Execute a command file
################################################################
sub send_command_file
{
	Line("Sending command file...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $file = $cmdref->[1];
	if ($hndl)
	{
		if ($file)
		{
			$FUNCTION = new Win32::API("$statdir/$dllname", "StdSendCommandFile", [I, P, P], I);
			if (defined($FUNCTION))
			{
				Line("Command File ($hndl, $file )");
				$ret = $FUNCTION->Call($hndl, $file, 0);
				if ($ret)
				{
					return 1;
				}
				else
				{
					GetDllError($hndl);
				}
			}
			else
			{
				$lasterror = "Function StdSendCommandFile not found\n";
			}
		}
		else
		{
			$lasterror = "Command file not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# Execute a raw command
################################################################
sub send_raw_command
{
	Line("Executing raw command...");
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $command = $cmdref->[1];
	if ($hndl)
	{
		if ($command)
		{
			$FUNCTION = new Win32::API("$statdir/$dllname", "StdSendRawCommand", [I, P, P], I);
			if (defined($FUNCTION))
			{
				Line("Command File ($hndl, $command)");
				$ret = $FUNCTION->Call($hndl, $command, 0);
				if ($ret)
				{
					return 1;
				}
				else
				{
					GetDllError($hndl);
				}
			}
			else
			{
				$lasterror = "Function StdSendRawCommand not found\n";
			}
		}
		else
		{
			$lasterror = "Command file not supplied";
		}
	}
	else
	{
		$lasterror = "Handle not supplied";
	}

	Line(get_last_error());
	return 0;
}


################################################################
# PRIVATE FUNCTIONS
################################################################

#############################################################################
# Get the STAT working path from the registry
#############################################################################
sub GetSTATWorkingDirectory
{
	my $swkey = $Registry->{"LMachine/software/"};
	my $endkey = $swkey->{"symbian/stat/"};
	
	$workdir = $endkey->{"/WorkingPath"};
	if (!$workdir)
	{
		$lasterror = "Cannot obtain STAT work dir from registry";
		return 0;
	}

	$statdir = $endkey->{"/InstallRoot"};
	if (!$statdir)
	{
		$lasterror = "Cannot obtain STAT Install dir from registry";
		return 0;
	}

	# substitute DOS '\' with '/' if needed
	$workdir = "$workdir/";
	$workdir =~ s/\\/\//g;
	$statdir =~ s/\\/\//g;

	return 1;
}


################################################################
# Get the error that occurred within the DLL call
################################################################
sub GetDllError
{
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];

	$FUNCTION = new Win32::API("$statdir/$dllname", "StdGetError", [I], P);
	if (defined($FUNCTION))
	{
		if ($hndl)
		{
			Line("Handle : ($hndl)");
		}

		$lasterror = $FUNCTION->Call($hndl);
	}
	else
	{
		$lasterror = "Function StdGetError not found";
	}
	
	return $lasterror;
}


#############################################################################
# Display a line of text
#############################################################################
sub Line
{
	my ($msg) = @_;

	if ($msg)
	{
		($hour, $min, $sec) = (localtime)[2,1,0];
		printf("%02d:%02d:%02d $msg\n", $hour, $min, $sec);
		STDOUT->autoflush;
		STDERR->autoflush;
	}
}


###############################################################################################
# Construct a build location depending on the code and build type
###############################################################################################
sub GetBuildLocation
{
	my $command;
	
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $codetype = $cmdref->[0];

	if (!$httpserver)
	{
		$lasterror = "TMS Server not defined";
		Line(get_last_error());
		return 0;
	}

	# ceate a user agent object
	my $ua = new LWP::UserAgent;
	$ua->agent("AgentName/0.1 " . $ua->agent);
	
	$req = new HTTP::Request GET => "$httpserver/GetBuildLocation?RunNo=$runno";
	$req->header('User-Agent' => $ua->agent);
	$req->header('Authorization' => "Basic $encoded");
	$req->header('Content-type' => 'binary');
	$res = $ua->request($req);
	if (!$res->is_success)
	{
		$lasterror = "Cannot open $httpserver/GetBuildLocation";
		Line(get_last_error());
		return 0;
	}

	# decide build type depending on $codetype
	my $buildtype = "UREL";
	if ($codetype eq "WINS")
	{
		$buildtype = "UDEB";
	}

	$command = "$httpserver/" . $res->content() . "/epoc32/release/$codetype/$buildtype";

	# remove any carriage returns in the string
	$command =~ s/\n//g;
	
	Line("Build location is [$command]");

	return $command;
}


###############################################################################################
# Download a single file to a local location
###############################################################################################
sub DownloadAndSaveFile
{
	my ($source, $destination) = @_;

	# substitute DOS '\' with '/' if needed
	$source =~ s/\\/\//g;
	$destination =~ s/\\/\//g;

	Line("$source => $destination");

	# ceate a user agent object
	my $ua = new LWP::UserAgent;
	$ua->agent("AgentName/0.1 " . $ua->agent);
	
	$req = new HTTP::Request GET => "$source";
	$req->header('User-Agent' => $ua->agent);
	$req->header('Authorization' => "Basic $encoded");
	$req->header('Content-type' => 'binary');
	$res = $ua->request($req);
	if (!$res->is_success)
	{
		$lasterror = "Cannot open $source";
		Line(get_last_error());
		return 0;
	}

	Line("Saving retrieved file to $destination...");

	# save the retrieved contents to disk
	my $fh = IO::File->new ("$destination", O_CREAT | O_WRONLY | O_TRUNC);
	binmode($fh);
	if ($fh)
	{
		print $fh $res->content();
		$fh->autoflush(1);
		close($fh);
	}
	else
	{
		$lasterror = "Cannot create $destination";
		Line(get_last_error());
		return 0;
	}

	return 1;
}


################################################################
# Send a command to the target
################################################################
sub SendCommand
{
	my @cmdline = @_;
	my $cmdref = \@cmdline;
	my $hndl = $cmdref->[0];
	my $command = $cmdref->[1];

	# substitute Perl '/' with DOS '\' if needed
	$command =~ s/\//\\/g;

	$FUNCTION = new Win32::API("$statdir/$dllname", "StdSendRawCommand", [I, P, P], I);
	if (defined($FUNCTION))
	{
		Line("Command ($hndl, $command)");
		$ret = $FUNCTION->Call($hndl, $command, 0);
		if ($ret)
		{
			return 1;
		}
		else
		{
			GetDllError($hndl);
		}
	}
	else
	{
		$lasterror = "Function StdSendRawCommand not found";
	}

	return 0;
}


# last line in file
1;
__END__
