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

#ifndef TE_LIGHTLOGGER_H_
#define TE_LIGHTLOGGER_H_

//************
//test version of LightLogger
//ver. 0.2
//************

#include <f32file.h>
#include <hal.h> //if you are going to use __TEST_HIRES_RESOLUTION or __TEST_GET_Resolution 
				 //use "hal.lib" in your project


//<LightLogger output file>
#ifdef __WINSCW__
_LIT(KTeLogFile,"c:\\test_light_log.txt");
#else
_LIT(KTeLogFile,"e:\\test_light_log.txt");
#endif
//</LightLogger output file>


//<const>
_LIT8(KTeLLNewLine, "\n");
//</const>


//<Record Ids>
/*
 * Record id decribes type of LightLogger record and can be used by external tools to manage logs views
 * or automate timestamp calculations.
 * */
const int gLL_TestMachineResolutionId = 0;
const int gLL_TestTimestampHiResId = 1;
const int gLL_TestTimestampLoResId = 2;
//</Record Ids>


#if defined __TEST_LIGHTLOGGER_ENABLED
//This code must be called before other macros. It just create fresh log file. 
/*
if you want to reset file use: __TEST_CREATE_LOG(true), otherwise __TEST_CREATE_LOG(false)
*/
#define __TEST_CREATE_LOG(replace) {TBool r(replace);RFs fs;RFile f;fs.Connect();\
								if(r){f.Replace(fs, KTeLogFile, EFileWrite);}else{\
									if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNotFound){\
										f.Replace(fs, KTeLogFile, EFileWrite);}\
								}f.Close(); fs.Close();}

//Log text object to file
//example: __TEST_LOG("ty text")
#define __TEST_LOG(x) {TBuf8<512> b;RFs fs;RFile f;b.Zero(); b.Copy(_L(x)); b.Append(KTeLLNewLine); fs.Connect(); if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNone){TInt s; f.Size(s); f.Seek(ESeekStart,s); f.Write(b); f.Close();} fs.Close();}

//Log number object to file.
//exaple: TInt i=4; __TEST_LOG(i);
#define __TEST_LOGNUM(x) {TBuf8<256> b;RFs fs;RFile f;b.Zero(); b.Num(x);b.Append(KTeLLNewLine); fs.Connect(); if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNone){TInt s; f.Size(s); f.Seek(ESeekStart,s); f.Write(b); f.Close();} fs.Close();}

//Log buffer to file.
//Example:
/*
 * TBuf<128> b;
 * b.Append(_L("some text"));
 * __TEST_LOGBUF(b)
 * */
#define __TEST_LOGBUF(x) {TBuf8<512> b;RFs fs;RFile f;b.Copy(x);b.Append(KTeLLNewLine); fs.Connect(); if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); f.Write(b); f.Close();} fs.Close();}

//Log timestamp (low resolution) with additional text object to file .
//Time stamp format: hour:minute:second:microsecond - user text comment
#define __TEST_LOGTIMESTAMP(x) {TTime time; time.HomeTime(); TBuf8<256> b; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); \
							TDateTime dt = time.DateTime();\
							b.AppendFormat(_L8("<[%d]> <[%d:%d:%d:%d]> %S"), gLL_TestTimestampLoResId, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond(), &_L8(x)); b.Append(KTeLLNewLine);\
							f.Write(b);\
							f.Close();} fs.Close();\
					   }
					   
//Log tick count to file.
#define __TEST_LOGTICKCOUNT {_LIT8(KTick,"TickCount: %u"); TBuf8<256> b; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); \
						b.AppendFormat(KTick, User::TickCount()); b.Append(KTeLLNewLine); f.Write(b); f.Close();} fs.Close();\
					   }
					   
//Log high resolution time stamp to file with additional user comment.
/*
This is the current value of the machine's high resolution timer. 
If a high resolution timer is not available, it uses the millisecond timer instead.
*/
#define __TEST_LOGTIMESTAMP_HIRES(x){TBuf8<256> b; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNone){ TInt s; f.Size(s); f.Seek(ESeekStart,s); \
								b.AppendFormat(_L8("<[%d]> <[%u]> %S"), gLL_TestTimestampHiResId, __TEST_GET_HiResTimestamp(), &_L8(x)); b.Append(KTeLLNewLine);\
								f.Write(b);\
								f.Close();} fs.Close();\
					 	  }
					 	  
//Fast counter resolution
//tick per second
#define __TEST_HIRES_RESOLUTION { TInt _tmp_=gLL_TestMachineResolutionId; RFs fs; RFile f; fs.Connect(); if(f.Open(fs, KTeLogFile, EFileWrite)==KErrNone){TInt s;f.Size(s);f.Seek(ESeekStart,s); \
							 TBuf8<256> b;b.AppendFormat(_L8("<[%d]> <[%d]> Fast timer resolution"), _tmp_, __TEST_GET_Resolution());b.Append(KTeLLNewLine);f.Write(b); f.Close();} fs.Close();\
					 	  }
					 	  
/**This function returns resolution of fast counter
 * tick per second
*/				 	  
inline TUint32 __TEST_GET_Resolution()
	{
	TInt freq = 0; 
	HAL::Get(HALData::EFastCounterFrequency, freq);
	return freq;
	}
	
/**This function returns hi resolution counter (timestamp)
 * 
*/
inline TUint32 __TEST_GET_HiResTimestamp() {return User::FastCounter();}
	
	
/**Method marker class
 * 
*/
class TTestMethodMarker
	{
	public:
		TTestMethodMarker(const TDesC8& aMethod)
			{
			__TEST_LOGBUF(aMethod)
			}	
		~TTestMethodMarker()
			{
			__TEST_LOG("}")
			}
	};
	
	
/**This macro is logging entry and exit points from method specified as parameter.
*/
#define __TEST_MARK_METHOD(x) TBuf8<128> b; b.AppendFormat(_L8("%S\n{"), &_L8(x)); TTestMethodMarker m(b);

#else
	
//empty definitions
#define __TEST_CREATE_LOG(replace)
#define __TEST_LOG(x) 
#define __TEST_LOGNUM(x)
#define __TEST_LOGBUF(x)
#define __TEST_LOGTIMESTAMP(x)
#define __TEST_LOGTICKCOUNT   
#define __TEST_LOGTIMESTAMP_HIRES(x)
#define __TEST_HIRES_RESOLUTION
inline TUint32 __TEST_GET_Resolution(){return 0;}
inline TUint32 __TEST_GET_HiResTimestamp(){return 0;}
#define __TEST_MARK_METHOD(x)

#endif //__TEST_LIGHLOGGER_ENABLED
#endif /*TE_LIGHTLOGGER_H_*/


/*Documentation:
 * 
 * Special data format for certain macros (__TEST_HIRES_RESOLUTION; __TEST_LOGTIMESTAMP; __TEST_LOGTIMESTAMP_HIRES):
 * "<[record_id]> <[value]> description>"
 * where:
 * record_id - is an integer value
 * value - is an integer value
 * description - is a string value
*/
