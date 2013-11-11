/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Plattest_Platsec_Cleanup.exe
* Deletes the file specified in the command line. 
* Command Line Syntax:
* Plattest_Platsec_Cleanup.exe <Filename>
* <Filename> can be with path or without path. If path is not
* specified, the default path of c:\sys\bin\ will be taken.
*
*/



/**
 @file cleanup.cpp
*/


//Epoc include
#include <f32file.h>
#include <f32image.h>
#include <test/wrapperutilsplugin.h>
#include <test/tefutils.h>

TParse FileName;

_LIT(KDefaultExePath,"?:\\sys\\bin\\");

/**
 Parses the Command Line arguments
 @return TInt - KErrNone if no error else, system wide error codes
*/
TInt ParseCommandLine()
      {
	TDriveName defaultSysDrive(_L("C:"));
	RFs fileServer;
	TVersionName version(fileServer.Version().Name());
	
	RLibrary pluginLibrary;
	CWrapperUtilsPlugin* plugin = TEFUtils::WrapperPluginNew(pluginLibrary);
	
	if (plugin!=NULL)
		{
		TDriveUnit driveUnit(plugin->GetSystemDrive());
		defaultSysDrive.Copy(driveUnit.Name());
		delete plugin;
		pluginLibrary.Close();
		}

	  TBuf<256> c;
      
      User::CommandLine(c);

      TLex l(c);

	  TFileName defaultExePath(KDefaultExePath);
	  defaultExePath.Replace(0, 2, defaultSysDrive);

      if(FileName.SetNoWild(l.NextToken(),0,&defaultExePath)!=KErrNone)
            return KErrArgument;

      // Check we used all the arguments
      if (l.NextToken() != KNullDesC)
            return KErrArgument;

      return KErrNone;
      }


TInt E32Main()
	{
	TInt r;

	//Parses the command line
	r = ParseCommandLine();
	if(r!=KErrNone)
		return r;

	//Connects with FileServer
	RFs Fs;
	r = Fs.Connect();
	if(r!=KErrNone)
		return r;

	//Deletes the File specified
	r= Fs.Delete(FileName.FullName());
	if(r != KErrNone)
		return r;

	//Closes the sesssion with FileServer
	Fs.Close();

	return r;
	}
