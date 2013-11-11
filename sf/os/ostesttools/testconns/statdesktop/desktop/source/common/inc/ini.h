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





#ifndef INI_H
#define INI_H

#include <afxcoll.h>

class CStatIniFile  
{
public:
	CStatIniFile();
	~CStatIniFile();
	void SetIniFileName(const CString& strName);
	BOOL SectionExists(const CString& strSection) const;
	BOOL DeleteSection(const CString& strSection) const;
	BOOL WriteKey(const CString& strValue, const CString& strKey, const CString& strSection) const;
	CString GetKeyValue(const CString& strKey, const CString& strSection) const;
private:
	CString istrIniFileName;
};

#endif // __INI.H__
