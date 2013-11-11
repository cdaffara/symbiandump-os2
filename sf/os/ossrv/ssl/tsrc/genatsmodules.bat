rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

rem Redistribution and use in source and binary forms, with or without 
rem modification, are permitted provided that the following conditions are met:

rem Redistributions of source code must retain the above copyright notice, this 
rem list of conditions and the following disclaimer.
rem Redistributions in binary form must reproduce the above copyright notice, 
rem this list of conditions and the following disclaimer in the documentation 
rem and/or other materials provided with the distribution.
rem Neither the name of Nokia Corporation nor the names of its contributors 
rem may be used to endorse or promote products derived from this software 
rem without specific prior written permission.

rem THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
rem AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
rem IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
rem DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
rem FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
rem DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
rem SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
rem CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
rem OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
rem OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
rem
rem Description:      
rem


@perl -x GenATSModules.bat %1 %2 %3 %4 %5 %6 %7 %8 %9 
@goto end

#!perl
use strict;
use File::Find;
use File::Copy;
use Cwd;

my $sendEmail = "no";
my $target = "winsspd";
my $suite;
my $install = "no";
my $session = "no";
my $combinedSession = "no";
my $sdkversion="";
my $noXmlsGenList = "play_new";

my %DirHash;


if(scalar @ARGV ==0 || $ARGV[0] =~ /[\-\\\/][h\?]/ || $ARGV[0] =~ /help/ )
{
	print <<EOHelp;
genatsmodules.bat - hardcoded script to generate ATSINPUT folders for mobileruntime and ngi projects	
Usage:
	genatsmodules.bat
		Create ATSINPUT for the default testsuite based on 
		location it is executed from place in stdlibs/internal 
		and run and it generates ATSINPUT for stdlibs
		similiarly place it in ssl/internal or glib/internal or 
    ngi/internal and run to generate  ATSINPUT for ssl, glib  
    and ngi respectively	
  
  genatsmodules [libname]
			where libname can be stdlibs, ssl, glib, etc. Creates 
			ats test drop for given library. Valid from any internal
			folder of mobileruntime.
			
  
  genatsmodules ngi
			create ngi testsuite. Valid inside ngi/internal only
			
genatsmodules play_new
			create play_new testsuite. Valid inside play_new/internal only
				
	
      
	genatsmodules ltp
			create ltp testsuite. Valid inside stdlibs/internal only
	
	genatsmodules lsb
			create lsb testsuite. Valid inside stdlibs/internal only
			
	genatsmodules stdcpp
			create stdcpp testsuite. Valid inside stdcpp/internal only
			
	genatsmodules [libname] install 
		 where libname can be stdlibs, ssl, glib, ltp, lsb to create a 
		 ATSINPUT where the dependent mobileruntime libraries are also 
		 copied and used (It copies the dependent libraries using the 
		 ATS component files)
		
	genatsmodules session [libname] [target] 		
		where libname is any of the standard libs as shown above and 
		target can be winsspd, winscw, scarfe, zeus etc. to create 
		session files for scarfe, winsspd etc. 
		
genatsmodules session combined [target] 				
      will look at the existing ATSINPUT(which can have any 
      combination of created components and create a combined 
      session file. This way you get increased flexibility in 
      the libraries you choose to combine into a single session 
      file. If the target is a hardware target(that is not winsspd
      or winscw), the script will first attempt to find a target 
      specific component file and if not found, the generic hw 
      component file. 
	

	Note:
	ATSINPUT creation is a 2 stage process. First we create 
	the components and then we run (for example) 
	"genatsmodules session stdlibs winscw" and 
	"genatsmodules session stdlibs scarfe" to create the 
	session files.
EOHelp
		exit();
}
my $do_armv5 = 1;
my $do_winscw = 1;

if(scalar @ARGV == 1)
{
	if($ARGV[0] eq "session")
	{
		die("parameter session must be followed by libname and target platform");
	}
	else
	{
		$suite = $ARGV[0];
	}
}
elsif(scalar @ARGV == 2)
{
	if($ARGV[0] eq  "session")
	{
		die("parameter session must be followed by libname and target platform");
	}
	elsif($ARGV[1] eq "install")
	{
		$suite = $ARGV[0];
		$install = "yes";
	}
	elsif($ARGV[1] eq "armv5")
	{
		$do_winscw = 0;
	}
	elsif($ARGV[1] eq "winscw")
	{
		$do_armv5 = 0; 
	}
	else
	{
		die("second parameter after target platform $ARGV[0] can only be \"install\" ");
	}
}
elsif(scalar @ARGV == 3)
{
		if($ARGV[0] ne "session")
		{
			die("invalid combination of parameters. Program expected 1st parameter to be \"session\" ");
		}
		else
		{
			$session = "yes";
			$suite = $ARGV[1];
			$target = $ARGV[2];
	
			if($suite eq "combined")
			{		
				$combinedSession = "yes";
	  	}
			
		}
}
elsif(scalar @ARGV == 4)
{
		if($ARGV[0] ne "session")
		{
			die("invalid combination of parameters. Program expected 1st parameter to be \"session\" ");
		}
		else
		{
			$session = "yes";
			$suite = $ARGV[1];
			$target = $ARGV[2];
			$sdkversion = $ARGV[3];
			if($suite eq "combined")
			{		
				$combinedSession = "yes";
	  	}
		}
}
else
{
		die("program recieved too many parameters!");
}#end of commandline processing


	
my $rootDrive = substr(getcwd(), 0,2);
my $templateRoot = getcwd();

if(rindex($templateRoot, "tsrc") == -1 && ($suite ne "stdlibs" && $suite ne "pcts" ) )
{
	die("Please run the script from the appropriate internal folder in the mobileruntime /omerta project!!");
}
else
{

	 my $srcPrefix = getcwd();
	 $srcPrefix =~ /(.*)tsrc.*/;
	 $srcPrefix = $1;
	 if(rindex($templateRoot, "stdlibs") != -1)
	 {
	 	$srcPrefix =~ /(.*)stdlibs.*/;
	 	$srcPrefix = $1;
	 	
	 	if($suite eq "glib")
	 	{
	 		$templateRoot = $srcPrefix."/glib/internal/";
	 	}
	 	elsif($suite eq "ssl")
	 	{
	 		$templateRoot = $srcPrefix."/ssl/tsrc/";
	 	}
	 	elsif($suite eq "ngi" && $session ne "yes")
	 	{
	 		print "ngi atsinput creation not supported from mobileruntime\n";
	 		exit();
	 	}
	 }
	 
	if(rindex($templateRoot, "glib") != -1)
	{
		$suite = "glib";
	}
	elsif(rindex($templateRoot, "ssl") != -1)
	{
		$suite = "ssl";
	}
	elsif(rindex($templateRoot, "ngi") != -1)
	{
		$suite = "ngi";
	}
}


my $optsGroupPath = $templateRoot."/ts/ltp/opts/group/";
#$optsGroupPath = getcwd()."/ts/ltp/opts/group/";
my $ltpPath =$templateRoot."/ts/ltp/";
my $stdcppPath =$templateRoot."/testapps/stlport/";
my @stdexeList = ("gnu","tstdcpp","stdcxx/tstapps");
#my $gnuPath =getcwd()."/testapps/gnu/";
#my $tstdcppPath =getcwd()."/testapps/tstdcpp/";
my	$ATSRoot = getcwd()."\\ATSINPUT";

my @generatedModules;
my %cfglocation;

my $TDIR = "\\internal\\testapps\\";

my @libcModules = ( "libc_loc_blr",  "libc_time_blr", "libc_db_blr","tctype", "tifioctls", "tinet",
                      "tlibcwchar", "tlink", "tmkfifo", "tmmap", "tmsgqueue",
		      "tnetdb", "tpipe","tregex", "tsemaphore", "tshm","tsocket" ,
		      "tstdio", "tstdlib", "tstring", "tsyscalls",
		      "tsyssim", "twctype", "tselect", "tsysunistd", "twideapis", "twopen", "twchar",
		      "twcharapi",
               );

my @libdlModules = ("tlibdl");
my @libmModules = ("libm_double_blr", "libm_float_blr","libm_ldouble_blr");


my @pthreadModules = ( "testcondbroadcast", "testconddestroy", "testcondinit", "testcondsignal", "testcondtimedwait", "testcondwait",
"tmutex", "tpthread", "testpthreadonce", "testsemclose", "testsemdestroy", "testsemgetvalue", "testseminit", "testsemopen", "testsempost",
"testsemtimedwait", "testsemtrywait", "testsemunlink", "testsemwait");


my @stdliblist = ( "libc", "libpthread", "libdl", "libm", "libz", "libcrypt");
my @dllList = ("backend.dll", "libc.dll", "LibCIpcClient.dll", "LibCIpcServer.exe", "libdl.dll", "libm.dll", "libpthread.dll", "libcrypt.dll", "libz.dll", "liblogger.dll");


my @stdcpplist = (
  				 ["fstream1_input.txt", "stlport_fstream", "/../internal/testapps/doc/", "/General/", "C:\\TestFramework\\tstdcpp"],
  				 ["nextperm1_good.txt", "stlport_nextprm", "/../internal/testapps/doc/", "/General/", "C:\\TestFramework\\tstdcpp"],
  				 ["nextperm2_good.txt", "stlport_nextprm", "/../internal/testapps/doc/", "/General/", "C:\\TestFramework\\tstdcpp"],
  				 ["ostmit_good.txt", "stlport_ostmit", "/../internal/testapps/doc/", "/General/", "C:\\TestFramework\\tstdcpp"],
  				 ["seek.txt", "stlport_seek", "/../internal/testapps/doc/", "/General/", "C:\\TestFramework\\tstdcpp"],
  				 ["istream_extractor_other-1.tst","istream_extractor_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_extractor_other-1.txt","istream_extractor_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_extractor_other-2.tst","istream_extractor_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_unformatted-1.tst","istream_unformatted", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_unformatted-1.txt","istream_unformatted", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_unformatted-2.tst","istream_unformatted", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_inserter_char-1.tst","ostream_inserter_char", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_inserter_other-1.tst","ostream_inserter_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_inserter_other-2.tst","ostream_inserter_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],  				 
  				 ["istream_extractor_other-1.tst","w_istream_extractor_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_extractor_other-1.txt","w_istream_extractor_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_extractor_other-2.tst","w_istream_extractor_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_unformatted-1.tst","w_istream_unformatted", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_unformatted-1.txt","w_istream_unformatted", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_unformatted-2.tst","w_istream_unformatted", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_inserter_char-1.tst","w_ostream_inserter_char", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_inserter_other-1.tst","w_ostream_inserter_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_inserter_other-2.tst","w_ostream_inserter_other", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["filebuf_members-1.tst","filebuf_members", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["filebuf_members-1.txt","filebuf_members", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["filebuf_virtuals-1.tst","filebuf_virtuals", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["filebuf_virtuals-1.txt","filebuf_virtuals", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["filebuf_virtuals-2.tst","filebuf_virtuals", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["filebuf_virtuals-3.tst","filebuf_virtuals", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ifstream_members-1.tst","ifstream_members", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ifstream_members-1.txt","ifstream_members", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ios_base_members_static-1.tst","ios_base_members_static", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_seeks-1.tst","istream_seeks", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_seeks-1.txt","istream_seeks", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_seeks-2.tst","istream_seeks", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["istream_seeks-3.tst","istream_seeks", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ofstream_members-1.tst","ofstream_members", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_inserter_char-1.txt","ostream_inserter_char", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["ostream_seeks-1.tst","ostream_seeks", "/../internal/testapps/gnu/io/doc/", "/General/", "C:\\TestFramework\\gnu"],
  				 ["infile_auto.txt","multi_thread_auto", "/../internal/testapps/doc/", "/General/", "C:\\TestFramework\\tstdcpp"],
);

my @gliblist = (
  				 ["1.gmarkup", "markup_test", "/epoc32/winscw/c/", "/General/", "C:"],
   				 ["1.gmarkup", "markup-test", "/epoc32/winscw/c/", "/General/", "C:"],
  				 ["iochannel-test-infile", "iochannel_test",  "/epoc32/winscw/c/", "/General/", "C:"],
  				 ["iochannel-test-infile", "iochannel-test",  "/epoc32/winscw/c/", "/General/", "C:"],
#  				 ["casemap.bin",  "unicode-caseconv",  "/epoc32/winscw/c/", "/General/", "C:"],
#  				 ["casefold.bin", "unicode-caseconv",  "/epoc32/winscw/c/", "/General/", "C:"],
  				 ["utf8.txt", "unicode-encoding","/epoc32/winscw/c/", "/General/", "C:"],
  				 ["casecollate.txt", "unicode-collate","/epoc32/winscw/c/", "/General/", "C:"],
  				 ["utf8_hindi.txt", "tutf8","/epoc32/winscw/c/", "/General/", "C:"],
  				 ["scanfile.txt", "tscanner","/epoc32/winscw/c/", "/General/", "C:"],
  				 ["test.txt", "dir_test","/epoc32/winscw/c/temp/tmp/", "/General/", "C:\\temp\\tmp"],
  				 ["NormalizationTest.txt", "unicode-normalize","/epoc32/winscw/c/", "/General/", "C:"],
  				 
  				 ["helloworld.exe", "spawn_test","/epoc32/release/armv5/urel/", "/armv5_urel/", "C:\\sys\\bin"],
  				 ["helloworld.exe", "spawn_test","/epoc32/release/winscw/udeb/", "/winscw_udeb/", "Z:\\sys\\bin"],
  				 
  				 ["libmoduletestplugin_a.dll", "module-test","/epoc32/release/armv5/urel/", "/armv5_urel/", "C:\\sys\\bin"],
  				 ["libmoduletestplugin_a.dll", "module-test","/epoc32/release/winscw/udeb/", "/winscw_udeb/", "Z:\\sys\\bin"],
  				 
  				 ["libmoduletestplugin_b.dll", "module-test","/epoc32/release/armv5/urel/", "/armv5_urel/", "C:\\sys\\bin"],
  				 ["libmoduletestplugin_b.dll", "module-test","/epoc32/release/winscw/udeb/", "/winscw_udeb/", "Z:\\sys\\bin"],
  				 
  			  				 
  				 );
  				 
my @ngilist = (
           ["Test.txt", "ngi_sa_runtimetester", "/EPOC32/winscw/c/Private/10202be9/", "/General/", ""],
  				 ["sa_input_repository.txt", "shared","/EPOC32/armv5/c/TestFramework/", "/armv5_urel/", ""],
  				 ["sa_input_repository.txt", "shared","/EPOC32/winscw/c/TestFramework/", "/winscw_udeb/", ""],
  				 ["audiomixingfiles.ini", "ngi_sa_audiomixing","/EPOC32/winscw/c/TestFramework/", "/general/", ""],
  				 ["sa_camera_supported.ini", "ngi_sa_camera","/EPOC32/armv5/c/TestFramework/", "/general/", ""],
  				 ["sa_clip.ini", "ngi_sa_clip","/EPOC32/winscw/c/TestFramework/", "/general/", ""],
  				 ["devicecapabilitiesdata.ini", "ngi_sa_devicecapabilities","/EPOC32/winscw/c/TestFramework/", "/general/", ""],
   				 ["highlevelaudiofiles.ini", "ngi_sa_highlevelaudio","/EPOC32/winscw/c/TestFramework/", "/general/", ""],
					 ["lowlevelaudiofiles.ini", "ngi_sa_lowlevelaudio","/EPOC32/winscw/c/TestFramework/", "/general/", ""],
					 ["runtimedata.ini", "ngi_sa_runtimetester","/EPOC32/winscw/c/TestFramework/", "/general/", ""],						
					 ["sa_stream.ini", "ngi_sa_stream","/EPOC32/winscw/c/TestFramework/", "/general/", ""],						
					 #["*", "ngi_sa_videoplayback","/../graphics/video/internal/sa_videoplayback/data/", "/general/", ""],
					 ["test.*", "ngi_sa_images","/Epoc32/winscw/c/Data/", "/general/", ""]
					);

  				 


my $validexpansions = "all stdlibs combined";
my $validLibraries = "ltp opts lsb glib ssl ngi pcts stdcpp gcce libc-locale play_new";
if(rindex($validexpansions, $suite)== -1 && rindex($validLibraries, $suite) == -1)
{
	die("$suite is not a valid library. Valid options are $validexpansions $validLibraries");
}

my $dontStrip = 1;
my	@liblist;
if($suite eq "all")
{
		@liblist = split " ", $validLibraries;
		@liblist = (@liblist, @stdliblist);
		
}#suite all
elsif($suite eq "ltp")
{
	 @liblist = ("ltp", "opts");
}
elsif($suite eq "stdlibs")
{
		@liblist = @stdliblist;
}
elsif($suite eq "ssl")
{
	@liblist = ("libcrypto", "libssl");
}
else
{
		@liblist = ("$suite");
}

#	foreach my $l(@liblist)
#	{
#			print "$l was there\n";
#	}


if($combinedSession eq "yes")
{
	GenerateSessionFile("combined", $target,"combined");
	exit();
}

if($session eq "yes")
{
	my $file = $templateRoot."/testsession.xml";
	opendir(DIRHANDLE, $ATSRoot) || die("Please try to create Session files only after ATSINPUT/components has been created!");
	closedir(DIRHANDLE);
	$dontStrip = 1;
	if($suite eq "ngi" || $suite eq "play_new")
	{
		@generatedModules = GenGenModules($suite);
		my @componentFiles = GetXmls($suite);
		GenerateSessionFile("combined",$target,$suite,\@componentFiles);
		exit();
	}
	elsif($suite eq "ssl")
	{
		my @componentFiles = GetSslXmls();
		GenerateSessionFile("combined",$target,$suite,\@componentFiles);
		exit();
	}
	elsif($suite eq "stdlibs")
	{
		#my $dst = $ATSRoot."/$suite"."_testsession.xml";
		#copy($file, $dst);
		GenerateSessionFile("combined",$target,$suite);
		exit();
	}
	foreach my $lib(@liblist)
	{          
			@generatedModules = GenGenModules($lib);
			if(scalar @generatedModules != 0)
			{
				#print "Generating for $lib\n";
				GenerateSessionFile($lib, $target,$lib);
			}
	}
	
	#@generatedModules = GetAllModules();
	#if(scalar @generatedModules != 0)
	#{
	#		GenerateSessionFile("combined",$target, "combined");
	#}
	exit();
}#session generation code


if ($suite eq "ltp" || $suite eq "all")
{
	my @ltpModules = GetLtpModules();
	@generatedModules = @ltpModules;
	DoGen("ltp", ".dll");
	CopyLtpCfg("ltp");
	
	
	my @saved = @dllList;
	@dllList = ("ltplib.dll");
	CopySharedDlls("ltp", \@dllList);
	TransformXmls("ltp","shared");
	@dllList = @saved;
	
	@generatedModules = GetCompiledModules($optsGroupPath."/bld.inf");
	DoGen("opts",".dll");

}
if ($suite eq "glib")
{
	my $bldfile = $templateRoot."/testapps/group/bld.inf";
	my @glibModules = GetCompiledModules($bldfile);
	@generatedModules = @glibModules;
	DoGen("glib",".exe");
	my $gDataRef = \@gliblist;
	CopyArbitDatFiles("glib",$gDataRef);
	
	if($install eq "yes")
	{
		my @saved = @dllList;
		my @glib2dllList = ("glibbackend.dll", "Libgobject.dll", "libgmodule.dll", "libgthread.dll",
                "libglib.dll");
                
   my  @glib1List = ( "libgobject.dll", "libgmodule.dll", "libgthread.dll",  "libglib.dll", );
  @dllList = @glib2dllList;
  CopySharedDlls("glib",\@dllList);     	
  TransformXmls("glib", "shared");
   	
		@dllList = @saved;
	}#if install
}#glib


if ($suite eq "ssl")
{
	my $stripStatus = $dontStrip;
	$dontStrip = 0;
	GenCryptoTest();
	if($install eq "yes")
	{
		@dllList = (@dllList, "libcrypto.dll", "libssl.dll");   
	}#if install
	$dontStrip = $stripStatus;
}#ssl

if($suite eq "ngi")
{

	GenNgiTest();
	CopyNgiXmls();
	CopyArbitDatFiles("",\@ngilist);

}

if($suite eq "play_new")
{
	my @play_newModules = ("fonttestmodule", "messagingtestmodule","locationtestmodule", "alertstestmodule", 
	"libxmlstiff","ngisvgtestmodule");
	@generatedModules = @play_newModules;
	DoGen("play_new",".dll");
	
	 my @list = (
  				 			 ["*.cfg", "fonttestmodule", "/../fonts/internal/testmodules/fonttestmodule/group/","/General/", ""],
	 				 			 ["*.xml", "fonttestmodule", "/../fonts/internal/testmodules/fonttestmodule/group/","/", ""],
	 				 			 
	 				 			 ["*.cfg", "messagingtestmodule", "/../ngimessaging/internal/testmodules/messagingtestmodule/group/","/General/", ""],
	 				 			 ["*.xml", "messagingtestmodule", "/../ngimessaging/internal/testmodules/messagingtestmodule/group/","/", ""],
	 				 			 ["*", "messagingtestmodule", "/EPOC32/winscw/c/messagingtestmodule/","/General/", ""],
	 				 			 
	 				 			 ["*.cfg", "locationtestmodule", "/../ngilocation/internal/testmodules/locationtestmodule/group/","/General/", ""],
	 				 			 ["*.xml", "locationtestmodule", "/../ngilocation/internal/testmodules/locationtestmodule/group/","/", ""],
	 				 			 
	 				 			 ["*.cfg", "alertstestmodule", "/../ngialerts/internal/testmodules/alertstestmodule/group/","/General/", ""],
								 ["*.xml", "alertstestmodule", "/../ngialerts/internal/testmodules/alertstestmodule/group/","/", ""],	 				 			 
								 ["vertigo.wav", "alertstestmodule", "/Epoc32/winscw/c/alertstestmodule/","/General/", ""],	 				 			 	 				 			 
	 		
	 			 			  ["*.cfg", "libxmlstiff", "/../libxmlpp/internal/group/","/General/", ""],
	 		 	 			  ["*.xml", "libxmlstiff", "/../libxmlpp/internal/group/","/", ""],
	 		 	 			  ["*.*", "libxmlstiff", "/../libxmlpp/internal/data/","/General/", ""],
	 		 	 			  
	 		 	 			  ["*.cfg", "ngisvgtestmodule", "/../ngisvg/internal/testmodules/ngisvgtestmodule/group/","/General/", ""],
   		 	 			  ["*.xml", "ngisvgtestmodule", "/../ngisvg/internal/testmodules/ngisvgtestmodule/group/","/", ""],
	 		 	 			  ["*.*", "ngisvgtestmodule", "/epoc32/winscw/c/ngisvgtest/","/General/", ""],
	 				 			 
  				 );
 
  my $bDataRef = \@list;  				 
	CopyArbitDatFiles("play_new",$bDataRef);  				 
	if($install eq "yes")
	{
		@generatedModules = GenGenModules("play_new");
		my @saved = @dllList;
		my @playList = (
"ngifonts.dll",
"ngimessaging.dll",
"ngisvg.dll",
"ngilocation.dll",
"ngialerts.dll",
"libstdcpp.dll",
"libxmlpp.dll",
"FeedsLibXml2xmlpp.dll");
       @dllList = @playList;
		CopySharedDlls("play_new",\@dllList);     
	  TransformXmls("play_new", "shared");
		@dllList = @saved;

	}#end install


}

if($suite eq "stdcpp")
{
	@generatedModules = GetstdcppModules();
	DoGen("stdcpp",".dll");
	CopyLtpCfg("stdcpp");
	@generatedModules = GetstdexeModules();
	DoGen("stdcpp",".exe");
	
	my @saved = @dllList;
	@dllList = ("stdcxxtestengine.dll");
	CopySharedDlls("stdcpp", \@dllList);
	TransformXmls("stdcpp","shared");
	@dllList = @saved;
	
	CopyArbitDatFiles("stdcpp",\@stdcpplist);
	
	if($install eq "yes")
	{
		@generatedModules = GenGenModules("stdcpp");
		my @saved = @dllList;
		my @cppList = ("libstdcpp.dll");
       @dllList = @cppList;
		CopySharedDlls("stdcpp",\@dllList);     
	  TransformXmls("stdcpp", "shared");
		@dllList = @saved;
	}#if install
}
	

if($suite eq "lsb" || $suite eq "all")
{
	my @lsbModules = GetLsbModules();
	@generatedModules = @lsbModules;
	DoGen("lsb",".exe");
	
	my @depDlls;
	my $dll;
	
	#copying the common dlls
	@depDlls  = 
	(
	 "libapi.dll",
	 "tcm.dll",
	 "vlib.dll",
	);
		
	foreach $dll (@depDlls)
	{
		CopyDeps("lsb",$dll);
	}
	#now copy the cfg files
	CopyLsbCfg("lsb");	
	#now copy the extra dependency dlls
	
	#dlopen
	@generatedModules = ("dlopen");
	
	
	
	@depDlls  = 
	(
	 "dynlibtest.1.dll",
	 "dynlibtest.11.1.dll",
	 "dynlibtest.11.2.dll",
	 "dynlibtest.12g.dll",
	 "dynlibtest.12r.dll",
	 "dynlibtest.14.1.dll",
	 "dynlibtest.14.2.dll",
	 "dynlibtest.14.3.dll",
	 "dynlibtest.3.1.dll",
	 "dynlibtest.3.2.dll",
	 "dynlibtest.3.dll",
	 "dynlibtest.4.dll",
	 "dynlibtest.8.dll",
	 "dynlibtest.9.dll",
	 "dynlibtest.3.dll",
	 "dynlibtest.8.dll",
	);
	
	foreach $dll (@depDlls)
	{
		CopyDeps("lsb",$dll);
	}
	
	#dlclose
	@generatedModules = ("dlclose");
	@depDlls = ("dynlibclose.1.dll");
	foreach $dll (@depDlls)
	{
		CopyDeps("lsb",$dll);
	}
	
	#dlerror
	@generatedModules = ("dlerror");
	@depDlls = ("dynlib.dll");
	foreach $dll (@depDlls)
	{
		CopyDeps("lsb",$dll);
	}
	
	#now copy the xmls!
	my $srcDir = $templateRoot."/ts/lsbsuites/group/";
	
	my $dst;
	
	foreach my $dllib("dlopen", "dlclose", "dlerror")
	{
		$dst  = $ATSRoot."\\components\\lsb_".$dllib."\\";
		copy($srcDir."/lsb_".$dllib."_module_blr_hw_component.xml", $dst) || die("DIED: unable to copy from $srcDir!\n");
		copy($srcDir."/lsb_".$dllib."_module_blr_winsspd_component.xml", $dst) || die("DIED: unable to copy from $srcDir!\n");
	}
	
}

if($suite eq "gcce")
{

	mkdir $ATSRoot;
	mkdir $ATSRoot."\\components" || die ("DIED: unable to create component dir");
	
	@generatedModules = ("gcce-validate", "gcce-load");
		
	my @depDlls  = 
	(
		"gcce-lib.dll",
	);
	
	my $suffix = ".exe";
	my $libname = "gcce";
	
	
	my $src;
	my $dst;
	foreach my $module(@generatedModules)
	{
		my $relroot = $ATSRoot."\\components\\$libname"."_$module";
		mkdir $relroot || die("DIED: unable to create dir $module\n");
		mkdir $relroot."\\armv5_urel" || die("DIED: unable to create dir $relroot\\armv5_urel\n");
		
		#GenExeXmls($module,$libname);
		
		$src = $rootDrive."\\epoc32\\release\\gcce\\urel\\".$module.$suffix;
		$dst = $ATSRoot."\\components\\".$libname."_".StripModulePrefix($module);
		$dst = $dst."\\armv5_urel\\";
		copy($src, $dst)  || warn("unable to copy $src to $dst");
	}
	$src = $rootDrive."\\epoc32\\release\\gcce\\urel\\"."gcce-lib.dll";
	$dst = $ATSRoot."\\components\\gcce_gcce-load";
	$dst = $dst."\\armv5_urel\\";
	copy($src, $dst)  || warn("unable to copy $src to $dst");
	
	
	my @list = (
  				 ["gcce_gcce-load_module_blr_hw_component.xml", "gcce-load", "./","/", ""],
  				 ["gcce_gcce-validate_module_blr_hw_component.xml", "gcce-validate", "./","/", ""],
  				 );
 
  my $bDataRef = \@list;  				 
	CopyArbitDatFiles("gcce",$bDataRef);  				 

	
}#if gcce



if($suite eq "libc-locale")
{
	$dontStrip = 0;
	mkdir $ATSRoot;
	mkdir $ATSRoot."\\components" || die ("DIED: unable to create component dir");
	
	@generatedModules = ("tstring");
	DoGen("libc",".dll");
	
	my @localelist = (
	  				 ["locales.txt", "string", "../libc/internal/testapps/tstring/data/", "/General/", ""],
	  				 ["tstring_locale_component.xml", "string", "../libc/internal/testapps/tstring/group/", "/", ""],
	 
	  				 );
		
  my $bDataRef = \@localelist;  				 
	CopyArbitDatFiles("libc",$bDataRef);  				 
	#delete($ATSRoot."/components/libc_string/"."libc_string_module_blr_hw_component.xml") || die("unable to delete");
	my $my_src = $ATSRoot."/components/libc_string/"."tstring_locale_component.xml";
	my $my_dst = $ATSRoot."/components/libc_string/"."libc_string_module_blr_hw_component.xml";
	
	unlink($my_dst) ||warn("unable to delete $my_dst");
	rename($my_src,$my_dst) || move($my_src,$my_dst) || die("Unable to rename!");
	
}#if libc-locale


if($suite eq "pcts")
{
	my $bldfile = $templateRoot."/group/bld.inf";
	my @pctsModules = GetCompiledModules($bldfile);
	@generatedModules = @pctsModules;
	DoGen("pcts",".exe");
	
	my @depDlls;
	my $dll;
	
	
	#copying the common dlls
	@depDlls  = 
	(
		"vsxlibapi.dll",
		"rttm.exe",
		"rtlibapi.dll",
		"vsxvlib.dll",
		"vsxgenlib.dll",
		"vsxtsetlib.dll",
		"vsxvport.dll",
	);
	
	if($install eq "yes")
	{
		my @saved = @dllList; 
		@dllList = @depDlls;	
		CopySharedDlls("pcts", \@depDlls);
		TransformXmls("pcts","shared");
		@dllList = @saved;
		
		#now copy the cfg files
		my $dst;
		my $srcDir = "/epoc32/winscw/c/data/tet/";
		my $file1 = $srcDir."tet_code";
		my $file2 = $srcDir."tetexec.cfg";
		foreach my $module(@generatedModules)
		{
			$dst = $ATSRoot."\\components\\pcts_".$module;
			$dst = $dst."\\General\\";
			copy($file1, $dst) or die "copy: $! $file1 $dst";
			copy($file2,$dst) or die "copy: $! $file2 $dst";
		}
	}
		
	
	
	
	my $dst;	
	foreach my $module(@generatedModules)
	{
		my $file3 = "/epoc32/winscw/c/logs/tet/m.".$module;

		$dst = $ATSRoot."\\components\\pcts_".$module;
		$dst = $dst."\\General\\";
		copy($file3, $dst) or die "copy: $! $file3 $dst";
	}#module
}



$dontStrip = 0;

if($suite eq "stdlibs" || $suite eq "all")
{
	@generatedModules = ("tzlib");
	DoGen("libz", ".dll");

	@generatedModules = ("tlibcrypt");
	DoGen("libcrypt",".dll");

	@generatedModules = @libcModules;
	DoGen("libc",".dll");
	GenBackendTest();
	
	@generatedModules = ("twchar");
	CopyDeps("libc","twchar_helloworld.exe");
	
	@generatedModules = ("twopen");
	CopyDeps("libc","twpopenwritechild.exe");
	CopyDeps("libc","twpopenreadchild.exe");


	@generatedModules = @libdlModules;
	DoGen("libdl",".dll");
	CopyDeps("libdl","Dll1.dll");
	CopyDeps("libdl","Dll2.dll");
	CopyDeps("libdl","DependencyTestDll.dll");

	@generatedModules = @libmModules;
	DoGen("libm",".dll");

	$TDIR = "\\internal\\";
	@generatedModules = @pthreadModules;
	DoGen("libpthread",".dll");
	CopyDeps("libpthread","testharness.dll");

	CopyDatFiles();
}

if($install eq "yes")
{
	foreach my $lib(@liblist)
	{   
		#print "Transforming lib $lib\n";       
		@generatedModules = GenGenModules($lib);
		my @oldList = @dllList;
		if($do_winscw)
  	{
  		@dllList = (@dllList, "wsdsoln.dll", "libestw32.dll");
  		CopySharedDlls($lib,\@dllList,"winscw");     	
	  	TransformXmls($lib, "shared","winscw");
  	}
  	@dllList = @oldList;
  	if($do_armv5)
  	{
	  	CopySharedDlls($lib,\@dllList,"armv5");     	
	  	TransformXmls($lib, "shared","armv5");
  	}
	}#FOREACH
}#if install



#function to generate ModuleList for a compiled bld.inf
sub GetCompiledModules($)
{
	my $file = shift @_;
	my @moduleList;
	
	
	# Open input file
	open (INFILE, "<".$file ) || die ("DIED: Can not find $file!");

	while (<INFILE>)
	{
		if(/(.*)\.mmp/ && $1 !~ /^\/\//) 
	  {
	  		if($_ !~ /\/\/Not for ATS/)
	  		{
	  			push @moduleList, $1;
	  		}
	  }
	}
	close (INFILE);
	return @moduleList;
}

my @groupList;
sub GetLtpModules()
{
        my $prevDir = getcwd();
        my @rootList = ("kernel", "misc");
        my @totalgroupList;
        foreach my $node(@rootList)
        {
        	chdir $ltpPath."/$node";
        	@groupList = ();
        	find(\&MakeGroupList, ".");
        	@totalgroupList = (@totalgroupList, @groupList);
        }
        chdir $prevDir;
        return @totalgroupList;
}

sub GetstdexeModules()
{
      my $prevDir = getcwd();
      my @fullList = ();
      foreach my $group(@stdexeList)
      {
      	my $groupPath = $templateRoot."/testapps/".$group. "/";
      	chdir $groupPath;
      	@groupList = ();
      	find(\&grovelAllMmpFiles, ".");
      	chdir $prevDir;
      	@fullList = (@fullList,@groupList);
      }
      return @fullList;      
}

sub grovelAllMmpFiles()
{
	if($_ =~ /(.*)\.mmp/ && ! ($_ =~ /manual/))
	{
		push @groupList, $1;#get name of module
	}
}

sub GetstdcppModules()
{
        my $prevDir = getcwd();
        my @rootList = ("auto");
        my @totalgroupList;
        foreach my $node(@rootList)
        {
        	chdir $stdcppPath."/$node";
        	@groupList = ();
        	find(\&MakeGroupList, ".");
        	@totalgroupList = (@totalgroupList, @groupList);
        }
        chdir $prevDir;
        return @totalgroupList;
}

sub GetTstdcppModules()
{
      my $prevDir = getcwd();
      #chdir $tstdcppPath;
      @groupList = ();
      find(\&grovelAllMmpFiles, ".");
      chdir $prevDir;
      return @groupList;
}

sub MakeGroupList()
{
	my $currentDir = $File::Find::dir;
	
	#print "Current dir:$currentDir\n";
	if( $currentDir =~ /.*\/(.*)\/group/)
	{
		if($cfglocation{$1} eq "")
		{
			$cfglocation{$1} = getcwd()."\/$1.cfg";
			push @groupList, $1;#get name of module
		}#first time in group
	}#group found
}#subrtn end


#function that generates the ATS setup
sub DoGen($$)
{
	my $libname = shift @_;
	my $ext = shift @_;
	mkdir $ATSRoot;
	mkdir $ATSRoot."\\components" || die ("DIED: unable to create component dir");
	
	
	GenDirs($libname);
	my $module;
	foreach my $listed(@generatedModules)
	{
	my @filesToProcess =
        (
         "LibXXX_ATSTemplateXXX_module_blr_hw_component.xml",
         "LibXXX_ATSTemplateXXX_module_blr_winsspd_component.xml",
         "testframework.ini",
        );
		if(rindex($noXmlsGenList, $libname) != -1 )	
		{
			@filesToProcess = ("testframework.ini",);
		}

			if($ext eq ".exe")
			{
				GenExeXmls($listed,$libname);
			}
			else
			{
				#just in case no xml file was supplied
				
        my $ref = \@filesToProcess;
				GenXmls($libname, $listed,$ref);
			}
	}#foreach
	
	CopyDlls($libname,$ext);
	#copy the provided cfg, ini and xmls
	if($libname eq "opts")
	{
		CopyOptsCfg($libname);
	}
	elsif($libname eq "ltp" || $libname eq "lsb" || $libname eq "glib")
	{
	  #copy latter and  dont copy respectively for ltp and lsb
	}
	else
	{
	  CopyCfgIniXmls($libname);
	}
}

sub CopyCfgIniXmls()
{
	my $libname = pop @_;
	
	my $libRoot = $templateRoot;
	$libRoot =~ /(.*)internal.*/;
	$libRoot = $1;


	my $module;
	my $src;
	my $dst;
		
	foreach my $listed(@generatedModules)
	{
	  my $dirname = $listed;
	  $dirname =~ s/test/t/;
	  $module = StripModulePrefix($listed);
	    
		$src = $libRoot.$libname.$TDIR.$dirname."\\group\\";
				
		my $xmlCopied = "0";
		my @globpattern = ("*.ini", "*.xml","*.cfg");
		foreach my $pat(@globpattern)
		{
			$dst = $ATSRoot."\\components\\".$libname."_".$module;
			if($pat ne "*.xml")
			{
				$dst = $dst."\\General\\";
			}
			else
			{
				$dst = $dst."\\";
			}
			my @filesFound = glob($src."\\$pat");
			if(scalar @filesFound eq "0")
			{
				#print "No files found for module $module $pat!\n";
				#print $src;
			}
			
			#handcoded
			if($pat eq "*.cfg" && $xmlCopied eq "0")#rename only if xml not copied
			{
				if(scalar @filesFound > 1)
				{
					print "Multiple cfg files found. Copying all!\n";
				}
				else
				{
						$dst = $dst.$listed.".cfg";
				}
			}				
			elsif($pat eq "*.xml")
			{
				my $folder = $dst;
				foreach my $from (@filesFound) 
				{
					if($from =~ /winscw_component/)
					{
						$dst = $folder.$libname."_".$module."_module_blr_winscw_component.xml";
						copy($from, $dst) or die "DIED: copy: $! $from $dst";
						$xmlCopied = "1";
					}
					elsif($from =~ /winsspd_component/)
					{
						$dst = $folder.$libname."_".$module."_module_blr_winsspd_component.xml";
						copy($from, $dst) or die "DIED: copy: $! $from $dst";
						$xmlCopied = "1";
					}
					elsif($from =~ /hw_component/)
					{
						$dst = $folder.$libname."_".$module."_module_blr_hw_component.xml";
						copy($from, $dst) or die "DIED: copy: $! $from $dst";
						$xmlCopied = "1";
					}
				}#examine individually			
				if(scalar @filesFound > 2)
				{
					#print "Multiple xml files found!!!\n";
				}
				@filesFound = ();
			}	
				
			
			foreach my $from (@filesFound) 
			{
					copy($from, $dst) or die "DIED: copy: $! $from $dst";
			}#copy individually			
		}#globpattern
	}#module
}#fn

#copy the cfg files for the opts test suite
sub CopyOptsCfg()
{
	my $libname = pop @_;
	
	my $libRoot = $optsGroupPath."\\";
	my $module;
	my $src;
	my $dst;
		
	foreach $module(@generatedModules)
	{
		$src = $libRoot.$module."\.cfg";
		$dst = $ATSRoot."\\components\\".$libname."_".$module;
		$dst = $dst."\\General\\";
		copy($src, $dst) or warn "copy: $! $src $dst";
	}#module
}#fn

#copy the lsb cfg files
sub CopyLsbCfg()
{
	my $libname = pop @_;
	
	my $module;
	
	my $dst;
	
	my $srcDir = $templateRoot."/ts/lsbsuites/lsblib/data/";
		
	foreach $module(@generatedModules)
	{
		my $file1 = $srcDir."tet_code.cfg";
		my $file2 = $srcDir."tetexec.cfg";
		$dst = $ATSRoot."\\components\\".$libname."_".$module;
		$dst = $dst."\\General\\";
		copy($file1, $dst) or die "copy: $! $file1 $dst";
		copy($file2, $dst) or die "copy: $! $file2 $dst";
	}#module
}#fn


#copy the ltp cfg files
sub CopyLtpCfg($)
{
	my $libname = pop @_;
	
	my $module;
	my $src;
	my $dst;
		
	foreach $module(@generatedModules)
	{
		$src = $cfglocation{$module};
		$dst = $ATSRoot."\\components\\".$libname."_".$module;
		$dst = $dst."\\General\\";
		copy($src, $dst) or warn "copy: $! $src $dst";
	}#module
}#fn


sub CopyDlls($$)
{
	my $libname = shift @_;
	my $suffix = shift @_;
	
	my $module;
	my $src;
	my $dst;
	
	
	foreach $module(@generatedModules)
	{
	  if($do_winscw == 1)
	  { 	  
			$src = $rootDrive."\\epoc32\\release\\winscw\\udeb\\".$module.$suffix;
			$dst = $ATSRoot."\\components\\".$libname."_".StripModulePrefix($module);
			$dst = $dst."\\winscw_udeb\\";
			copy($src, $dst)  || warn("unable to copy $src to $dst");
		}
	  if($do_armv5 == 1)
	  {
			$src = $rootDrive."\\epoc32\\release\\armv5\\urel\\".$module.$suffix;
			$dst = $ATSRoot."\\components\\".$libname."_".StripModulePrefix($module);
			$dst = $dst."\\armv5_urel\\";
			copy($src, $dst)  || warn("unable to copy $src to $dst");
		}
	}#foreach
}
#fn to generate the session file
sub GenerateSessionFile($$$$)
{
	my $libname = shift @_;
	my $target = shift @_;
	my $name = shift @_;
	my $listptr = shift @_;
	my $outdir = $ATSRoot;
	my $infile = $templateRoot."\\testsession.xml";
	chdir $outdir || die("DIED: Unable to chdir!"); 
	#open INFILE, $infile || die ("DIED: Can not open input file $infile");
	my $prefix = $name."_".$target;
	if($sdkversion ne "")
	{
		$prefix = $prefix."_".$sdkversion;
	}
	
	open OUTFILE, ">".$outdir."\\".$prefix."_testsession.xml" || die ("DIED: Can not open output file");
	my $line;
  my $snippet = $target;
  if($target ne "winscw" && $target ne "winsspd")
  {
  	$snippet = "hw";
  }

print OUTFILE "<?xml version=\"1.0\"?>\n";
print OUTFILE "<testsession>\n";
#print OUTFILE "<symbian-version>$libname"."_".$snippet."_TestDrop_blr_component</symbian-version>\n";	
print OUTFILE "<symbian-version>ats_test_session</symbian-version>\n";	
print OUTFILE "<testitem>at</testitem>\n";	
print OUTFILE "<resultfile>ATSOUTPUT\\".$prefix."_testsession_results.xml</resultfile>\n";		
if($sendEmail eq "yes")
{
print OUTFILE <<EOEmailNote;
<email_notification>
<include_test_cases>true</include_test_cases>
<message_file>C:\\\\lm.txt </message_file>
<send_after_rerun>false</send_after_rerun>
<subject>ATS testreport</subject>
<from>swbuild\@nokia.com</from>
<to>santosh.ks\@nokia.com</to>
<to>Imtiyaj.Kaji\@nokia.com</to>
<to>tp-ap-india-mrt-testdg\@nokia.com</to>
<to>tp-ap-india-mrt-libsdg\@nokia.com</to>
<to>subeesh.sivanandan\@nokia.com</to>
<to>girish.bhat\@nokia.com</to>
</email_notification>
EOEmailNote
}

	if($libname ne "combined")
	{
		AddAssumedIncludes($libname,$snippet);
	}
	else
	{
		if($listptr ne "")
		{
			AddFileList($libname,$listptr);
		}
		else
		{
			AddActualIncludes($libname);
		}
	}
   				
	print OUTFILE "</testsession>\n";

	close(OUTFILE);
	#close(INFILE);
}#fn


sub AddAssumedIncludes($$)
{
	my $libname = shift @_; 
	my $snippet = shift @_;
	
	my $APITest;
	foreach my $listed(@generatedModules)
						{
							$APITest = StripModulePrefix($listed);
							print OUTFILE  "\n";
							print OUTFILE "	<!--Inclusion of $target xml for $APITest -->\n";
							print OUTFILE "	<component>\n";
							print OUTFILE "		<factory>Symbian</factory>\n";
							if($libname eq "all")
							{
									print OUTFILE "		 <testplan>ATSINPUT\\components\\$listed\\$listed"."_module_blr_".$snippet."_component.xml</testplan>\n";	 
							}
							else
							{
									print OUTFILE "		 <testplan>ATSINPUT\\components\\$libname"."_"."$APITest\\$libname"."_$APITest"."_module_blr_".$snippet."_component.xml</testplan>\n";	 
							}
							if($target eq "winscw")
							{
									print OUTFILE "		<target hardware=\"winscw\" build=\"udeb\" />\n";
							}
							elsif($target eq "winsspd")
							{
								print OUTFILE " <target hardware=\"winsspd\" platform=\"winscw\" build=\"udeb\" />\n"; 
							}
							else
							{
										print OUTFILE "		<target hardware=\"$target\" build=\"urel\" />\n";
							}
							print OUTFILE "	</component>\n";
							print OUTFILE "\n";
	           }#foreach @generatedModules
}


sub AddFileList($$)
{
	my $libname = shift @_; 
	my $listptr = shift @_;
	my @componentfiles = @$listptr;
	
			foreach my $listed(@componentfiles)
						{
								print OUTFILE  "\n";
							print OUTFILE "	<!--Inclusion of $target xml -->\n";
							print OUTFILE "	<component>\n";
							print OUTFILE "		<factory>Symbian</factory>\n";
							print OUTFILE "		 <testplan>ATSINPUT\\components\\$listed</testplan>\n";	 
							if($target eq "winscw")
							{
									print OUTFILE "		<target hardware=\"winscw\" build=\"udeb\" />\n";
							}
							elsif($target eq "winsspd")
							{
								print OUTFILE " <target hardware=\"winsspd\" platform=\"winscw\" build=\"udeb\" />\n"; 
							}
							else
							{
										print OUTFILE "		<target hardware=\"$target\" build=\"urel\" />\n";
							}
							print OUTFILE "	</component>\n";
							print OUTFILE "\n";
	           }#foreach @componentfiles
}



sub GetSslXmls()
{
	my $APITest;
	my @componentfiles;
	
	my @emulatorfiles = 
	(
	"libssl_ssltest\\libssl_blr_winsspd_component.xml",
	"libcrypto_crypto_test\\libcrypto_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltestss_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltestverify_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltestenc_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltestgen_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltpkcs7_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltpkcs7d_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltreq_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltreq2_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltrsa_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltcrl_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltsid_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltx509_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltx5091_blr_winsspd_component.xml",
	"libcrypto_openssl\\libcrypto_topenssltx5092_blr_winsspd_component.xml",
	 );
	 
	my @hwfiles =
	(
		"libcrypto_openssl\\libcrypto_topenssltestss_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltestverify_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltestenc_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltestgen_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltpkcs7_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltpkcs7d_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltreq_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltreq2_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltrsa_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltcrl_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltsid_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltx509_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltx5091_blr_hw_component.xml",
		"libcrypto_openssl\\libcrypto_topenssltx5092_blr_hw_component.xml",		
		"libssl_ssltest\\libssl_blr_hw_component.xml",
		"libcrypto_crypto_test\\libcrypto_blr_hw_component.xml",
	);
	if($target eq "winsspd" || $target eq "winscw")
	{
		@componentfiles = @emulatorfiles;
	}
	else
	{
		@componentfiles = @hwfiles;
	}
	return @componentfiles;
}



sub AddActualIncludes($)
{
	my $libname = shift @_; 
	find(\&XmlComponentFiles, ".");
	my $key;
	my $value;
	while(($key, $value) = each(%DirHash)) 
	{
							$value =~ s/\//\\/g;
							print OUTFILE  "\n";
							print OUTFILE "	<!--Inclusion of dynamically found $target xml -->\n";
							print OUTFILE "	<component>\n";
							print OUTFILE "		<factory>Symbian</factory>\n";
							print OUTFILE "		 <testplan>$value</testplan>\n";	 
							if($target eq "winscw")
							{
									print OUTFILE "		<target hardware=\"winscw\" build=\"udeb\" />\n";
							}
							elsif($target eq "winsspd")
							{
								print OUTFILE " <target hardware=\"winsspd\" platform=\"winscw\" build=\"udeb\" />\n"; 
							}
							else
							{
										print OUTFILE "		<target hardware=\"$target\" build=\"urel\" />\n";
							}
							print OUTFILE "	</component>\n";
							print OUTFILE "\n";
	
	}#while
}#fn

sub XmlComponentFiles()
{
	if($_ !~ /\.xml/ || $_ =~ /manual/)
	{
		return;
	}
	#print "Processing $_\n";
	my $dir = getcwd();
	if($dir =~ /.*(ATSINPUT.*)/)
	{
			$dir = $1;
			if($dir !~ /.*components.*/)
			{
				return;
			}
			#print "The dir is $dir\n";
	}
	else
	{
		return;
	}
	
	if($target eq "winsspd" )
	{
		if($_ =~ /winsspd/)
		{
			#print "Adding $target file $_ for $dir\n";
			$DirHash{$dir} = $dir."/".$_;
		}
	}
	elsif($target eq "winscw")
	{
		if($_ =~ /winscw/)
		{
			#print "Adding $target file $_ for $dir\n";
			$DirHash{$dir} = $dir."/".$_;
		}
	}
	elsif($_ =~ /$target/)
	{
		#print "$_ matched $target \n";
		#print "Adding $target file $_ for $dir\n";
		$DirHash{$dir} = $dir."/".$_;
	}
	elsif($_ =~/hw/)
	{
		#print "$_ matched hw";
		if(scalar $DirHash{$dir} == 0)
		{
			#print "Adding hw file $_ for $dir\n";
			$DirHash{$dir} = $dir."/".$_;
			#print " and added\n"
		}
		else
		{
			#print " and not added\n";
		}
	}
}



sub emptyArray()
{
	@generatedModules = ();
}

sub GenDirs($)
{
 my $libname = shift @_;
 my $dirname;
 foreach my $module(@generatedModules)
 {
  $dirname = StripModulePrefix($module);
	my $relroot = $ATSRoot."\\components\\$libname"."_$dirname";
	mkdir $relroot || die("DIED: unable to create dir $dirname\n");
	mkdir $relroot."\\General" || die("DIED: unable to create dir $relroot\\General\n");
	if($do_armv5 == 1)
	{
		mkdir $relroot."\\armv5_urel" || die("DIED: unable to create dir $relroot\\armv5_urel\n");
	}
	if($do_winscw == 1)
	{
		mkdir $relroot."\\winscw_udeb" || die("DIED: unable to create dir $relroot\\winscw_udeb\n");
	}
 }
}

sub GenXmls($$$)
{
	my $libname = shift @_;
	my $listedName = shift @_;
	my $bref = shift @_;
	my @filesToProcess = @$bref;
	
	#strip leading test and t names from module
	my $moduleName = StripModulePrefix($listedName);

      my $toPath = $ATSRoot."\\components\\$libname"."_$moduleName\\"; 
        

       my $dst;
       my $src;
       foreach $src(@filesToProcess)
       {
               $dst = $src;
               $dst =~ s/ATSTemplateXXX/$moduleName/;
               $dst =~ s/LibXXX/$libname/;
             

               if($src eq "testframework.ini")
               {
					$dst =  $toPath."General\\".$dst;
               }
               else
               {
					$dst =  $toPath.$dst;
               }
               copy($templateRoot."\\".$src, $dst) || die("DIED: Unable to copy $src to $dst\n");
               ExpandModuleMacros($dst, $libname, $listedName);
       }#foreach file
}


sub GenExeXmls()
{
	my $moduleName = shift @_;
	my $libname = shift @_;
			
	my $toPath = $ATSRoot."\\components\\$libname"."_$moduleName\\"; 
  my @filesToProcess =
  (
     "LibXXX_ATSTemplateXXX_exemodule_blr_hw_component.xml",
     "LibXXX_ATSTemplateXXX_exemodule_blr_winsspd_component.xml",
  );
	
	my $dst;
  my $src;
  foreach $src(@filesToProcess)
  {
        $dst = $src;
        $dst =~ s/ATSTemplateXXX/$moduleName/;
        $dst =~ s/LibXXX/$libname/;
        $dst =~ s/_exemodule_/_module_/;
				$dst =  $toPath.$dst;
        copy($templateRoot."\\".$src, $dst) || die("DIED: Unable to copy to $dst\n");
        ExpandModuleMacros($dst, $libname, $moduleName);
   }#foreach file
}

#function to expand the macros existing in a file
#file is replaced by new version
#args are relativePath. filename and  $moduleName
sub ExpandModuleMacros()
{
	# Take module name
	my $moduleName = pop @_;
	my $libname = pop @_;
	my $filename = pop @_;

	my $MODULENAME = $moduleName;
	$MODULENAME =~ tr/[a-z]/[A-Z]/;

	# Open input file
	open (INFILE, $filename ) || die ("DIED: Can not find $filename");

	#Open output file
	my $newOutput = $filename."new";
	open (OUTFILE, ">".$newOutput ) || die ("DIED: Can not open $newOutput");

	# Replace text in files
	while (<INFILE>)
	{
	  s/ATSTemplateXXX/$moduleName/g;
	  s/LibXXX/$libname/g;
    	  print OUTFILE $_;
	}

	# Close filehandles
	close (INFILE);
	close (OUTFILE);

	# Rename result file
	unlink $filename;
	rename $newOutput,$filename;
}



sub CopyDeps($$)
{
  my $libname = shift @_;
  my $copyDll = shift @_;
	my $src;
	my $dst;
	foreach my $listed(@generatedModules)
	{
				my $module = StripModulePrefix($listed);
				if($do_winscw == 1)
				{
					$src = $rootDrive."\\epoc32\\release\\winscw\\udeb\\".$copyDll;
					$dst = $ATSRoot."\\components\\$libname"."_$module";
					$dst = $dst."\\winscw_udeb\\";
					copy($src, $dst)  || warn("unable to copy $src to $dst");
				}
				if($do_armv5 == 1)
				{
					$src = $rootDrive."\\epoc32\\release\\armv5\\urel\\".$copyDll;
					$dst = $ATSRoot."\\components\\$libname"."_$module";
					$dst = $dst."\\armv5_urel\\";
					copy($src, $dst)  || warn("unable to copy $src to $dst");
				}
	}#foreach
}#fn

#dat file copy
sub CopyDatFiles()
{
	my $libRoot = $templateRoot;
	$libRoot =~ /(.*)internal.*/;
	$libRoot = $1;

 my @list = (
  				 ["libcrypt", "tlibcrypt", "group/Test_Data.dat"],
  				 ["libc", "tnetdb", "data/services.txt"],
  				 ["libc", "tstring", "data/locales.txt"],
  				 ["libc", "libc_loc_blr", "data/locales.txt"],
  				 ["libz", "tzlib", "data/*.*"],
  				 ["libc", "tlink", "data/*.*"],
  				 ["libc", "tmmap", "data/*.*"],
           );

	 my $libname;
	 my $dirname;
	 my $glob;
	foreach my $ref(@list)
	{
			my @inarr = @$ref;
			($libname, $dirname, $glob) = @inarr;
			my $modulename = StripModulePrefix($dirname);
			my $src = "$libRoot/$libname/internal/testapps/$dirname/$glob";
			my $dst = $ATSRoot."\\components\\$libname"."_"."$modulename\\General\\";
  		if($dirname eq "libc_loc_blr")
  		{
  			$src = "$libRoot/libc/src/locales.txt";
  		}
  		my @filesFound = glob($src);
  		if(scalar @filesFound == 0)
  		{
  			die("DIED: no Dat file found to copy!");
  		}
  		foreach my $file(@filesFound)
  		{
  			copy($file, $dst) || die("DIED: unable to copy $file to $dst");
  		}
  }#ref processing
}#fn


#glib dat file copy
sub CopyArbitDatFiles($$)
{
	
	 my $libname = shift @_;
	 my $refArr = shift @_;
	 my @list = @$refArr;
	 
 	
	my $fname;
	my $modulename;
	my $location;
	my $atsDst;
	my $targetDst;
	
	#now copy the files appropriately
	foreach my $ref(@list)
	{
			my @inarr = @$ref;
			($fname, $modulename, $location, $atsDst, $targetDst) = @inarr;
			#print "[$fname, $modulename, $location, $atsDst, $targetDst]\n";
			if($location =~ /(.*)\.\.(.*)/)
			{
				$location = GetPathPrefix().$2;
			}
			else
			{
				$location = $rootDrive.$location;
			}
			my $src = $location.$fname;
			my $dst = $ATSRoot."\\components\\$libname"."_".$modulename.$atsDst;
			if($libname eq "")
			{
				$dst = $ATSRoot."\\components\\".$modulename.$atsDst;
				#print "$dst CHANGED!\n"
			}
  		
  		my @filesFound = glob($src);
  		if(scalar @filesFound == 0)
  		{
  			die("DIED: no Dat file  $src found to copy!");
  		}
  		foreach my $file(@filesFound)
  		{
  			copy($file, $dst) || die("DIED: unable to copy $file to $dst");
  		}
  		
  		if($targetDst ne "")
  		{
  			 		my @filesToChange = ();
			  		if($atsDst =~ "armv5")
			  		{
			  			#armv5 component file
			  			unshift @filesToChange, $libname."_".$modulename."_module_blr_hw_component.xml";
			  			
			  		}
			  		elsif($atsDst =~ "winscw")
			  		{
			  			#winscw component file
			  			unshift @filesToChange, $libname."_".$modulename."_module_blr_winsspd_component.xml";
			  		}
			  		else
			  		{
			  			#add entry to both xmls
			  			unshift @filesToChange, $libname."_".$modulename."_module_blr_hw_component.xml";
			  			unshift @filesToChange, $libname."_".$modulename."_module_blr_winsspd_component.xml";
			  		}
			  		foreach my $file(@filesToChange)
			  		{
			  			$src = $ATSRoot."/components/".$libname."_"."$modulename/".$file;
			  			ModifyXml($src,$libname,$fname,$atsDst,$targetDst);
			  		}
			  }#updating the xmls
    		
  }#ref processing
 
  
}#fn

sub ModifyXml($$$$$)
{
	my $filename = shift @_;
	my $lib = shift @_;
	my $fname = shift @_;
	my $atsDst = shift @_;
	my $targetDst = shift @_;
	
	
	#printf("Modifying $filename with lib= $lib fname= $fname atsDst = $atsDst targetDst = $targetDst\n");
	
	# Open input file
	open (INFILE, $filename ) || die ("Can not find $filename");

	#Open output file
	my $newOutput = $filename."new";
	open (OUTFILE, ">".$newOutput ) || die ("Can not open $newOutput");

		
	# Replace text in files
	while (<INFILE>)
	{
	  if(/\<stif\>/ || /\<execute\>/)
	  {
	  	if(/\<stif\>/)
	  	{
	  		print OUTFILE "\<stif\>\n";
	  	}
	  	else
	  	{
	  		print OUTFILE "\<execute\>\n";
	  	}
	  	my $type = "data";
	  	if($atsDst !~ "General")
			{
				$type = "binary";
			}
			print OUTFILE <<EObinary;
				
	  		<install type=\"$type\">
				<src>$fname</src>
				<dst>$targetDst\\$fname</dst>
 				</install>
EObinary
	  }#stif or execute tag found
	  else
	  {
	  	  print OUTFILE $_;
	  }
	}#while
	# Close filehandles
	close (INFILE);
	close (OUTFILE);

	# Rename result file
	unlink $filename;
	rename $newOutput,$filename;
	
}#fn
sub GenGenModules($)
{
	my $libname = shift @_;
	
		
	my $src = $ATSRoot."/components/";
	my @fileList = ();
	opendir(DIRHANDLE, "$src") || die "Cannot opendir $src";
  foreach my $name (readdir(DIRHANDLE)) 
  {
    if($name =~ /^$libname/)
    {
    	if($libname ne "libc" || $name !~ /^libcrypt/) 
    	{
    		$name =~ s/^$libname//;
    		$name =~ s/^_//;
    		unshift @fileList, $name;
    	}#excluded libcrypt from libc list
    }#libname match
  }#foreach
  closedir(DIRHANDLE);
	return @fileList;
}


sub GetAllModules()
{
	
	my $src = $ATSRoot."/components/";
	my @fileList = ();
	opendir(DIRHANDLE, "$src") || die "Cannot opendir $src";
  foreach my $name (readdir(DIRHANDLE)) 
  {
    	if($name ne "." && $name ne ".." && $name ne "shared")
			{
				unshift @fileList, $name;
			}

  }#foreach
  closedir(DIRHANDLE);
	return @fileList;
}



sub TransformXmls($$)
{
	my $libname = shift @_;
	my $shared = shift @_;
	my $target = shift @_;
	my $module;
	my $src;
	my $dst;

	my $armv5 = 0;
	my $winscw = 0;
	if($target eq "armv5")
	{
			$armv5 = 1;
  }	
  elsif($target eq "winscw")
  {
  		$winscw = 1;
  }
  else
  {
  	$armv5  = $do_armv5;
  	$winscw = $do_winscw;
	}
		

	foreach $module(@generatedModules)
	{
	  if($armv5 == 1 && $winscw == 1)
	  {
			$src = $ATSRoot."/components/".$libname."_"."$module/*.xml";
		}
		elsif($armv5 == 1)
		{
			$src = $ATSRoot."/components/".$libname."_"."$module/*_hw_component.xml";
		}
		elsif($winscw == 1)
		{
			$src = $ATSRoot."/components/".$libname."_"."$module/*_winsspd_component.xml";
		}
		my @fileList = glob($src);
		foreach my $file(@fileList)
		{
		 	#print"$file to  be transformed!\n";
			AddLibsInXml($libname,$file,$shared);
		}
   }
}#fn

sub AddLibsInXml($$$)
{
	my $libname = shift @_;
	my $filename = shift @_;
	my $shared = shift @_;
	
	# Open input file
	open (INFILE, $filename ) || die ("Can not find $filename");

	#Open output file
	my $newOutput = $filename."new";
	open (OUTFILE, ">".$newOutput ) || die ("Can not open $newOutput");

	my $drive = "c:";
	
	# Replace text in files
	while (<INFILE>)
	{
	  if(/\<stif\>/ || /\<execute\>/)
	  {
	  	if(/\<stif\>/)
	  	{
	  		print OUTFILE "\<stif\>\n";
	  	}
	  	else
	  	{
	  		print OUTFILE "\<execute\>\n";
	  		if($filename =~ /winsspd/)
				{
					$drive = "z:";
				}
	  	}
	  	foreach my $installable(@dllList)
			{
				print OUTFILE <<EOLine;
				
	  		<install type=\"$shared binary\">
				<src>$installable</src>
				<dst>$drive\\sys\\bin\\$installable</dst>
 				</install>
EOLine
			}

	  }
	  else
	  {
	  	  print OUTFILE $_;
	  }
	}#while
	# Close filehandles
	close (INFILE);
	close (OUTFILE);

	# Rename result file
	unlink $filename;
	rename $newOutput,$filename;
}

sub CopySharedDlls()
{
  my $libname = shift @_;
  my $ref = shift @_;
  my @ListOfDlls = @$ref;
  my $target = shift @_;
	my $module;
	my $src;
	my $dst;
	
	my $do_armv5 = 0;
	my $do_winscw = 0;
	if($target eq "armv5")
	{
		$do_armv5 = 1;
	}
	elsif($target eq "winscw")
	{
		$do_winscw =  1;
	}
	else
	{
		$do_armv5  = 1;
		$do_winscw = 1;
	}
	
	
	mkdir  $ATSRoot."\\components\\shared" || die("Wouldn't make shared folder in $ATSRoot\\components");
	if($do_winscw == 1)
	{
		mkdir  $ATSRoot."\\components\\shared\\winscw_udeb" || die("Wouldn't make shared folder in $ATSRoot\\components\winscw_udeb");
	}
	if($do_armv5 == 1)
	{
		mkdir  $ATSRoot."\\components\\shared\\armv5_urel" || die("Wouldn't make shared folder in $ATSRoot\\components\armv5_urel");
	}
	
	foreach my $copyDll(@ListOfDlls)
	{
		if($do_winscw == 1)
		{
			$src = $rootDrive."\\epoc32\\release\\winscw\\udeb\\".$copyDll;
			$dst = $ATSRoot."\\components\\shared";
			$dst = $dst."\\winscw_udeb\\";
			copy($src, $dst)  || warn("unable to copy $src to $dst");
		}	
		if($do_armv5 == 1)
		{	
			$src = $rootDrive."\\epoc32\\release\\armv5\\urel\\".$copyDll;
			$dst = $ATSRoot."\\components\\shared";
			$dst = $dst."\\armv5_urel\\";
			copy($src, $dst)  || warn("unable to copy $src to $dst");
		}
	}#foreach
}#fn


sub CopyMultiDlls($)
{
  my $libname = shift @_;
	my $module;
	my $src;
	my $dst;
	foreach my $listed(@generatedModules)
	{
		foreach my $copyDll(@dllList)
		{
			if($do_winscw == 1)
			{
				$src = $rootDrive."\\epoc32\\release\\winscw\\udeb\\".$copyDll;
				$dst = $ATSRoot."\\components\\$libname"."_$listed";
				$dst = $dst."\\winscw_udeb\\";
				copy($src, $dst)  || warn("unable to copy $src to $dst");
			}
			if($do_armv5 == 1)
			{
				$src = $rootDrive."\\epoc32\\release\\armv5\\urel\\".$copyDll;
				$dst = $ATSRoot."\\components\\$libname"."_$listed";
				$dst = $dst."\\armv5_urel\\";
				copy($src, $dst)  || warn("unable to copy $src to $dst");
			}
		}#foreach
	}
}#fn

sub StripModulePrefix($)
{
	my $listed = pop @_;
	my $module = $listed;
	if($dontStrip == 0)
	{
		$module =~ s/^test//;
		$module =~ s/^t//;
		$module =~ s/^libc_//;
		$module =~ s/^libm_//;
  }
  return $module;
  	
}

sub GetLsbModules()
{
	my @lsbList = 
	(
	 "abs",
"acos",
"asctime",
"asin",
"atan",
"atan2",
"atof",
"atoi",
"atol",
"bsearch",
"calloc",
"ceil",
"chdir",
"clearerr",
"close",
"closedir",
"cos",
"cosh",
"creat",
"lsb_ctime",
"lsb_difftime",
"dlclose",
"dlerror",
"dlopen",
"dup",
"dup2",
"exp",
"fabs",
"fclose",
"fcntl_x",
"fdopen",
"feof",
"ferror",
"fflush",
"fgetpos",
"fgets",
"fileno",
"floor",
"fmod",
"fopen",
"fopen_X",
"fprintf",
"fputs",
"fread",
"free",
"freopen",
"freopen_X",
"frexp",
"fseek",
"fsetpos",
"fstat",
"ftell",
"ftok",
"ftok_l",
"fwrite",
"getc",
"getcwd",
"gets",
"lsb_gmtime",
"ldexp",
"link",
"lsb_localtime",
"log",
"log10",
"longjmp",
"lseek",
"malloc",
"memchr_X",
"memcmp_X",
"memcpy_X",
"memmove",
"memset_X",
"mkdir",
"mkfifo",
"lsb_mktime",
"modf",
"msgctl",
"msgget",
"msgsnd",
"open",
"opendir",
"open_x",
"perror",
"pipe",
"pow",
"printf",
"qsort",
"rand",
"read",
"readdir",
"readv_l",
"realloc",
"remove",
"rename",
"rewind",
"rewinddir",
"rmdir",
"scanf",
"scanf_X",
"seekdir",
"semctl",
"semget",
"semop",
"setbuf",
"setjmp",
"shmat",
"shmdt",
"shmget",
"sin",
"sinh",
"sqrt",
"srand",
"stat",
"strcat",
"strchr",
"strcmp",
"strcoll_X",
"strcpy",
"strcspn",
"strerror_X",
"strftime",
"strftime_X",
"strlen",
"strncat",
"strncmp",
"strncpy",
"strpbrk",
"strptime",
"strrchr",
"strspn",
"strstr",
"strtod_X",
"strtok",
"strtol_X",
"strxfrm_X",
"tan",
"tanh",
"telldir",
"telldir_l",
"lsb_time",
"tmpfile",
"tmpnam",
"ungetc",
"unlink",
"utime",
"vfprintf",
"vprintf",
"write",
"writev_l",

	);
  return @lsbList;
}



sub GenBackendTest()
{

	my @datList = 
	(
	["libc", "tlibcbackend"],
	);
	
	mkdir $ATSRoot;
	mkdir $ATSRoot."\\components" || die ("DIED: unable to create component dir");
		
	my $lib;
	my $module;
	my $ext;
	foreach my $ref(@datList)
	{
			my @inarr = @$ref;
			($lib, $module,$ext) = @inarr;
			@generatedModules = ($module);
			GenDirs($lib);
			CopyDlls($lib,".dll");
	}
 #now copy xmls			
 CopyAtsSetup();
 #extra exes
 @generatedModules = ("tlibcbackend");
 CopyDeps("libc","TPopenReadChild.exe");
 CopyDeps("libc","TPopenWriteChild.exe");
			
}#fn

sub CopyAtsSetup()
{
	 my $libname = "libc"; 
	 my @list = (
  				 ["*.xml", "libcbackend", "/../backend/internal/tlibcbackend/group/","/", ""],
  				 ["*.cfg", "libcbackend", "/../backend/internal/tlibcbackend/group/","/General/", ""],
  				 ["*.ini", "libcbackend", "/../backend/internal/tlibcbackend/group/","/General/", ""],
  				 );
 
  my $bDataRef = \@list;  				 
	CopyArbitDatFiles("libc",$bDataRef);  				 

}#fn
sub GetPathPrefix()
{
	 my $srcPrefix = $templateRoot;
	 $srcPrefix =~ /(.*)tsrc.*/;
	 $srcPrefix = $1;
	 return $srcPrefix;
}
	

#dat file copy
sub CopyCryptoDat()
{
	my $libRoot = $templateRoot;
	$libRoot =~ /(.*)internal.*/;
	$libRoot = $1;

 my @list = (
 								 ["libssl", "ssltest", "/tsrc/ssl_test/data/*"],
 								 ["libcrypto", "topenssl", "/tsrc/topenssl/data/*"],
           );

	 my $libname;
	 my $dirname;
	 my $glob;
	foreach my $ref(@list)
	{
			my @inarr = @$ref;
			($libname, $dirname, $glob) = @inarr;
			my $modulename = StripModulePrefix($dirname);
			my	$srcPrefix = GetPathPrefix();
			
			my $src = "$srcPrefix"."$glob";
			
			my $dst = $ATSRoot."\\components\\$libname"."_"."$modulename\\General\\";
  		my @filesFound = glob($src);
  		if(scalar @filesFound == 0)
  		{
  			die("DIED: no Dat file found to copy!");
  		}
  		foreach my $file(@filesFound)
  		{
  			copy($file, $dst) || die("DIED: unable to copy $file to $dst");
  		}
  }#ref processing
}#fn

#Crypto xmls copy
sub CopyCryptoXmls()
{
	  my @list = (
  				 ["*.xml", "libssl_ssltest/", "/tsrc/ssl_test/group/"],
   				 ["*.xml", "libcrypto_crypto_test/", "/tsrc/crypto_test/group/"],
   				 ["*.xml", "libcrypto_openssl/", "/tsrc/topenssl/group/"],   				    				 			 
  				 );

	
	my $fname;
	my $moduleDst;
	my $location;
	
	#now copy the files appropriately
	foreach my $ref(@list)
	{
			my @inarr = @$ref;
			($fname, $moduleDst, $location) = @inarr;
			my $src = GetPathPrefix().$location.$fname;
			my $dst = $ATSRoot."\\components\\$moduleDst";
  		
  		my @filesFound = glob($src);
  		if(scalar @filesFound == 0)
  		{
  			die("DIED: no xml file found to copy!");
  		}
  		foreach my $file(@filesFound)
  		{
  			copy($file, $dst) || die("DIED: unable to copy $file to $dst");
  		}
  	}#ref
  	
  	my @crypto_copyList = ("*.txt", "*cfg", "*.ini");
  	
  	foreach my $item(@crypto_copyList)
  	{
  			$location = "/libcrypto/internal/testapps/crypto_test/group/";
  			$fname = GetPathPrefix().$location.$item;
  			my @filesFound = glob($fname);
  			foreach my $file(@filesFound)
  			{
    			$moduleDst = "libcrypto_crypto_test/";	
  				my $dst = $ATSRoot."\\components\\".$moduleDst."/General/";
  				copy($file, $dst);
  			}
  	}
}#fn

sub GenCryptoTest()
{

	my @datList = 
	(
	["libssl", "ssltest", ".exe"],
	["libcrypto", "crypto_test",".dll"],
	["libcrypto", "openssl",".exe"],
	);
	
	mkdir $ATSRoot;
	mkdir $ATSRoot."\\components" || die ("DIED: unable to create component dir");
		
	my $lib;
	my $module;
	my $ext;
	foreach my $ref(@datList)
	{
			my @inarr = @$ref;
			($lib, $module,$ext) = @inarr;
			@generatedModules = ($module);
			GenDirs($lib);
			CopyDlls($lib,$ext);
	}
	CopyCryptoDat();
	CopyCryptoXmls();
	
			
}#fn


#ngi xml copy
sub CopyNgiXmls()
{
	  my @list = (
   				 ["*.xml", "ngi_sa_amrrecording", "/audio/lowlevelaudio/internal/sa_amrrecording/group/"],
   				 ["*.xml", "ngi_sa_highlevelaudio", "/audio/highlevelaudio/internal/sa_highlevelaudio/group/"],
  				 ["*.xml", "ngi_sa_audiomixing", "/audio/lowlevelaudio/internal/sa_audiomixing/group/"],
  				 ["*.xml", "ngi_sa_audiorecording", "/audio/lowlevelaudio/internal/sa_audiorecording/group/"],
  				 ["*.xml", "ngi_sa_clip", "/audio/lowlevelaudio/internal/sa_clip/group/"],
  				 ["*.xml", "ngi_sa_lowlevelaudio", "/audio/lowlevelaudio/internal/sa_lowlevelaudio/group/"],
  				 ["*.xml", "ngi_sa_stream", "/audio/lowlevelaudio/internal/sa_stream/group/"],
  				 
  				 ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_antitearing/group/"],
  				 ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_colortranslation/group/"],
  				 ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_createfb565/group/"],
  				 ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_createfb888/group/"],
  				 ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_heapusage/group/"],
  				 ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_multibb/group/"],
  				 ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_performance/group/"],
           ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_plswitching/group/"],
           ["*.xml", "ngi_sa_backbuffer", "/graphics/backbuffer/internal/sa_bb_misc/group/"],
  				 ["*.xml", "ngi_sa_bitmap", "/graphics/bitmap/internal/sa_bitmap/group/"],
  				 ["*.xml", "ngi_sa_camera", "/graphics/camera/internal/sa_camera/group/"],
  				 ["*.xml", "ngi_sa_images", "/graphics/images/internal/sa_images/group/"],
  				 ["*.xml", "ngi_sa_display", "/graphics/display/internal/sa_display/group/"],
  				 ["*.xml", "ngi_sa_lights", "/graphics/lights/internal/sa_lights/group/"],
  				 ["*.xml", "ngi_sa_videoplayback", "/graphics/video/internal/sa_videoplayback/group/"],
  				 
  				 ["*.xml", "ngi_sa_input", "/input/input/internal/sa_input/group/"],
  				 ["*.xml", "ngi_sa_textinput", "/input/textinput/internal/sa_textinput/group/"],
  				 ["*.xml", "ngi_sa_devicecapabilities", "/system/devicecapabilities/internal/sa_devicecapabilities/group/"],
  				 ["*.xml", "ngi_sa_devicestatus", "/system/devicestatus/internal/sa_phone/group/"],
  				 ["*.xml", "ngi_sa_runtimetester", "/system/runtime/internal/sa_runtimetester/group/"],
  				 ["*.xml", "ngi_sa_timing", "/timing/internal/sa_timing/group/"],
	  				 );

	
	my $fname;
	my $moduleDst;
	my $location;
	
	#now copy the files appropriately
	foreach my $ref(@list)
	{
			my @inarr = @$ref;
			($fname, $moduleDst, $location) = @inarr;
			my $src = GetPathPrefix().$location.$fname;
			my $dst = $ATSRoot."\\components\\$moduleDst";
			
			#print "My src is $src\n";
			#print "My dst is $dst\n";
			  		
  		my @filesFound = glob($src);
  		if(scalar @filesFound == 0)
  		{
  			warn("warning: no xml file found to copy from $src!");
  		}
  		foreach my $file(@filesFound)
  		{
  			copy($file, $dst) || die("DIED: unable to copy $file to $dst");
  		}
  	}#ref
    	
}#fn


sub GenNgiTest()
{

	my @datList = 
	(
	["ngi", "sa_amrrecording", ".dll"],
	["ngi", "sa_highlevelaudio", ".dll"],
	["ngi", "sa_audiomixing", ".dll"],
	["ngi", "sa_audiorecording", ".dll"],
	["ngi", "sa_clip", ".dll"],
	["ngi", "sa_lowlevelaudio", ".dll"],
	["ngi", "sa_stream", ".dll"],
	["ngi", "sa_bitmap", ".dll"],
	["ngi", "sa_camera", ".dll"],
	["ngi", "sa_images", ".dll"],
	["ngi", "sa_display", ".dll"],
	["ngi", "sa_lights", ".dll"],	
	["ngi", "sa_videoplayback", ".dll"],	
	["ngi", "sa_input", ".dll"],	
	["ngi", "sa_textinput", ".dll"],
	["ngi", "sa_devicecapabilities", ".dll"],	
	["ngi", "sa_devicestatus", ".dll"],	
	["ngi", "sa_runtimetester", ".dll"],	
	["ngi", "sa_timing", ".dll"],	
		
	);
	
	mkdir $ATSRoot;
	mkdir $ATSRoot."\\components" || die ("DIED: unable to create component dir");

		
	my $lib;
	my $module;
	my $ext;
	foreach my $ref(@datList)
	{
			my @inarr = @$ref;
			($lib, $module,$ext) = @inarr;
			@generatedModules = ($module);
			GenDirs($lib);
			CopyDlls($lib,$ext);
	}
	
   @generatedModules =("sa_backbuffer");
   GenDirs($lib);
   @dllList = 
   (
   	"sa_bb_antitearing.dll",
   	"sa_bb_colortranslation.dll",
   	"sa_bb_createfb444.dll",
   	"sa_bb_createfb565.dll",
   	"sa_bb_createfb888.dll",
   	"sa_bb_misc.dll",
   	"sa_bb_heapusage.dll",
   	"sa_bb_multibb.dll",
   	"sa_bb_performance.dll",
   	"sa_bb_plswitch.dll",
   	"sa_bb_testenv.dll"
   );
   CopyMultiDlls("ngi");

	
 	mkdir  $ATSRoot."\\components\\shared" || die("Wouldn't make shared folder in $ATSRoot\\components");
  mkdir  $ATSRoot."\\components\\shared\\winscw_udeb" || die("Wouldn't make shared folder in $ATSRoot\\components\winscw_udeb");
  mkdir  $ATSRoot."\\components\\shared\\armv5_urel" || die("Wouldn't make shared folder in $ATSRoot\\components\armv5_urel");
			
}#fn


sub GetXmls()
{
  my @foundList;
  my $libname = shift @_;
	my @globlist = ();
	if($target eq "winsspd" && $sdkversion ne "")
	{
		@globlist = ("winsspd_".$sdkversion);
		#print "winsspd ".$sdkversion." selected\n";
	}
	elsif($target eq "winsspd")
	{
		@globlist = ("winsspd");
		#print "winsspd selected\n";
	}
	elsif($sdkversion ne "")
	{
		@globlist = ($target,"hw_".$sdkversion);
		#print "$target $sdkversion selected\n";
	}
	else
	{
		@globlist = ($target,"hw");
		#print "$target $sdkversion selected\n";
	}
	
	foreach my $module(@generatedModules)
	{
	  my $glob_to_use = "";
	  my @fileList;
	  foreach my $glob(@globlist)
	  {
			my $src = $ATSRoot."/components/".$libname."_"."$module/*.xml";
			#print "Analysing $module and  $glob\n";
			@fileList = glob($src);
			foreach my $file(@fileList)
			{
			 my $pat = $glob."\_component\.xml";
			 #print "[$file - $glob"."_component.xml"."]\n";
				if($file =~ /$pat/)
				{
					$glob_to_use = $pat;
					#print "[$file - $glob"."_component.xml"."]\n";
					last
				}
			}#foreach FILE
			if($glob_to_use ne "")
			{
				last;
			}
		}#finding glob	
		if($glob_to_use ne "")
		{
				#print "Using the glob $glob_to_use\n";
				my $src = $ATSRoot."/components/".$libname."_"."$module/*.xml";
				@fileList = glob($src);
				foreach my $file(@fileList)
				{
					#print "[$file - $glob_to_use]\n";
					if($file =~ /$glob_to_use/)
					{
				  	$file =~ /.*components[\\\/](.*)/;
				  	my $value = $1;
				  	$value =~ s/\//\\/g;
				  	#print "adding $value for $module with $glob_to_use\n";
						unshift @foundList,$value;
				  }#if
				}#foreach
		}#glob is present
		else
		{
			warn("Unable to figure out glob to use for $module. Skipping...\n");
		}
	}#each module
  return @foundList;
 }#fn

__END__
:end




