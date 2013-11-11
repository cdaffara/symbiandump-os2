/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class responsible of handling mmp files.
*
*/


#ifndef __CATMMP_H__
#define __CATMMP_H__

// Includes
#include "ATCommonDefines.h"
#include "CATBase.h"

/**
* CATMmp represents mmp file of module.
* All mmp file related actions are done using this class.
* @author
*/
class CATMmp : public CATBase
{

public:

	// Constructor
	CATMmp();
	
	// Destructor
	~CATMmp();
	
	string m_sMmpFile; /** The mmp file with path */

	/**
	* Makes AnalyzeTool changes to given mmp file.
	* @param sTargetType target type of module i.e. exe / dll
	* @param sId unique identification string added to source file name
	* @return true if editing was successful.
	*/
	bool EditMmpFile(const string& sTargetType, const string& sId);

	/**
	* Backups mmp file to atool_temp/file
	* to path/atool_temp/filename.mmp.tmp.
	* Calling this function results always to 
	* none edited mmp & none edited backup.
	* @return true if successful.
	*/
	bool BackupMmpFile();

	/**
	* Restores backup from temporary directory (replacing current).
	* @return true if successful.
	*/
	bool RestoreMmpFile();

	/**
	* VerifyAndRecover mmp file.
	* Wont change mmp if it is not edited
	* Replaces mmp file using backup if it exists and it is
	* not edited otherwise removes changes from mmp file.
	* @return true if successful
	*/
	bool VerifyAndRecover();

#ifndef MODULE_TEST
private:
#endif
		/**
	* Checks if the given mmp file is modified by AT
	* @param bBackup if true checks backup
	* @return bool true if it is edited
	*/
	bool IsMmpEdited(bool bBackup = false );

	/**
	* CreateBackupPath creates string containinig
	* full path to backup mmp file
	*/
	string CreateMmpBackupPath();

	/**
	* Removes AnalyzeTool changes from given file
	* @param bBackup if true removes changes from backup
	* @return true if successfully removed changes
	*/
	bool RemoveMmpFileChanges(bool bBackup = false);

	/**
	* Removes write protections from mmp file and its backup if exists.
	* @return true if successful.
	*/
	bool RemoveWriteProtections();

};
#endif