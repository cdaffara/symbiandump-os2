// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __SCHLOGGER_H__
#define __SCHLOGGER_H__

#ifdef __SCHLOGGING__
	#pragma message(__FILE__ ": (15) WARNING! Schedule logging is enabled")
	
	// System includes
	#include <f32file.h>
	
	/** 
	Literal constants
	@internalComponent
	*/
	_LIT(KSchSvrLogFile,	"SCHSVR.TXT");
	_LIT(KSchSvrLogFolder,	"SCHSVR");

	//
	// ------> CSheduleServerLog (header)
	//
	class CSheduleServerLog : public CBase
	/** 
	@internalComponent
	*/
		{
	//
	public: // CONSTRUCT / DESTRUCT
	//
		static CSheduleServerLog* NewL(const TDesC& aLogFileName);
		~CSheduleServerLog();

	//
	private: // INTERNAL CONSTRUCT
	//
		CSheduleServerLog();
		void ConstructL(const TDesC& aLogFileName);

	//
	public: // ACCESS
	//
		void Log(TRefByValue<const TDesC> aFmt,...);
		void LogList(TRefByValue<const TDesC> aFmt, VA_LIST aList);
		void SeekEnd();

	//
	private: // INTERNAL
	//
		void Write(const TDesC& aText);
		void Write(const TDesC& aFmt, VA_LIST& aList);
		void WriteWithTimeStamp(const TDesC& aText);
		void NewLine();

	//
	private: // MEMBER DATA
	//
		RFs iFs;
		RFile iFile;
	};


	/** 
	Defines
	@internalComponent
	*/
	#define LOGSTRING(AAA) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(tempLogDes()); }
	#define LOGSTRING2(AAA,BBB) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB); }
	#define LOGSTRING3(AAA,BBB,CCC) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); }
	#define LOGSTRING4(AAA,BBB,CCC,DDD) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD); }
	#define LOGSTRING5(AAA,BBB,CCC,DDD,EEE) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD,EEE); }
	#define LOGSTRING6(AAA,BBB,CCC,DDD,EEE,FFF) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD,EEE,FFF); }
	#define LOGSTRING7(AAA,BBB,CCC,DDD,EEE,FFF,GGG) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD,EEE,FFF,GGG); }
	#define LOGSTRING8(AAA,BBB,CCC,DDD,EEE,FFF,GGG,HHH) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD,EEE,FFF,GGG,HHH); }
	#define LOGSTRING9(AAA,BBB,CCC,DDD,EEE,FFF,GGG,HHH,III) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD,EEE,FFF,GGG,HHH,III); }
	#define LOGSTRING10(AAA,BBB,CCC,DDD,EEE,FFF,GGG,HHH,III, JJJ) \
		{ _LIT(tempLogDes, AAA); reinterpret_cast<CSheduleServerLog*>(Dll::Tls())->Log(TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD,EEE,FFF,GGG,HHH,III,JJJ); }


#else

	// Defines
	#define LOGSTRING(AAA)
	#define LOGSTRING2(AAA,BBB)
	#define LOGSTRING3(AAA,BBB,CCC)
	#define LOGSTRING4(AAA,BBB,CCC,DDD)
	#define LOGSTRING5(AAA,BBB,CCC,DDD,EEE)
	#define LOGSTRING6(AAA,BBB,CCC,DDD,EEE,FFF)
	#define LOGSTRING7(AAA,BBB,CCC,DDD,EEE,FFF,GGG)
	#define LOGSTRING8(AAA,BBB,CCC,DDD,EEE,FFF,GGG,HHH)
	#define LOGSTRING9(AAA,BBB,CCC,DDD,EEE,FFF,GGG,HHH,III)
	#define LOGSTRING10(AAA,BBB,CCC,DDD,EEE,FFF,GGG,HHH,III, JJJ)

#endif

#endif
