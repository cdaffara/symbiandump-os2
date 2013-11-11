#
# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This scripts scans all header files under .\security folder to see if they are classifed properly.
# DEF098862: Categorisation tags need to be checked for all of security sub-system  
#

use Cwd;

my $masterdir = "$ENV{'SECURITYSOURCEDIR'}";
my $keyTagFile = "@" . "file";
my $keyTagInternal = "@" . "internal";
my $keyTagPublished = "@" . "published";

my $numfiles = 0;
my $numuntagged = 0;
my $numtagged = 0;

$logFile = "\\epoc32\\winscw\\c\\searchtags.txt";

#
#Function to write log into file
#
sub WriteLog 
	{
	my ($log) = @_;
	unless( open($fh, ">> $logFile")) 
		{
		printf STDERR "Can\'t open $logfile:$!\n";
		return;
		}
	printf $fh $log;
	printf $log;
	close $fh;
	}

#
#Main
#
unlink($logFile);
WriteLog("Searching Classification Tags test.\n\n");
WriteLog("Scans all header files to make sure that all the files are already classified correctly at file level.\n");
WriteLog("A properly classifed header file must contain an \@file tag and then one of the following API classification tags:\n");
WriteLog("\@publishedAll, \@publishedPartner, \@internalAll, \@internalTechnology or \@internalComponent\n\n"); 

#check if the folder is really exists
if ( ! -d $masterdir ) 
	{ 
	WriteLog("Can't find `$masterdir'\n"); 
	}
else
	{
	chdir $masterdir;
	&SearchFolder($masterdir);
	}
#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $numfiles);
WriteLog(sprintf "Passed: %d\n", $numtagged);
WriteLog(sprintf "%d tests failed out of %d\n", $numuntagged, $numfiles); 

#
#Function to check a file to see whether it has any file level classification tag
#	
sub SearchEngine
	{
	 my ($filename) = @_;
	 
	 my $fileexist = 0;
	 my $fileleveltag = 0;
	 my $subleveltag = 0;
	 my $distance = 0;

	 if ( lc(substr($filename,-2)) eq ".h"  )
            { 
            ++$numfiles;	
            $pid = open FILE, "<$filename"; 
            while (<FILE>)
            	{
                chop;
                if($distance)
                	{
                	++$distance;
                	}    
                if ((/$keyTagFile/)) #check if file level tag exists
                	{ 
					++$fileexist;
					++$distance;
			  		}
			  			
			  	if((/$keyTagInternal/) || (/$keyTagPublished/)) #check if the classification tag exists
			  		{
			  		if(($fileexist == 1) && ($fileleveltag == 0))
			  			{
			  			if($distance < 15)#if the classification tag is far away, it means it belongs to something else.
			  				{
			  				++$fileleveltag;
			  				$distance = 0;
			  				}
			  			}
			  		elsif(($fileexist == 1) && ($fileleveltag == 1))
			  			{
			  			++$subleveltag;
			  			}
			  		}		
                }
                
            if (($fileexist!=1) || ($fileleveltag != 1))
				{
				++$numuntagged; 
				WriteLog("$filename: $_\nFailed\n");
				}
			else
				{
				++$numtagged;
				}
				
            close FILE;
            #print "fileexist=$fileexist fileleveltag=$fileleveltag\n";
     		}           
	}

#
#Function to search all subfolders of the given base folder
#
sub SearchFolder 
	{
    local($dir,$nlink) = @_;
    local($dev,$ino,$mode,$subcount);

    ($dev,$ino,$mode,$nlink) = stat('.') unless $nlink;

    opendir(DIR,'.') || die "Can't open $dir";
    local(@filenames) = readdir(DIR);
    closedir(DIR);
	
    if ($nlink == 2) 
    	{# There is no subdirectories.
        for (@filenames) 
        	{
            next if $_ eq '.';
            next if $_ eq '..';
        
			SearchEngine("$dir/$_");           
            }
    	}
    else 
    	{# There are some subdirectories.
        $subcount = $nlink - 2;
        
        for (@filenames) 
        	{
            next if $_ eq '.';
            next if $_ eq '..';
            
        	next if lc(substr($_,0,4)) eq "test"; 			#Ignore test files
        	next if lc(substr($_,0,7)) eq "openssl";		#Ignore openssl files
        	next if lc(substr($_,0,9)) eq "wincrypto";		#Ignore wincrypto files
        	next if lc(substr($_,0,4)) eq "utf8";			#Ignore utf8.h sntpclient
        	next if lc(substr($_,0,10)) eq "sntpclient";	#Ignore sntpclient
        	next if lc(substr($_,0,17)) eq "referencedrmagent"; 			#Ignore test referencedrmagent files
			next if lc(substr($_,0,14)) eq "securitytestfw"; 			#Ignore test test files
			
            $name = "$dir/$_";
			SearchEngine("$name");
            # If checked all the subdirs
            next if $subcount == 0;

            ($dev,$ino,$mode,$nlink) = lstat($_);
            next unless -d _;

            chdir $_ || die "Can't cd to $name";
            &SearchFolder($name,$nlink);
            chdir '..';
            --$subcount;
        	}
    	}
	}
	
# end.

