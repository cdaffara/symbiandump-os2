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

#!/usr/bin/perl
use strict;
use warnings;
use Getopt::Long;
#use Cwd 'abs_path';
use Cwd;


# Main
my $root_dir = "";
GetOptions("dir=s" => \$root_dir);
if ($root_dir eq "") {
    $root_dir = ".";
}
convert_cpp_in_directory($root_dir);
convert_files_in_directory($root_dir);
print "Conversion completed.\n";

sub convert_cpp_in_directory {
    my $dir_name = shift;
    print "Opening directory: " . $dir_name . "\n";
    opendir(DIR, $dir_name) || die "Cannot open directory " . $dir_name;
    chdir($dir_name);
    my @sourcefiles = readdir(DIR);
    foreach (@sourcefiles) {
        if ( $_ =~ /.cpp$/i ) {
            # make the file writable (0666 is in linux/unix terms rw-)
            chmod(0666,$_);
            print "Converting: ";
            print $_;
            my $converted_file_content = "";
            if (/.cpp$/ ) {
                $converted_file_content = convert_source_file_content($_);
            }
	    open(my $result_file_handle, ">", $_) or die(". Writing " . $_ . " failed!\n");
	    print $result_file_handle $converted_file_content;
	    close $result_file_handle;
            print ". Done\n";
        }
        elsif ( /\./ ) {
            # Other types of files
        }
        else {
            # Directories    
            convert_cpp_in_directory($_);
            chdir(".."); # After recursion change back to the current directory
        }
    }
    closedir DIR;
}

sub convert_files_in_directory {
    my $dir_name = shift;
    print "Opening directory: " . $dir_name . "\n";
    opendir(DIR, $dir_name) || die "Cannot open directory " . $dir_name;
    chdir($dir_name);
    my @sourcefiles = readdir(DIR);
    foreach (@sourcefiles) {
        if ( $_ =~ /.h$/i || $_ =~ /.mmp$/i || $_ =~ /.def$/i) {
            # make the file writable (0666 is in linux/unix terms rw-)
            chmod(0666,$_);
            print "Converting: ";
            print $_;
            my $converted_file_content = "";
            if (/.h$/) {
                $converted_file_content = convert_source_file_content($_);
            }
            elsif (/.mmp$/) {
                $converted_file_content = convert_mmp_file_content($_);
            }
            else {
                $converted_file_content = convert_def_file_content($_);
            }
	    open(my $result_file_handle, ">", $_) or die(". Writing " . $_ . " failed!\n");
	    print $result_file_handle $converted_file_content;
	    close $result_file_handle;
            print ". Done\n";
        }
        elsif ( /\./ ) {
            # Other types of files
        }
        else {
            # Directories    
            convert_files_in_directory($_);
            chdir(".."); # After recursion change back to the current directory
        }
    }
    closedir DIR;
}


sub convert_source_file_content {
    my $file_name = shift;
    
    my $file_content = read_file_content_into_string($file_name);

    #check if this source include a separate test table header file
    #in that case, we need to insert the test table content from header first
    if ($file_content =~ m/\#include\s*\"(.*)testtable\.h\"/) {
	    my $curpath = cwd; 
            my $table_file_name = $curpath . "/../inc/" . $1 . "testtable.h";
	    print "\n    try to merge header file at: " . $table_file_name . "\n";
            my $tabledef = read_file_content_into_string($table_file_name);
	    #remove copyright and other comments
            $tabledef =~ s/\/\/.*|\/\*[\s\S]*?\*\///g;
            $tabledef =~ s/#include\s*\".*\"//g;
            $file_content =~ s/\#include\s*\".*testtable\.h\"/$tabledef/g;  


    }
    
    # Convert the EUnit test table to SymbianUnit tests and move it to the constructor
    my $symbianunit_constructor_content = "BASE_CONSTRUCT";
    my $converted_test_table = convert_eunit_test_table($file_content);
    #print "converted test table: " . $converted_test_table . "\n";
    $symbianunit_constructor_content .= $converted_test_table;
    $file_content =~ s/CEUnitTestSuiteClass::ConstructL\(.*\)\;/$symbianunit_constructor_content/g;

    # Remove the EUnit test table
    $file_content =~ s/EUNIT_BEGIN_TEST_TABLE(.*)EUNIT_END_TEST_TABLE//ms;
    $file_content =~ s/\/\/  TEST TABLE//ms;

    # Do rest of the conversions
    $file_content =~ s/#include <eunitmacros.h>/#include <symbianunittestmacros.h>/gi;
    $file_content =~ s/#include <ceunittestsuiteclass.h>/#include <symbianunittest.h>/gi;
    $file_content =~ s/#include <ceunittestsuite.h>/#include <symbianunittestsuite.h>/gi;
    $file_content =~ s/#include <eunitdecorators.h>//gi;
    $file_content =~ s/CEUnitTestSuiteClass/CSymbianUnitTest/g;
    $file_content =~ s/CEUnitTestSuite/CSymbianUnitTestSuite/g;
    $file_content =~ s/MEUnitTest/MSymbianUnitTestInterface/g;  
    $file_content =~ s/EUNIT_DECLARE_TEST_TABLE;//g;
    $file_content =~ s/EUNIT_ASSERT_SPECIFIC_LEAVE/SUT_ASSERT_LEAVE_WITH/g;
    $file_content =~ s/EUNIT_ASSERT_LEAVE/SUT_ASSERT_LEAVE/g;
    $file_content =~ s/EUNIT_ASSERT_EQUALS/SUT_ASSERT_EQUALS/g;
    $file_content =~ s/EUNIT_ASSERT_NO_LEAVE//g;
    $file_content =~ s/EUNIT_ASSERT/SUT_ASSERT/g;
    $file_content =~ s/EUNIT_ASSERT_DESC/SUT_ASSERT_DESC/g;
    $file_content =~ s/EUNIT_ASSERT_EQUALS_DESC/SUT_ASSERT_EQUALS_DESC/g;
    $file_content =~ s/EUNIT_PRINT/\/\/EUNIT_PRINT/g;

    return $file_content;
}


sub convert_mmp_file_content {
    my $file_name = shift;
    
    my $file_content = read_file_content_into_string($file_name);

    $file_content =~ s/eunit.lib/symbianunittestfw.lib/gi;
    $file_content =~ s/eunitutil.lib//gi;
    $file_content =~ s/\/epoc32\/include\/Digia\/EUnit//gi;
    $file_content =~ s/\/epoc32\/include\/platform\/Digia\/EUnit//gi;
    $file_content =~ s/TARGETPATH(.*)\/DigiaEUnit\/Tests//gi;
    $file_content =~ s/UID(.*)0x1000af5a/MACRO SYMBIAN_UNIT_TEST\nUID 0x20022E76/gi;

    return $file_content;
}


sub convert_def_file_content {
    my $file_name = shift;
    
    my $file_content = read_file_content_into_string($file_name);

    $file_content =~ s/MEUnitTest/MSymbianUnitTestInterface/g;

    return $file_content;
}


sub read_file_content_into_string {
    my $file_name = shift;

    open(my $src_file_handle, "<", $file_name) or die("\nFile not found!\n");
    my @file_content_array = <$src_file_handle>;
    my $file_content = "";
    foreach (@file_content_array) {
        $file_content .= $_;
        }
    close $src_file_handle;

    return $file_content;
}


sub convert_eunit_test_table {
    my $file_content = shift;
    my $converted_test_table = "\n";
    if ($file_content =~ /EUNIT_BEGIN_TEST_TABLE(.*)EUNIT_END_TEST_TABLE/ms) {  
        my $test_table_content = $1;
        my @test_list = split(/EUNIT_TEST/, $test_table_content);
        shift @test_list; # Remove the test table parameters before the first test
        foreach (@test_list) {
            if ($_ =~ /\((.*)\)/ms) {
                $converted_test_table .= "\n";
                $converted_test_table .= convert_eunit_test_entry($1);
            }
        }
    }
    return $converted_test_table;
}


sub convert_eunit_test_entry {
    # Parameters for EUNIT_TEST:
    # text1, text2, text3, text4, setupFunc, runFunc, teardownFunc
    my $eunit_test_parameters = shift;
    # Remove whitespaces, tabs and line breaks
    $eunit_test_parameters =~ s/\s//g;
    my @test_parameter_array = split(/,/, $eunit_test_parameters);
    my $result = "";
    if (@test_parameter_array == 7) {
        if ($test_parameter_array[4] ne "SetupL" || 
            $test_parameter_array[6] ne "Teardown") {
            # Non-default setup or teardown used
            $result = "    ADD_SUT_WITH_SETUP_AND_TEARDOWN( ";
            $result .= $test_parameter_array[4];
            $result .= ", ";
            $result .= $test_parameter_array[5];
            $result .= ", ";
            $result .= $test_parameter_array[6];
            $result .= " )";
        }
        else {
            $result = "    ADD_SUT( ";
            $result .= $test_parameter_array[5];
            $result .= " )";
        }
    }   
    return $result;
}
