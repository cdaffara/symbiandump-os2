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


#include "../inc/CATMmp.h"

CATMmp::CATMmp()
{
	LOG_FUNC_ENTRY("CATMmp::CATMmp");
}

CATMmp::~CATMmp()
{
	LOG_FUNC_ENTRY("CATMmp::~CATMmp");
}

// -----------------------------------------------------------------------------
// CATMmp::IsMmpEdited
// Checks is the file edited by AT
// -----------------------------------------------------------------------------
bool CATMmp::IsMmpEdited( bool bBackup)
{
	LOG_FUNC_ENTRY("CATMmp::IsMmpEdited");
	// Stream to read file
	ifstream in;
	// Temp char array to read line
	char cTemp[MAX_LINE_LENGTH];
	// Open file
	if ( bBackup )
		in.open( CreateMmpBackupPath().c_str() );
	else
		in.open( m_sMmpFile.c_str() );
	// Is file open ok
	if( ! in.good() )
	{
		cout << AT_MSG << "Error, can not open file "
			<< m_sMmpFile << endl;
		in.close();
		return false;
	}
	// Search edit start line
	bool bEdited=false;
	const char* cFind = MMPFILECHANGES[0].c_str();
	while( ! bEdited && in.good() )
	{
		// Get line
		in.getline( cTemp, MAX_LINE_LENGTH );
		// Compare to first line in changes
		if ( strstr( cTemp, cFind) )
		{
			bEdited = true;
			// Stop looking any further
			break;
		}
	}
	// Close file and return result
	in.close();
	return bEdited;
}

// -----------------------------------------------------------------------------
// CATMmp::EditMmpFile
// Makes AnalyzeTool changes to given mmp file
// -----------------------------------------------------------------------------
bool CATMmp::EditMmpFile(const string& sTargetType, const string& sId)
{
	LOG_FUNC_ENTRY("CATMmp::EditMmpFile");

	if ( ! RemoveWriteProtections() )
		return false;

	// Stream where to add changes
	ofstream out;

	// Open mmp file for editing (append changes to the end)
	out.open( m_sMmpFile.c_str(), ios::out | ios::app );

	// File open ok?
	if( !out.good() )
	{
		cout << AT_MSG << "Error, can not open file "
			<< m_sMmpFile;
		out.close();
		return false;
	}
	
	// Write lines to mmp file
	if ( sTargetType.compare( "dll" ) == 0 || sTargetType.compare( "lib" ) == 0 )
	{
		// DLL changes
		int size = sizeof( MMPFILECHANGES_DLL ) / sizeof( string );
		for( int i = 0; i < size; i++ )
		{
			out << endl << MMPFILECHANGES_DLL[i];
		}
		out << endl;
	}
	else
	{
		// Other than DLL changes
		int size = sizeof( MMPFILECHANGES ) / sizeof( string );
		for( int i = 0; i < size; i++ )
		{
			// After second line of changes add also source statement
			out << endl << MMPFILECHANGES[i];
			if ( i == 1 )
			{
				out << endl
					<< "SOURCE            "
					<< AT_TEMP_CPP_LOWER_START
					<< sId
					<< AT_TEMP_CPP_LOWER_END;
			}
		}
		out << endl;
	}
	// Close stream
	out.close();

	cout << AT_MSG << "Mmp file : " << m_sMmpFile << " edited." << endl;

	return true;
}

// -----------------------------------------------------------------------------
// CATMmp::BackupMmpFile
// Backups the mmp file to path/atool_temp/filename.mmp.tmp
// Calling this function results always to 
// - none edited mmp
// - none edited backup
// - If mmp is write protected. Create writable copy from it. Backup the write
// procted one.
// -----------------------------------------------------------------------------
bool CATMmp::BackupMmpFile( )
{
	LOG_FUNC_ENTRY("CATMmp::BackupMmpFile");

	if ( ! RemoveWriteProtections() )
		return false;

	// Backup path+filename
	string sBackup = CreateMmpBackupPath();

	// Backup mmp.
	if ( CopyFile( m_sMmpFile.c_str() , sBackup.c_str(), false ) == 0 )
	{
		// Log and return false if failed to copy file
		LOG_STRING( "error copyfile " << m_sMmpFile << " to " << sBackup );
		return false;
	}

	// If backup now edited remove changes from it.
	if ( IsMmpEdited( true ) )
	{
		if ( ! RemoveMmpFileChanges( true ) )
			return false;
	}

	return true;
}
// -----------------------------------------------------------------------------
// CATMmp::RestoreMmpFile
// Restores the mmp file from backup
// -----------------------------------------------------------------------------
bool CATMmp::RestoreMmpFile()
{
	LOG_FUNC_ENTRY("CATMmp::RestoreMmpFile");
	
	if ( ! RemoveWriteProtections() )
		return false;

	if ( CopyFile( CreateMmpBackupPath().c_str() , m_sMmpFile.c_str(), false ) == 0 )
	{
		// Log and return false if failed to copy file
		LOG_STRING("error copyfile " << CreateMmpBackupPath() << " to " << m_sMmpFile );
		return false;
	}
	else
		cout << AT_MSG << "Mmp file : " << m_sMmpFile << " restored." << endl;
	return true;
}

// -----------------------------------------------------------------------------
// CATMmp::RemoveMmpFileChanges
// Removes AT changes from given mmp file
// -----------------------------------------------------------------------------
bool CATMmp::RemoveMmpFileChanges(bool bBackup)
{
	LOG_FUNC_ENTRY("CATMmp::RemoveMmpFileChanges");
	
	if ( ! RemoveWriteProtections() )
		return false;

	// File reading stream
	ifstream in;
	// Vector to hold file data
	vector<string> vLines;
	// Open file
	if ( bBackup )
		in.open( CreateMmpBackupPath().c_str(), ios::in );
	else
		in.open( m_sMmpFile.c_str(), ios::in );
	// Check file open ok
	if ( ! in.good() )
	{
		cout << AT_MSG << "Error, opening file";
		if ( bBackup )
			cout << CreateMmpBackupPath();
		else
			cout << m_sMmpFile;
		cout << endl;
		return false;
	}
	// Read file to temporary stream except AT changes
	char cLine[MAX_LINE_LENGTH];
	// Boolean to know read or not
	bool bRead = true;
	// Integer to confirm that AT changes were succefully found and
	// not read even if they are found multiple times
	int iSuccessfull = 0;
	// Number of 'lines' in mmp changes
	int iChangesSize = sizeof( MMPFILECHANGES ) / sizeof( string );
	// First mmp changes line
	string sFirstLine = MMPFILECHANGES[0];
	// Last mmp changes line
	string sLastLine = MMPFILECHANGES[iChangesSize-1];
	while( in.good() )
	{
		in.getline( cLine, MAX_LINE_LENGTH );
		// Check start of AT changes
		if( strstr( cLine, sFirstLine.c_str() ) != 0 )
		{
			// Remove last linefeed
			vector<string>::iterator it = vLines.end();
			it--;
			if ( it->size() == 0 ) 
				vLines.erase( vLines.end()-1, vLines.end() );
			// Stop reading
			bRead = false;
			iSuccessfull+=3;
		}
		// Read lines outside AT changes
		if ( bRead )
		{
			// Gather all other lines except the AT edits
			vLines.push_back( string(cLine) );
		}
		// Check end of AT changes
		if( strstr( cLine, sLastLine.c_str() ) != 0 )
		{
			// Get empty line
			in.getline( cLine, MAX_LINE_LENGTH );
			// Continue reading
			bRead = true;
			iSuccessfull-=1;
		}
	}
	// Close reading file stream
	in.close();
	// To check all went ok iSuccesfull%2 = 0
	if ( iSuccessfull%2 != 0 && iSuccessfull >= 2 )
	{
		cout << AT_MSG << "Error, removing mmp changes from ";
		if ( bBackup )
			cout << CreateMmpBackupPath();
		else
			cout << m_sMmpFile;
		cout << endl;
		return false;
	}
	// Overwrite current mmp file
	ofstream out;
	// Open file (truncates old data)
	if ( bBackup )
		out.open( CreateMmpBackupPath().c_str(), ios::trunc );
	else
		out.open( m_sMmpFile.c_str(), ios::trunc );
	// Is open ok
	if( ! out.good() )
	{
		cout << AT_MSG << "Error, opening file ";
		if ( bBackup )
			cout << CreateMmpBackupPath();
		else
			cout << m_sMmpFile;
		cout << endl;
	}
	// Write lines to file
	for( vector<string>::iterator it = vLines.begin() ; it != vLines.end() ; it++ )
	{
		out << *it << endl;
	}
	// Close
	out.close();
	// Return true
	return true;
}
// -----------------------------------------------------------------------------
// CATMmp::VerifyAndRecover
// Wont change mmp if it is not edited
// Replaces mmp file using backup if it exists and it is
// not edited otherwise removes changes from mmp file.
// -----------------------------------------------------------------------------
bool CATMmp::VerifyAndRecover()
{
	LOG_FUNC_ENTRY("CATMmp::VerifyAndRecover");
	// Is it edited
	if ( IsMmpEdited() )
	{
		string sBackup = CreateMmpBackupPath();
		if ( FileExists( sBackup.c_str() ) )
		{
			// Is backup edited
			if ( ! IsMmpEdited( true ) )
			{
				// Replace original with backup
				return RestoreMmpFile();
			}
		}
		// Remove changes from original
		return RemoveMmpFileChanges();
	}
	// Non edited original
	return true;
}

// -----------------------------------------------------------------------------
// CATMmp::CreateMmpBackupPath
// Creates string containing full path to backup mmp file
// -----------------------------------------------------------------------------
string CATMmp::CreateMmpBackupPath()
{
	LOG_FUNC_ENTRY("CATMmp::CreateMmpBackupPath");
	// backup path+filename
	string sBackup;
	sBackup.append( GetPathOrFileName( false, m_sMmpFile ) );
	sBackup.append( AT_TEMP_DIR );
	sBackup.append( "\\" );
	// Add mmp file name to it and .tmp
	sBackup.append( GetPathOrFileName( true, m_sMmpFile ) );
	// Add .tmp
	sBackup.append( ".tmp" );
	// Return it
	return sBackup;
}

// -----------------------------------------------------------------------------
// CATMmp::RemoveWriteProtections
// Removes write protection of mmp file and backup if exists.
// -----------------------------------------------------------------------------
bool CATMmp::RemoveWriteProtections()
{
	LOG_LOW_FUNC_ENTRY("CATMmp::RemoveWriteProtections");

	// Backup path+filename
	string sBackup = CreateMmpBackupPath();

	// Check is mmp read-only
	if ( IsFileReadOnly( m_sMmpFile.c_str() ) )
	{
		if( ! SetFileWritable( m_sMmpFile.c_str() ) )
		{
			LOG_STRING( "error setting mmp file writable" << m_sMmpFile );
			return false;
		}
	}

	// Check is there a backup if is remove any write protection from it.
	if ( FileExists( sBackup.c_str() ) )
	{
		if ( IsFileReadOnly( sBackup.c_str() ) )
		{
			if( ! SetFileWritable( sBackup.c_str() ) )
			{
				LOG_STRING( "error setting mmp file writable" << sBackup );
				return false;
			}
		}
	}
	return true;
}
// End of file
