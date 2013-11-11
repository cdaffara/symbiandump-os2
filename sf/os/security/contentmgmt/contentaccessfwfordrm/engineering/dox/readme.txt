// Copyright (c) Symbian Software Ltd 2006-2007. All rights reserved.

// Description:		Content Access Framework Doygen Readme file
// Location:		caf2/docs_internal/dox


This folder holds the Doxygen files needed to build the Compiled HTML Help file 
that documents the CAF2 APIs.


// ----------------------------------------------------
// Pre-requisites for building the documentation
// ----------------------------------------------------

Windows PC with:

o DOxygen 1.4.6 or later, 
	http://www.stack.nl/~dimitri/doxygen/
o Graphviz 2.8 or later, 
	http://www.graphviz.org/
o Microsoft HTML Help Workshop 4.74 or later,  
	http://www.microsoft.com/downloads/details.aspx?familyid=00535334-c8a6-452f-9aa0-d597d16580cc&displaylang=en
 

// ----------------------------------------------------
// Build Procedure
// ----------------------------------------------------

This section shows how to create the 'CAF_Documentation.chm' file.
Use the caf2\docs_internal\dox\builddox.bat or the procedure below.

1. Open a command window and change directory to the caf2\docs_internal component folder.
2. Create the 'output' folder with the command: mkdir dox\output
3. Make sure 'CAF_documentation.chm' is not read-only (i.e. Open for Edit in Perforce)
4. Execute:  del /S /F /Q dox\output\*.*
5. Execute:	 doxygen dox\Doxyfile.cfg > doxygen.log 2>&1
6. Review the doxygen.log for errors/warnings that need fixing
7. Clean up and remove the dox.log file and output folder.
8. Documentation is now ready!

TIP: If Doxygen is failing to pick up image files specified with the @image tag, try
emptying the 'output' folder and rebuilding.

TIP: You can also the Doxygen wizard to configure and execute the configuration file.

