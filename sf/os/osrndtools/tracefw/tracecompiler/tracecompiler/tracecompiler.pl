#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Finds MMP paths from component makefiles and forwards them to MMP parser
#
#!perl -w
use FindBin;
my $trace_compiler_path;

BEGIN
{
  # Get current directory as TraceCompiler path
  $trace_compiler_path = $FindBin::Bin; # e.g. X:/epoc32/tools
  $trace_compiler_path =~ s/\\/\//g; # Replace all "\" with "/"
  $trace_compiler_path =~ s/\/$//; # Remove possible trailing slash
}

use lib $trace_compiler_path;

use strict;
use env;
use Cwd;
use tracecompiler;
use tracecompiler_mmp_data;

# EPOC root is two levels down from TraceCompiler level
my $epocroot = tracecompiler::concatenatePath($trace_compiler_path, "../../");
$epocroot =~ s/\/$//; # Remove possible trailing slash
my $drive = substr($epocroot, 0, 2);
my $build_target;
my $project_name;
my $action = "RUN";

# Get Java command or EXIT if not found
my $java_command = tracecompiler::getJavaCommand();

if (not defined $java_command)
{
  print STDERR "Java 1.5 or newer required!\n";
  exit;
}

#-------------------------------------------------------
# Go through command line arguments
#-------------------------------------------------------
while ( @ARGV )
{
  my $arg = shift(@ARGV);
  
  # Print version information
  if ($arg eq "-v")
  {
    my $version = tracecompiler::getTraceCompilerVersion();
    print $version;
    exit;
  }
  # Clean TraceCompiler generated files
  elsif ($arg eq "-clean")
  {
    $action = "CLEAN";
  }
  # Finalize TraceCompiler running
  elsif ($arg eq "-final")
  {
    $action = "FINAL";
  }
  # Build target (e.g. armv5)
  elsif (not defined $build_target)
  {
    $build_target = $arg;
  }
  # Project name (e.g. MyComponent)
  else
  {
    $project_name = $arg; 
  }
}

#-------------------------------------------------------
# Build target must now be defined or print usage info
#-------------------------------------------------------
if (not defined $build_target)
{
  my $version = tracecompiler::getTraceCompilerVersion();
  print $version;
  print "\nUsage: tracecompiler.pl build_target in the folder containing bld.inf. bldmake bldfiles must be run before this.\n";
  print "Example: tracecompiler.pl armv5\n";
  exit;
}

my $return_string = "";

#-------------------------------------------------------
# Find component makefile path from abld.bat
#-------------------------------------------------------
tracecompiler::debugTraceCompilerVersion();
tracecompiler::debugMsg("Find component makefile path from abld.bat");
open(ABLD, "<abld.bat") or die $!;
my $makefile;
while (<ABLD>)
{
  if (/ABLD.PL/)
  {
    my $makefilepath = $_;
    $makefilepath =~ s/perl -S ABLD.PL "(.*)\\" .*$/$1/;
    chop($makefilepath);
    
    # Change all "\" characters to "/"
    $makefilepath =~ s/\\/\//g;
    
    $makefilepath = "$epocroot/epoc32/build".$makefilepath;
    $makefile = $makefilepath.$build_target.".MAKE";
  }
}
close ABLD or die $!;

#-------------------------------------------------------
# Find project makefile paths from component makefile
#-------------------------------------------------------
my @projectmakefiles;
my $mmps;

tracecompiler::debugMsg("Find project makefile paths from component makefile $makefile");

open(MAKEFILE, "<$makefile") or (print STDERR "Cannot open makefile $makefile\n" and exit);
while (<MAKEFILE>)
{
  my $line = $_;
    
  if ($line =~ /\"([^\"]+?)\"\s+\"PRJ_.*MMPFILES\s+MMP\"\s+\"([^\"]+?)\"/i)
  {
    my $bldinf = $1;
    my $mmp = $2;
    
    tracecompiler::debugMsg("Found BLD file: $bldinf");
    
    # Remove bld.inf from the end of the path
    $bldinf =~ s/(.*)[\\\/].*/$1\//;
    
    my $mmp_file = tracecompiler::concatenatePath($bldinf, $mmp);
        
    # Change all "//" to "/"
    $mmp_file =~ s/\/\//\//g;
    
    # If .mmp is missing from the end, add it
    if ($mmp_file !~ /\.mmp$/i)
    {
      $mmp_file .= ".mmp";
    }
        
    tracecompiler::debugMsg("Found MMP file: $drive$mmp_file\n");
    
    # If project name is defined, only add that MMP to the list of mmp files
    if (defined $project_name)
    {
        if ($mmp_file =~ /($project_name.mmp)$/i)
        {
          $mmps .= $drive . $mmp_file . " ";
        }       
    }
    else
    {
      $mmps .= $drive . $mmp_file . " ";  
    }
  }

}
close MAKEFILE or die $!;

#-------------------------------------------------------
# Call the MMP parser for each MMP (or clean / final)
#-------------------------------------------------------
if (defined $mmps)
{
  foreach my $mmp (split(" ", $mmps))
  {
    # Pass MMP file path to Parse MMP
    if ($action eq "RUN")
    {
      my $command = "perl $trace_compiler_path/tracecompiler_parse_mmp.pl $mmp";
      tracecompiler::debugMsg("Call the MMP parser. Command: $command\n");
      print qx($command);
    }
    
    # Clean files using this MMP
    elsif ($action eq "CLEAN")
    {
    	my $mmpObject = tracecompiler_mmp_data->new($mmp);
      cleanTraceCompilerFiles($mmpObject);
    }
    
    # Finalize files using this MMP
    elsif ($action eq "FINAL")
    {
    	my $mmpObject = tracecompiler_mmp_data->new($mmp);
      finalizeTraceCompilerRun($mmpObject);
    }
  }
}
else
{
  tracecompiler::debugMsg("Could not find any MMP files!");
}





#-------------------------------------------------------#
#-------------------------------------------------------#
#--------------------SUBROUTINES------------------------#
#-------------------------------------------------------#
#-------------------------------------------------------#

#-------------------------------------------------------
# Cleans TraceCompiler generated files
#-------------------------------------------------------
sub cleanTraceCompilerFiles
{
  tracecompiler::debugMsg("Clean TraceCompiler Files");
  my $mmpObj = $_[0];
  if($mmpObj->{tracespaths}) {
  	foreach my $tracesFolder (@{$mmpObj->{tracespaths}})
  	{
    	if (-e $tracesFolder and opendir(DIR, $tracesFolder))
    	{
      	while (defined(my $file = readdir(DIR)))
      	{
        	# Delete files ending with *Traces.h
        	if ($file =~ /Traces\.h$/i)
        	{
          	print "Cleaning: " . $tracesFolder . "/" . $file . "\n";
          	unlink $tracesFolder . "/" . $file || print $!;
        	}
        	# Delete cache files
        	elsif ($file =~ /\.cache$/i)
        	{
          	print "Cleaning: " . $tracesFolder . "/" . $file . "\n";
          	unlink $tracesFolder . "/" . $file || print $!;
        	}
      	}
      	closedir(DIR);
    	}
 	 }
 	}
  
  tracecompiler::debugMsg("Cleaning TraceCompiler Files ready\n");
  
  # Finalize TraceCompiler run
  finalizeTraceCompilerRun($mmpObj);
}

#-------------------------------------------------------
# Finalizes TraceCompiler run
#-------------------------------------------------------
sub finalizeTraceCompilerRun
{
	tracecompiler::debugMsg("Finalize TraceCompiler Run");
	my $mmpObj = $_[0];
	if($mmpObj->{tracespaths}) {
	foreach my $tracesFolder (@{$mmpObj->{tracespaths}})
	{
		my $defFile = $tracesFolder . "/" . "OstTraceDefinitions.h";
		# Open OstTraceDefinitions.h and read stuff to array
		if (-e $defFile and open(FILE,"<$defFile"))
		{
			my @lines = <FILE>;
			close(FILE);

			# Write stuff back to OstTraceDefinitions.h
			open(FILE,">$defFile") || exit;
			foreach my $line (@lines)
			{
				# Comment out the define
				$line =~ s/(.*)#define OST_TRACE_COMPILER_IN_USE/\/\/ #define OST_TRACE_COMPILER_IN_USE/;
				print FILE $line;
			}
			close(FILE);
		}
	}
}

tracecompiler::debugMsg("Finalizing TraceCompiler Run ready\n");
}
