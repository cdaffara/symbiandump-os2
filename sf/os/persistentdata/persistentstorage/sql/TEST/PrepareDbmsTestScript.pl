# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

$FilePath = "< add_simple_contacts.sql";
$FilePath2 = "> add_simple_contacts_dbms.sql";
open(SRC, $FilePath) or print STDERR "Cannot open $FilePath\n" and exit 1;
create(DEST, $FilePath2) or print STDERR "Cannot open $FilePath2\n" and exit 1;
my $Line;
my $cnt = 0;
while($Line = <SRC>)
{
   if($Line =~ /^'\d\d\d\d-\d{1,2}-\d{1,2} \d{1,2}:\d{1,2}:\d{1,2}',/)
   {
       print "$Line\n";
       print DEST "#07/06/2006#,\n";
       $cnt++;	
   }
   else 
   {
      if($Line =~ /x'.*',/)
      {
          @pair = split(/x/, $Line);
          print @pair[1]."\n";
          print DEST @pair[1]."\n";
          $cnt++;	
      }
      else
      {
          print DEST $Line;
      }
   }
}
close(SRC);
close(DEST);
print "changed lines count: $cnt\n";

