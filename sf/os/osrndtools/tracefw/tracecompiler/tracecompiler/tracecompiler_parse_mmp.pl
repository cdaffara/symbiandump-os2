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
# Parses UID and source files from given MMP files. Starts TraceCompiler Java application.
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
use warnings;
use env;
use FileHandle;
use IPC::Open3;
use tracecompiler;
use tracecompiler_mmp_data;

my $java_command = tracecompiler::getJavaCommand();

if (not defined $java_command)
{
  print STDERR "Java 1.5 or newer required!\n";
  exit;
}

# Global variables
my $project_name;
my $traces_folder = "";


# run from class files

my $command = "$java_command -classpath $trace_compiler_path/tracecompiler com.nokia.tracecompiler.TraceCompilerMain";

# run from jar file
#my $command = "$java_command  -jar $trace_compiler_path/tracecompiler/tracecompiler.jar";

# Open MMP file
foreach my $mmp_file (@ARGV)
{
  $traces_folder = "";
  $project_name = $mmp_file;
  
  # Take the module name from the MMP file path. e.g. "X:/temp/mycomponent.mmp" would create "mycomponent"
  $project_name =~ s/.*\/([^\.]*).*/$1/;
  
  tracecompiler::debugMsg("Module name: $project_name");
  
  # Parse sources in an own function
  my $mmpObject = tracecompiler_mmp_data->new($mmp_file);
    
  # There must be UID
  if (!$mmpObject->{uid})
  {
    tracecompiler::debugMsg("UID is not defined, don't call TraceCompiler!"); 
    exit;
  }
  if (!$mmpObject->{target})
  {
  	tracecompiler::debugMsg("Target not defined, don't call TraceCompiler!"); 
  	exit;
  }
  if (!$mmpObject->{type})
  {
  	tracecompiler::debugMsg("Target type not defined, don't call TraceCompiler!"); 
  	exit;
  }
  if (!$mmpObject->{ext})
  {
  	tracecompiler::debugMsg("Target extension not defined, don't call TraceCompiler!"); 
  	exit;
  }
  
  # find out the right traces folder to use if ther is more than one and set the project name accordingly.
  # the following order of the code is very important.
  my $tmp;
  if ($mmpObject->{tracespaths})
  {
  	#Check if there is one of the expected format
  	
  	#search for /trace/<target>_<ext>
  	$tmp = $mmpObject->{target} ."_" . $mmpObject->{ext};
  	my @list = grep(/.*\/traces\/$tmp\s*$/i, @{$mmpObject->{tracespaths}});
  	
  	if (scalar @list > 0) 
  	{
  		$traces_folder = pop(@list);
  		$project_name = $mmpObject->{target} ."_" . $mmpObject->{ext};
  		tracecompiler::debugMsg("Found traces folder 1: $traces_folder" ); 
  	}
  	 
  	if ($traces_folder eq "" ) 
  	{
  		#search for /traces_<target>_<type>
  		$tmp = $mmpObject->{target} ."_" . $mmpObject->{type};
  		@list = grep(/.*\/traces_$tmp\s*$/i, @{$mmpObject->{tracespaths}});
  		if (scalar @list > 0) 
  		{
  			$traces_folder = pop(@list);
  			$project_name = $mmpObject->{target} . "_" . $mmpObject->{type};
  			tracecompiler::debugMsg("Found traces folder 2: $traces_folder" ); 
  		}
  	}
  	
  	if ($traces_folder eq "" ) 
  	{
  		#search for /traces_<mmpname>
  		@list = grep(/.*\/traces_$project_name\s*$/i, @{$mmpObject->{tracespaths}});
  		if (scalar @list > 0) 
  		{
  			$traces_folder = pop(@list);
  			tracecompiler::debugMsg("Found traces folder 3: $traces_folder" ); 
  		}
  	}
  		
  	if ($traces_folder eq "" ) 
  	{
  		#search for /traces
  		@list = grep(/.*\/traces\s*$/i, @{$mmpObject->{tracespaths}});
  		if (scalar @list > 0) 
  		{
  			$traces_folder = pop(@list);
  			tracecompiler::debugMsg("Found traces folder 4: $traces_folder" ); 
  		}
  	}
  }
  else 
  {
  	tracecompiler::debugMsg("No Traces folder was found in userinclude, don't call TraceCompiler!"); 
  	exit;
  }
  
  if (!$mmpObject->{sources})
  {
  	tracecompiler::debugMsg("No sources found!"); 
  	exit; 	
  }
 
 
  # IMPORTANT NOTE:
  # please note that IPC::open3() is the only suitable api available in the perl version 5.6.1 used in ONB
  # but it has a limit. If the size of the command is more than 264, perl will crash and it seems to be a windows limit
  # rather perl.
  # Therefore we have to parse the mmp and the traces folder from the stdin (*WRITER in this case) and 
  # relax the new TraceCompiler API to allow this if they are not specified on the command line.
  # It's not an issue in Raptor as it calls TraceCompiler directly.
  #
  $command .= " --uid=" . $mmpObject->{uid} . " --project=" . $project_name;
           
  tracecompiler::debugMsg("\nCall the Java TraceCompiler. Command: $command\n");
    
  # Create TraceCompiler process
  local (*READER, *WRITER);
  my $ pid = open3(\*WRITER, \*READER, \*READER, $command);
    
  # Remove duplicates from the sources list
  my %seen = ();
  my @uniqueSources = ();
  foreach my $item (@{$mmpObject->{sources}}) 
  {
    push(@uniqueSources, $item) unless $seen{$item}++;
  }
 
  tracecompiler::debugMsg("Send mmp file: $mmp_file");
  print WRITER "--mmp=$mmp_file\n";
  
  tracecompiler::debugMsg("Send traces folder path: $traces_folder");
  print WRITER "--traces=$traces_folder\n";
    
    
  # Send sources to the TraceCompiler     
  foreach my $source (@uniqueSources)
  {
    tracecompiler::debugMsg("Send source: $source");
    print WRITER "$source\n";
   }
    
  # Send the end of the source files tag
  print WRITER "*ENDOFSOURCEFILES*\n";
  WRITER->autoflush(1);
    
  # Gather up the response from the TraceCompiler
  my $compilerReturn = "";
  foreach my $line (<READER>)
  {
    tracecompiler::debugMsg("Response line from TraceCompiler: $line");
    $compilerReturn .= $line;
  }       

  # If Compiler doesn't return anything or we get no class found error from Java, don't print anything
  if ($compilerReturn ne "" and $compilerReturn !~ /NoClassDefFoundError/)
  {
    tracecompiler::debugMsg("TraceCompiler succeeded! Returned: $compilerReturn");
    print "\n******************* TRACECOMPILER STARTS *******************\n\n";
    print "Building traces for component $project_name. Component UID: 0x$mmpObject->{uid}.\n";
    print $compilerReturn . "\n";
    print "\n******************* TRACECOMPILER ENDS *********************\n\n";
  }
  else 
  {
    tracecompiler::debugMsg("No traces were generated. Returned:\n$compilerReturn\n");
  }
 }
