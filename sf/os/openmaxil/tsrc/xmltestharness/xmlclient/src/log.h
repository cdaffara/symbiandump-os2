/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LOG_H_
#define LOG_H_

#include "omx_xml_script.h"

#define __FILE8__                                   REINTERPRET_CAST(const TText8*, __FILE__)

#define INFO_PRINTF1(p1)                            LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1)) 
#define INFO_PRINTF2(p1, p2)                        LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2)) 
#define INFO_PRINTF3(p1, p2, p3)                    LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2), (p3)) 
#define INFO_PRINTF4(p1, p2, p3, p4)                LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2), (p3), (p4)) 
#define INFO_PRINTF5(p1, p2, p3, p4, p5)            LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2), (p3), (p4), (p5)) 
#define INFO_PRINTF6(p1, p2, p3, p4, p5, p6)        LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define INFO_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 
#define INFO_PRINTF8(p1, p2, p3, p4, p5, p6, p7, p8)        LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8)) 
#define INFO_PRINTF9(p1, p2, p3, p4, p5, p6, p7, p8, p9)    LogExtra(__FILE8__, __LINE__, EOmxScriptSevInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9)) 

#define WARN_PRINTF1(p1)                            LogExtra(__FILE8__, __LINE__, EOmxScriptSevWarn, (p1)) 
#define WARN_PRINTF2(p1, p2)                        LogExtra(__FILE8__, __LINE__, EOmxScriptSevWarn, (p1), (p2)) 
#define WARN_PRINTF3(p1, p2, p3)                    LogExtra(__FILE8__, __LINE__, EOmxScriptSevWarn, (p1), (p2), (p3)) 
#define WARN_PRINTF4(p1, p2, p3, p4)                LogExtra(__FILE8__, __LINE__, EOmxScriptSevWarn, (p1), (p2), (p3), (p4)) 
#define WARN_PRINTF5(p1, p2, p3, p4, p5)            LogExtra(__FILE8__, __LINE__, EOmxScriptSevWarn, (p1), (p2), (p3), (p4), (p5)) 
#define WARN_PRINTF6(p1, p2, p3, p4, p5, p6)        LogExtra(__FILE8__, __LINE__, EOmxScriptSevWarn, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define WARN_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    LogExtra(__FILE8__, __LINE__, EOmxScriptSevWarn, (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 

#define ERR_PRINTF1(p1)                             LogExtra(__FILE8__, __LINE__, EOmxScriptSevErr, (p1)) 
#define ERR_PRINTF2(p1, p2)                         LogExtra(__FILE8__, __LINE__, EOmxScriptSevErr, (p1), (p2)) 
#define ERR_PRINTF3(p1, p2, p3)                     LogExtra(__FILE8__, __LINE__, EOmxScriptSevErr, (p1), (p2), (p3)) ;
#define ERR_PRINTF4(p1, p2, p3, p4)                 LogExtra(__FILE8__, __LINE__, EOmxScriptSevErr, (p1), (p2), (p3), (p4)) 
#define ERR_PRINTF5(p1, p2, p3, p4, p5)             LogExtra(__FILE8__, __LINE__, EOmxScriptSevErr, (p1), (p2), (p3), (p4), (p5)) 
#define ERR_PRINTF6(p1, p2, p3, p4, p5, p6)         LogExtra(__FILE8__, __LINE__, EOmxScriptSevErr, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define ERR_PRINTF7(p1, p2, p3, p4, p5, p6, p7)     LogExtra(__FILE8__, __LINE__, EOmxScriptSevErr, (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 

#endif /* LOG_H_ */
