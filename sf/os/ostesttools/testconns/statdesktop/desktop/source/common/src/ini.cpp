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
*
*/





#include <afxwin.h>

#include "INI.h"
#include <afxcoll.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CStatIniFile::CStatIniFile()
	{
	}
	
CStatIniFile::~CStatIniFile()
	{
	}

// return an empty string if section or key does not exist
CString CStatIniFile::GetKeyValue(const CString& strKey,const CString& strSection) const
{
	TCHAR result[255];
	GetPrivateProfileString((LPCTSTR)strSection,(LPCTSTR)strKey,_T(	""),result, 255, (LPCTSTR)istrIniFileName);	
	CString strResult(result);
	return strResult;
}


BOOL CStatIniFile::WriteKey(const CString& strValue, const CString& strKey, const CString& strSection) const
{
	return WritePrivateProfileString (strSection, strKey, strValue, istrIniFileName);

}
BOOL CStatIniFile::DeleteSection(const CString& strSection) const
{
	return WritePrivateProfileString(strSection,NULL,NULL,istrIniFileName);
}

BOOL CStatIniFile::SectionExists(const CString& strSection) const
{
	TCHAR result[255];	
	long lRetValue = GetPrivateProfileString((LPCTSTR)strSection,NULL,_T(	""),result, 255, (LPCTSTR)istrIniFileName);
	return (lRetValue > 0); //true if section exists
}

void CStatIniFile::SetIniFileName(const CString& strName)
{
	istrIniFileName = strName;
}
