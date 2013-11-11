// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file LightLogger.h
 @internalTechnology
*/

#ifndef LIGHTLOGGER_H_
#define LIGHTLOGGER_H_

//************
//LightLogger
//ver. 0.2
//************

#include <f32file.h>
#include <hal.h> //if you are going to use __HIRES_RESOLUTION or __GET_Resolution 
				 //use "hal.lib" in your project


//<LightLogger output file>
#ifdef __WINSCW__
_LIT(KLogFile,"c:\\light_log.txt");
#else
_LIT(KLogFile,"e:\\light_log.txt");
#endif
//</LightLogger output file>


//<const>
_LIT8(KLLNewLine, "\n");
//</const>


//<Record Ids>
/*
 * Record id decribes type of LightLogger record and can be used by external tools to manage logs views
 * or automate timestamp calculations.
 * */
const int gLL_MachineResolutionId = 0;
const int gLL_TimestampHiResId = 1;
const int gLL_TimestampLoResId = 2;
//</Record Ids>


#if defined __LIGHTLOGGER_ENABLED
//This code must be called before other macros. It just create fresh log file. 
/*
if you want to reset file use: __CREATE_LOG(true), otherwise __CREATE_LOG(false)
*/
#define __CREATE_LOG(replace) {TBool r(replace);RFs fs;RFile f;fs.Connect();\
								if(r){f.Replace(fs, KLogFile, EFileWrite);}else{\
									if(f.Open(fs, KLogFile, EFileWrite)==KErrNotFound){\
										f.Replace(fs, KLogFile, EFileWrite);}\
								}f.Close(); fs.Close();}

//Log text object to file
//example: __LOG("ty text")
#define __LOG(x) {TBuf8<512> b;RFs fs;RFile f;b.Zero(); b.Copy(_L(x)); b.Append(KLLNewLine); fs.Connect(); if(f.Open(fs, KLogFile, EFileWrite)==KErrNone){TInt s; f.Size(s); f.Seek(ESeekStart,s); f.Write(b); f.Close();} fs.Close();}

//Log number object to file.
//exaple: TInt i=4; __LOG(i);
#define __LOGNUM(x) {TBuf8<256> b;RFs fs;RFile f;b.Zero(); b.Num(x);b.Append(KLLNewLine); fs.Connect(); if(f.Open(fs, KLogFile, EFileWrite)==KErrNone){TInt s; f.Size(s); f.Seek(ESeekStart,s); f.Write(b); f.Close();} fs.Close();}

//Log buffer to file.
//Example:
/*
 * TBuf<128> b;
 * b.Append(_L("some text"));
 * __LOGBUF(b)
 * */
#define __LOGBUF(x) {TBuf8<512> b;RFs fs;RFile f;b.Copy(x);b.Append(KLLNewLine); fs.Connect(); if(f.Open(fs, KLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); f.Write(b); f.Close();} fs.Close();}

//Log timestamp (low resolution) with additional text object to file .
//Time stamp format: hour:minute:second:microsecond - user text comment
#define __LOGTIMESTAMP(x) {TTime time; time.HomeTime(); TBuf8<256> b; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); \
							TDateTime dt = time.DateTime();\
							b.AppendFormat(_L8("<[%d]> <[%d:%d:%d:%d]> %S"), gLL_TimestampLoResId, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond(), &_L8(x)); b.Append(KLLNewLine);\
							f.Write(b);\
							f.Close();} fs.Close();\
					   }
					   
//Log tick count to file.
#define __LOGTICKCOUNT {_LIT8(KTick,"TickCount: %u"); TBuf8<256> b; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); \
						b.AppendFormat(KTick, User::TickCount()); b.Append(KLLNewLine); f.Write(b); f.Close();} fs.Close();\
					   }
					   
//Log high resolution time stamp to file with additional user comment.
/*
This is the current value of the machine's high resolution timer. 
If a high resolution timer is not available, it uses the millisecond timer instead.
*/
#define __LOGTIMESTAMP_HIRES(x){TBuf8<256> b; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); \
								b.AppendFormat(_L8("<[%d]> <[%u]> %S"), gLL_TimestampHiResId, __GET_HiResTimestamp(), &_L8(x)); b.Append(KLLNewLine);\
								f.Write(b);\
								f.Close();} fs.Close();\
					 	  }
					 	  
//Fast counter resolution
//tick per second
#define __HIRES_RESOLUTION { TInt _tmp_=gLL_MachineResolutionId; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KLogFile, EFileWrite)==KErrNone){TInt s;f.Size(s);f.Seek(ESeekStart,s); \
							 TBuf8<256> b;b.AppendFormat(_L8("<[%d]> <[%d]> Fast timer resolution"), _tmp_, __GET_Resolution());b.Append(KLLNewLine);f.Write(b); f.Close();} fs.Close();\
					 	  }
					 	  
/**This function returns resolution of fast counter
 * tick per second
*/				 	  
inline TUint32 __GET_Resolution()
	{
	TInt freq = 0; 
	HAL::Get(HALData::EFastCounterFrequency, freq);
	return freq;
	}
	
/**This function returns hi resolution counter (timestamp)
 * 
*/
inline TUint32 __GET_HiResTimestamp() {return User::FastCounter();}
	
	
/**Method marker class
 * 
*/
class TMethodMarker
	{
	public:
		TMethodMarker(const TDesC8& aMethod)
			{
			__LOGBUF(aMethod)
			}	
		~TMethodMarker()
			{
			__LOG("}")
			}
	};
	
	
/**This macro is logging entry and exit points from method specified as parameter.
*/
#define __MARK_METHOD(x) TBuf8<128> b; b.AppendFormat(_L8("%S\n{"), &_L8(x)); TMethodMarker m(b);

#else
	
//empty definitions
#define __CREATE_LOG(replace)
#define __LOG(x) 
#define __LOGNUM(x)
#define __LOGBUF(x)
#define __LOGTIMESTAMP(x)
#define __LOGTICKCOUNT   
#define __LOGTIMESTAMP_HIRES(x)
#define __HIRES_RESOLUTION
inline TUint32 __GET_Resolution(){return 0;}
inline TUint32 __GET_HiResTimestamp(){return 0;}
#define __MARK_METHOD(x)

#endif //__LIGHLOGGER_ENABLED
#endif /*LIGHTLOGGER_H_*/


/*Documentation:
 * 
 * Special data format for certain macros (__HIRES_RESOLUTION; __LOGTIMESTAMP; __LOGTIMESTAMP_HIRES):
 * "<[record_id]> <[value]> description>"
 * where:
 * record_id - is an integer value
 * value - is an integer value
 * description - is a string value
*/
