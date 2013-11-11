// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// TestContentHandler.h
//


#ifndef __TESTUSERDATA_H__
#define __TESTUSERDATA_H__


class CTestUserData : public MXmlEngUserData
	{
	public:
		RBuf8 str;
	public:
        void Destroy()
			{
			str.Close();
			delete this;
			}
				
		MXmlEngUserData* CloneL()
			{
			return NULL;
			};
		void *ID()
			{
			return NULL;
			}
	};

#endif //__TESTUSERDATA_H__