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




#ifndef CSTATMessageCOM_H
#define CSTATMessageCOM_H

#include <mpsdrc.h>
#include <mpsdrc_i.c>
#include <ECENG.H>
#include "CSTATLogFile.h"

class CSTATMessageCOM : public CCmdTarget
{
	DECLARE_DYNCREATE(CSTATMessageCOM)
	
	public :
		CSTATMessageCOM();
		virtual ~CSTATMessageCOM();
		int Connect(CString& platformtype, CSTATLogFile *theLog);
		bool Disconnect();
		HRESULT SendMsg(BYTE id);
		HRESULT SendMsg(BYTE id, VARIANT data);

		//pointers to interfaces - need to be public so that CommandDecoderCOM can access and send the messages
		LPECENGINES m_pIECEngineS;
		LPECMACHINE m_pIECMachine;
		LPECDRIVE m_pIECDrive;
		EDFILEREQ From;
		EDFILEREQ To;
		EDDRIVEREQ Drive;
		
		bool Connected;

	private :	
		DECLARE_INTERFACE_MAP()
			BEGIN_INTERFACE_PART(CommonSink, ICommonSink)
				STDMETHOD(ProcessResponse)(BYTE* Response, DWORD bufsize);
			END_INTERFACE_PART(CommonSink)
		DECLARE_MESSAGE_MAP()

		//non-interface part, called from interface
		STDMETHOD(ProcessResponse)(BYTE* Response, DWORD bufsize);

		void CopyResponseData(BYTE* pResponse, DWORD bufsize);

		//used for processing device response data
		BYTE* m_pResponseData;

		// COM interfaces for EPOC Connect
		ISendRequest* m_pISendReq;
		ICommonSink* m_pICommonSink;		

		CSTATLogFile *pLog;
		bool bExternalInterface;
};

#endif
