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
* Description: 
*
*/


#include <e32std.h>
TTime NowTime;
TTime NowTime1;

extern "C"
{
void start_time()
{
	NowTime.HomeTime();
}

void end_time()
{
	NowTime1.HomeTime();
}

long time_diff()
{
	TTimeIntervalMicroSeconds micro_sec;
	TInt64 diff_int64;
	micro_sec = NowTime1.MicroSecondsFrom(NowTime);
	diff_int64 = micro_sec.Int64();
	return diff_int64;
}
}
