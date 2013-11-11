#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Module for TraceCompiler
#
package tracecompiler;

# If this is 1, debug prints are shown
my $DEBUG = 0;

use FindBin;

# Get current directory as TraceCompiler path
my $trace_compiler_path = $FindBin::Bin;  # e.g. X:/epoc32/tools
$trace_compiler_path =~ s/\\/\//g; # Replace all "\" with "/"
$trace_compiler_path =~ s/\/$//; # Remove possible trailing slash

#-------------------------------------------------------
# Concatenate path
#-------------------------------------------------------
sub concatenatePath
{
  my $concatenatePathBase = $_[0];
  my $concatenatePathFile = $_[1];
  
  my $backCount = 0;
  
  # Change all "\" characters to "/"
  $concatenatePathBase =~ s/\\/\//g;
  $concatenatePathFile =~ s/\\/\//g;
  
  # Replace all "/./" with "/"
  $concatenatePathBase =~ s/\/\.\//\//g;
  $concatenatePathFile =~ s/\/\.\//\//g;
    
  # Find how many back references there are and remove them
  while ($concatenatePathFile =~ /\.\.\//g) 
  { 
    $backCount++ 
  }
  $concatenatePathFile =~ s/\.\.\///g;
  
  # If there is / in the end of the base remove it
  $concatenatePathBase =~ s/\/$//;
  
  # Remove directories from the end of the path
  $concatenatePathBase = reverse($concatenatePathBase);
  for (my $i=0; $i<$backCount; $i++)
  {
    $concatenatePathBase =~ s/.*?\///;
  }
  $concatenatePathBase = reverse($concatenatePathBase);
  
  my $concatenatePathFullFilePath = "$concatenatePathBase\/$concatenatePathFile";
  
  # Replace again all "/./" with "/"
  $concatenatePathFullFilePath =~ s/\/\.\//\//g;
  
  debugMsg("Concatenate returns $concatenatePathFullFilePath");
  return $concatenatePathFullFilePath;
}

#-------------------------------------------------------
# Get java command
#-------------------------------------------------------
sub getJavaCommand
{
  my @java_commands = ("/tools/ncp_tools/helium/external/jdk1.5/jre/bin/java.exe", "java.exe", "c:/apps/seeinstaller/jre/bin/java.exe");

  my $java_command;
  
  foreach my $command (@java_commands)
  {
    if (checkJava($command))
    {
      $java_command = $command;
      last;
    }
  }
  
  return $java_command;
}

#-------------------------------------------------------
# Check java
#-------------------------------------------------------
sub checkJava
{
  open (IN, "$_[0] -version 2>&1 |");
  while(<IN>)
  {
    if (/(\d+\.\d+)/)
    {
      if ($1 >= 1.5)
      {
        return 1;
      }
    } 
  }
  
  return 0;
}

#-------------------------------------------------------
# Gets TraceCompiler version number
#-------------------------------------------------------
sub getTraceCompilerVersion
{
  # Get Java command or EXIT if not found
  my $java_command = getJavaCommand();

  if (not defined $java_command)
  {
    return "Java 1.5 or newer required!\n";
    exit;
  }
  
   # run from class files class files
  my $version_query_command = "$java_command -classpath $trace_compiler_path/tracecompiler com.nokia.tracecompiler.TraceCompilerMain -v";
  
  # run from jar  file	command
  #my $version_query_command = "$java_command -jar $trace_compiler_path/tracecompiler.jar -version"; 

  my $version = qx($version_query_command);
    
  return $version;
}


#-------------------------------------------------------
# Debug Message. Writes to log file and outputs to screen.
#-------------------------------------------------------
sub debugMsg
{
  if ($DEBUG)
  {
    open FILE, ">>/tracecompiler_debug.txt" or die $!;
    print FILE $_[0] . "\n";
    close FILE;
    
    return print "DEBUG: $_[0]\n";
  }
}

#-------------------------------------------------------
# Writes TraceCompiler version to log and screen if Debug is on.
#-------------------------------------------------------
sub debugTraceCompilerVersion
{
  if ($DEBUG)
  {   
    open FILE, ">>/tracecompiler_debug.txt" or die $!;
    my $version = getTraceCompilerVersion();
    print FILE $version;
    close FILE;
    
    return print "DEBUG: $version\n";
  }
}

1;

