#!C:\Perl\bin

&do_c_header;
#&do_cpp_header;



#end of sub calls


sub do_c_header
{
$l1="/**   \
\@file   FILENAME	\
\@internalComponent \
*/";

	$infile = "ls.txt";
	#$logfile = "sample.dosc";
	$cmd = "ls *.h > ls.txt";
	$cmd2 = "rm ls.txt";

	#open(LOGFILE, ">$logfile");	

	# take dir list
	system $cmd;

	#open ls.txt and read lines
	open(INFILE, $infile);	
	@lines=<INFILE>;

	foreach $filename (@lines)	#$filename = @lines[1];
	{
		#print LOGFILE "$filename\n";
		$l2 = "$l1";
		#print LOGFILE "-----------\n";
		#print LOGFILE "$l2\n";
		#print LOGFILE "----\n";
		$l2 =~ s/FILENAME/$filename/; 			#replace FILENAME with header file name
		#print LOGFILE "$l2\n";
		#print LOGFILE "-----------\n";
		$filename =~ s/\.h/.dosc/; 				#rename .h to .dosc
		#print LOGFILE $filename;
		
		open (OUTFILE, ">$filename");	
		print OUTFILE $l2;
		close (OUTFILE);
	}


	#clean the ls.txt file
	close (INFILE);
	system $cmd2;

	#print LOGFILE "$l1";
	#print LOGFILE @lines;
	#close (LOGFILE);
}

sub do_cpp_header
{
$l1="/**   \
\@file   FILENAME	\
\@internalComponent \
*/";

	$infile = "ls.txt";
	#$logfile = "sample.dosc";
	$cmd = "ls | grep -v \\. > ls.txt";
	$cmd2 = "rm ls.txt";

	#open(LOGFILE, ">$logfile");	

	# take dir list
	system $cmd;

	#open ls.txt and read lines
	open(INFILE, $infile);	
	@lines=<INFILE>;

	foreach $filename (@lines)	#$filename = @lines[1];
	{
		#print "$filename\n";  
		$l2 = "$l1";
		$l2 =~ s/FILENAME/$filename/; 			#replace FILENAME with header file name
		#print LOGFILE "-----------\n";
		#print "$l2\n";
		#print LOGFILE "----\n";
		
		#print LOGFILE "$l2\n";
		#print LOGFILE "-----------\n";
		#print LOGFILE $filename;
		$filename =~ s/\n/\.dosc/;
		#print "$filename\n";
		
		open (OUTFILE, ">$filename");	
		print OUTFILE $l2;
		close (OUTFILE);
	}

	#clean the ls.txt file
	close (INFILE);
#	system $cmd2;

	#print LOGFILE "$l1";
	#print LOGFILE @lines;
	#close (LOGFILE);
}







