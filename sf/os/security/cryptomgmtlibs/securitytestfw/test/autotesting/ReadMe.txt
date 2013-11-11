Notes on the automatic testing script
-------------------------------------

In order to run, the RunSecurityTests.pl script requires the following:
- Perl 5 or newer installed;
- environment variable ROOTDRIVE defined as the development root drive (e.g.
  "W:\");
- environment variable RELSUBSYSROOT defined as the top-level directory for the
  subsystem being tested; for security subsystem it is usually "security";
- you may also need to modify the path to mainline Zip archives if it changes
  (currently \\lonfiler01\Devbuilds\Symbian_OS_7.0);
- before running runplatformtest.bat don't forget to run
  appinst\tdata\makeall.bat to build the necessary test data filrs and copy them
  to relevant directories.
