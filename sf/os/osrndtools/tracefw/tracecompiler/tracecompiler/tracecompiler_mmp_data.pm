#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Module for TraceCompiler. parse the mmp file and prepare an object to be re-used by tracecompiler.pl and tracecompiler_perse_mmp.pl
#
package tracecompiler_mmp_data;

use strict;
use warnings;
use File::Basename;
use tracecompiler;
sub readMmp($);


my $component_name;


sub new
{
	my $pkg = shift;
	my $self = {};
	bless $self,$pkg;
	my $mmp = shift;
	if (defined($mmp) and -e $mmp) {
	  tracecompiler::debugMsg("Starting to parse MMP file: $mmp");
	  my $file_path = $mmp;
	  $component_name =  basename $mmp;

	  # Take the module name from the MMP file path. e.g. "X:/temp/mycomponent.mmp" would create "mycomponent"
	  $component_name =~ s/([^\.]*).*/$1/;

		$self->{mmpName} = $mmp;
		$self->readMmp($mmp);
	} else 
	{
		tracecompiler::debugMsg("tracecompiler_mmp_data :: Valid MMP file must be provided ...\n");
	}

	return $self;
}

# parse mmp and get the infos we need. This method moved from tracecompiler_parse_mmp.pl to here with some add-on
sub readMmp($)
{
	my $self = shift;
	my $file = shift;
	if (-e $file)
	{
		tracecompiler::debugMsg("Starting to parse file: $file");
		my @sources;
		my @tracesfolders;
		my $in_comment_block = 0;

		# Get file path (remove file + extension)
		my $file_path = dirname $file;

		# Change "//" to "/"
		$file_path =~ s/\/\//\//g;

		my $current_src_path = $file_path;

		# Go through lines
		open FILE, "<$file" or die $!;
		foreach my $line (<FILE>)
		{
			# Check if contains includes
			if ($line =~ /#include.+\"(.*)?\"/i)
			{
				my $includedFile = $1;

				# Get absolute path if the first character is not "\" or b"/"
				if ($includedFile =~ /^[^\\\/]/)
				{
					$includedFile = tracecompiler::concatenatePath($file_path, $includedFile);
				}
				else
				{
					$includedFile = substr($file_path, 0, 2) . $includedFile;
				}

				if (defined $includedFile)
				{
					tracecompiler::debugMsg("Found #include from $file. Start parsing it..\n");
					$self->readMmp($includedFile);
				}
			}

			# Check if in comment block
			if ($in_comment_block > 0)
			{
				if ($line =~ /\*\/(.*)/)
				{
					$line = $1;
					$in_comment_block--;
				}
				if ($in_comment_block == 0)
				{
					# Comment block ended, continue parsing the line
				}
				else
				{
					# We are still in comment block, jump to next line
					next;
				}
			}

			# Delete possible comments in one line
			$line =~ s/\/\/.*//; # //
			$line =~ s/\/\*.*\*\///; # /* */

			if ($line =~ /(.*?)\/\*/)
			{
				$line = $1;
				$in_comment_block++;
			}

			# Find uid
			if ($line =~ /uid.+0x([a-fA-F0-9]+)?/i)
			{
				$self->{uid} = $1;

				tracecompiler::debugMsg("Found Uid: $self->{uid}");
			}

			#Find target
			if($line =~ /target\s+(\S+)\.(\S+)/i)
			{
				$self->{target} = $1;
				$self->{ext} = $2;
			}

			#Find target type
			if($line =~ /targettype\s+(\S+)/i)
			{
				$self->{type} = $1;
			}

			# Find source path
		if ($line =~ /sourcepath\s+([^\s]+)/i)
			{
				my $src_path = $1;

				# Get absolute path if the first character is not "\" or "/"
				if ($src_path =~ /^[^\\\/]/)
				{
					$current_src_path = tracecompiler::concatenatePath($file_path, $src_path . "/");
				}
				else
				{
					$current_src_path = substr($file_path, 0, 2) . $src_path;
				}

				tracecompiler::debugMsg("Source path changed to: $current_src_path");
			}

			# Find sources
			while ($line =~ /source\s+([^\s]+)/i)
			{
				my $src = $1;

				my $src_path = tracecompiler::concatenatePath($current_src_path, $src);

				if (-e $src_path)
				{
					push(@sources, $src_path);

					tracecompiler::debugMsg("Found source: $src_path");
				}
				else
				{
					tracecompiler::debugMsg("Source doesn't exist!: $src_path");
				}

				$line =~ s/\Q$src//;
			}

			#Find unserincludes
			if ($line =~ /userinclude\s+([^\s]+)/i)
			{
				tracecompiler::debugMsg("Found userinclude: $line");
				my $userinclude = $1;
				$userinclude =~ s/\\/\//g;
				my $tmp1 = $self->{target} . "_" . $self->{ext};
				my $tmp2 = $self->{target} . "_" . $self->{type};
				if (   $userinclude =~ /.*\/traces\/$tmp1$/i
				or $userinclude =~ /.*\/traces_$tmp2$/i
				or $userinclude =~ /.*\/traces_$component_name$/i
				or $userinclude =~ /.*\/traces$/i)
				{
					tracecompiler::debugMsg("Found traces userinclude: $userinclude");
					push(@tracesfolders, $userinclude);
				}
			}
		}
		if (scalar @sources > 0)
		{
			$self->{sources} = [@sources];
		}
		if (scalar @tracesfolders > 0)
		{
			$self->{tracespaths} = [@tracesfolders];
		}
		close FILE;

		tracecompiler::debugMsg("Ending the parsing of MMP file: $file\n");
	} else
	{
		tracecompiler::debugMsg("Could not find file: $file\n");
	}

}

1;