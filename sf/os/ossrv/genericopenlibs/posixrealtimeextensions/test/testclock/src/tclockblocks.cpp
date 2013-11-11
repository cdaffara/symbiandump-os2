// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tclockblocks.cpp
// Test cases for blocking signal api's
// 
//

#include "tclock.h"

// -----------------------------------------------------------------------------
// CTestclock::Testgetclockid1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getcpuclockid()
// Description: To access the clock id of CPU time clock with pid = 0. API tested: clock_getcpuclockid()
// -----------------------------------------------------------------------------

TInt CTestclock::Testgetclockid1 (  )
	{
	int ret, ret1 = KErrGeneral;
	clockid_t clockid;
	ret = clock_getcpuclockid(0,&clockid);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to retrieve the clock id of the calling process and errno is %d"),errno);
	 	goto close;	
		}
	if (clockid != CLOCK_REALTIME)
		{
	 	ERR_PRINTF1(_L("Failed to return the right clock id"));
	 	goto close;	
		}
	INFO_PRINTF1(_L("Successfully able to get the calling process's clock id") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testgetclockid2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getcpuclockid()
// Description:  To access the clock id of CPU time clock of self. API tested: clock_getcpuclockid() 
// -----------------------------------------------------------------------------

TInt CTestclock::Testgetclockid2 (  )
	{
	int ret, ret1 = KErrGeneral;
	clockid_t clockid;
	ret = clock_getcpuclockid(getpid(),&clockid);
	if ((ret != -1) || (errno !=  ESRCH))
		{
	 	ERR_PRINTF2(_L("clock_getcpuclockid() failed on negative test and errno is %d"),errno);
	 	goto close;	
		}
	INFO_PRINTF1(_L("clock_getcpuclockid() is successful on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestclock::Testgetclockid3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getcpuclockid()
// Description: To access the clock id of CPU time clock of other process 
// -----------------------------------------------------------------------------

TInt CTestclock::Testgetclockid3 (  )
	{
	int ret, ret1 = KErrGeneral;
	pid_t pid;
	clockid_t clockid;
	char **argv = (char **)malloc(2*sizeof(char*));
	argv[0] = (char *)malloc(30*sizeof(char*));
	argv[1] = 0;
	strcpy(argv[0],"z:\\sys\\bin\\getclockid.exe");
	ret = posix_spawn(&pid, "z:\\sys\\bin\\getclockid.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	ret = clock_getcpuclockid(pid,&clockid);
	if ((ret != -1) || (errno !=  ESRCH))
		{
	 	ERR_PRINTF2(_L("clock_getcpuclockid() failed on negative test and errno is %d"),errno);
	 	goto close;	
		}
	INFO_PRINTF1(_L("clock_getcpuclockid() is successful on negative test") );
	ret1 = KErrNone;
	
	close:
	free((void*)argv[0]);
	free((void*)argv);
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testgetclockid4
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getcpuclockid()
// Description: To access the clock id of CPU time clock of an invalid process. 
// -----------------------------------------------------------------------------

TInt CTestclock::Testgetclockid4 (  )
	{
	int ret, ret1 = KErrGeneral;
	clockid_t clockid;
	ret = clock_getcpuclockid(-1,&clockid);
	if ((ret != -1) || (errno !=  ESRCH))
		{
	 	ERR_PRINTF2(_L("clock_getcpuclockid() failed on negative test and errno is %d"),errno);
	 	goto close;	
		}
	INFO_PRINTF1(_L("clock_getcpuclockid() has successfully returned ESRCH on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTestclock::Testgetclockid5
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getcpuclockid()
// Description: Trying to access the clock id of an invalid id passing to it.
// -----------------------------------------------------------------------------

TInt CTestclock::Testgetclockid5 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = clock_getcpuclockid(0,NULL);
	if ((ret != -1) || (errno !=  EFAULT))
		{
	 	ERR_PRINTF2(_L("clock_getcpuclockid() failed on negative test and errno is %d"),errno);
	 	goto close;	
		}
	INFO_PRINTF1(_L("clock_getcpuclockid() has successfully returned EFAULT on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;			
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclockresolution1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getres()
// Description: To get the clock resolution with valid clock id using clock_getres() 
// -----------------------------------------------------------------------------

TInt CTestclock::Testclockresolution1 (  )
	{
	int ret, ret1 = KErrGeneral;
	clockid_t clockid;
	struct timespec tmspec;
	ret = clock_getcpuclockid(0,&clockid);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to retrieve the clock id of the calling process and errno is %d"),errno);
	 	goto close;	
		}
	ret = clock_getres(clockid,&tmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to retrieve resolution of the clock id specified and errno is %d"),errno);
	 	goto close;	
		}	
	if ((tmspec.tv_nsec != 1000) || (tmspec.tv_sec != 0))
		{
	 	ERR_PRINTF1(_L("Resolution of the clock id is not set properly"));
	 	goto close;			
		}
	INFO_PRINTF1(_L("Successfully able to get the clock resolution of the specified clock id") );
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclockresolution2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getres()
// Description: Trying to get the clock resolution using clock_getres() for a clockid other than CLOCK_REALTIME 
// -----------------------------------------------------------------------------

TInt CTestclock::Testclockresolution2 (  )
	{
	int ret, ret1 = KErrGeneral, Clockid, Error;
	struct timespec tmspec;
	ret = GetIntFromConfig(ConfigSection(), _L("Clockid"), Clockid); 
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Error"), Error); 
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read expected error num")) ;
	 	goto close;
	  	}
	ret = clock_getres(Clockid,&tmspec);
	if ((ret != -1) || (errno != Error))
		{
	 	ERR_PRINTF2(_L("The expected and errno are not same and errno is %d"),errno);
	 	goto close;			
		}
	INFO_PRINTF1(_L("The output and expected value are same for clock_getres()") );
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclockresolution3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_getres()
// Description: Negative Test: Trying to get the resolution with res = NULL using clock_getres() 
// -----------------------------------------------------------------------------

TInt CTestclock::Testclockresolution3 (  )
	{
	int ret, ret1 = KErrGeneral, Clockid;
	ret = GetIntFromConfig(ConfigSection(), _L("Clockid"), Clockid); 
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	ret = clock_getres(Clockid,NULL);
	if (ret != 0) 
		{
	 	ERR_PRINTF2(_L("clock_getres() failed on negative test if timespec argument is NULL and errno is %d"),errno);
	 	goto close;	
		}	
	INFO_PRINTF1(_L("clock_getres() successfully returned 0 on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocknanosleep1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_nanosleep()
// Description: Trying to suspend the process for the specified time using clock_nanosleep() in the absence of TIMER_ABSTIME
// Relative timer
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocknanosleep1 (  )
	{
	int ret, ret1 = KErrGeneral, Valuesec, Valuenanosec;
	struct timespec gettmspec, oldtmpsec, sleeptmspec, sleeptmspec1;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	goto close;
	  	}
	sleeptmspec.tv_sec = Valuesec;
	sleeptmspec.tv_nsec = Valuenanosec;
	ret = clock_gettime(CLOCK_REALTIME,&gettmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}	
	oldtmpsec = gettmspec;
	ret = clock_nanosleep(CLOCK_REALTIME,0,&sleeptmspec,&sleeptmspec1);
	if (ret != 0) 
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() failed and errno is %d"),errno);
	 	goto close;			
		}
	ret = clock_gettime(CLOCK_REALTIME,&gettmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}	
	if (gettmspec.tv_sec != (oldtmpsec.tv_sec + Valuesec))
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() has failed to sleep for %d secs"),Valuesec);
	 	goto close;			
		}
	INFO_PRINTF1(_L("Relative timer"));
	INFO_PRINTF2(_L("clock_nanosleep() successfully able to sleep for %d secs"),Valuesec );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocknanosleep2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_nanosleep()
// Description: Trying to suspend the process for the specified time using clock_nanosleep()
// Absolute timer
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocknanosleep2 (  )
	{
	int ret, ret1 = KErrGeneral, Valuesec, Valuenanosec;
	struct timespec gettmspec, oldtmpsec, sleeptmspec, sleeptmspec1;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	goto close;
	  	}
	sleeptmspec.tv_sec = Valuesec;
	sleeptmspec.tv_nsec = Valuenanosec;
	ret = clock_gettime(CLOCK_REALTIME,&gettmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}	
	oldtmpsec = gettmspec;
	sleeptmspec.tv_sec = gettmspec.tv_sec + Valuesec;
	sleeptmspec.tv_nsec = gettmspec.tv_nsec + Valuenanosec;
	ret = clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&sleeptmspec,&sleeptmspec1);
	if (ret != 0) 
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() failed and errno is %d"),errno);
	 	goto close;			
		}
	ret = clock_gettime(CLOCK_REALTIME,&gettmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}	
	if (gettmspec.tv_sec < (oldtmpsec.tv_sec + Valuesec))
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() has failed to sleep for %d secs"),Valuesec);
	 	goto close;			
		}
	INFO_PRINTF1(_L("Absolute timer"));
	INFO_PRINTF2(_L("clock_nanosleep() successfully able to sleep for %d secs"),Valuesec );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocknanosleep3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_nanosleep()
// Description: Trying to suspend the process for an invalid time using clock_nanosleep() in the absence of TIMER_ABSTIME 
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocknanosleep3 (  )
	{
	int ret, ret1 = KErrGeneral, Valuesec, Valuenanosec;
	struct timespec sleeptmspec;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	goto close;
	  	}
	sleeptmspec.tv_sec = Valuesec;
	sleeptmspec.tv_nsec = Valuenanosec;
	ret = clock_nanosleep(CLOCK_REALTIME,0,&sleeptmspec,NULL);
	if ((ret != -1) || (errno != EINVAL)) 
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() failed and errno is %d"),errno);
	 	goto close;			
		}
	INFO_PRINTF1(_L("clock_nanosleep() successfully able to sleep for 2 secs") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocknanosleep4
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_nanosleep()
// Description: Trying to suspend the process with an invalid clock id clock_nanosleep() 
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocknanosleep4 (  )
	{
	int ret, ret1 = KErrGeneral, Invalidid;
	struct timespec sleeptmspec;
	ret = GetIntFromConfig(ConfigSection(), _L("Invalidid"), Invalidid);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	sleeptmspec.tv_sec = 2;
	sleeptmspec.tv_nsec = 0;
	ret = clock_nanosleep(Invalidid,0,&sleeptmspec,NULL);
	if ((ret != -1) || (errno != EINVAL)) 
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() failed to return EINVAL and errno is %d"),errno);
	 	goto close;			
		}
	INFO_PRINTF1(_L("clock_nanosleep() successfully able to return EINVAL ") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocknanosleep5
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_nanosleep()
// Description: clock_nanosleep() with an invalid parameter of timespec 
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocknanosleep5 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = clock_nanosleep(CLOCK_REALTIME,0,NULL,NULL);
	if ((ret != -1) || (errno != EFAULT)) 
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() failed to return EINVAL and errno is %d"),errno);
	 	goto close;			
		}
	INFO_PRINTF1(_L("clock_nanosleep() successfully able to return EINVAL ") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocknanosleep6
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_nanosleep()
// Description: clock_nanosleep() with a flag other than Absolute value
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocknanosleep6 (  )
	{
	int ret, ret1 = KErrGeneral, Valuesec, Valuenanosec;
	struct timespec gettmspec, oldtmpsec, sleeptmspec, sleeptmspec1;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	goto close;
	  	}
	sleeptmspec.tv_sec = Valuesec;
	sleeptmspec.tv_nsec = Valuenanosec;
	ret = clock_gettime(CLOCK_REALTIME,&gettmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}	
	oldtmpsec = gettmspec;
	ret = clock_nanosleep(CLOCK_REALTIME,15,&sleeptmspec,&sleeptmspec1);
	if (ret != 0) 
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() failed and errno is %d"),errno);
	 	goto close;			
		}
	ret = clock_gettime(CLOCK_REALTIME,&gettmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}	
	if (gettmspec.tv_sec != (oldtmpsec.tv_sec + Valuesec))
		{
	 	ERR_PRINTF2(_L("clock_nanosleep() has failed to sleep for %d secs"),Valuesec);
	 	goto close;			
		}
	INFO_PRINTF1(_L("Relative timer"));
	INFO_PRINTF2(_L("clock_nanosleep() successfully able to sleep for %d secs"),Valuesec );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclockgettime1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_gettime()
// Description: To get the current value for the specified valid clock_id<CLOCK_REALTIME> 
// -----------------------------------------------------------------------------

TInt CTestclock::Testclockgettime1 (  )
	{
	int ret, ret1 = KErrGeneral;
	clockid_t clockid;
	struct timespec tmspec, oldtmspec;
	ret = clock_getcpuclockid(0,&clockid);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to retrieve the clock id of the calling process and errno is %d"),errno);
	 	goto close;	
		}
	ret = clock_gettime(clockid,&tmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}	
	sleep(2);
	oldtmspec = tmspec;
	ret = clock_gettime(clockid,&tmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}
	if (tmspec.tv_sec != (oldtmspec.tv_sec + 2))
		{
	 	ERR_PRINTF1(_L("Failed to retrieve resolution of the clock id specified"));
	 	goto close;			
		}
	INFO_PRINTF1(_L("clock_gettime() successfully able to get the time") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclockgettime2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_gettime()
// Description: Trying to get the current time value for an invalid clock id using clock_gettime()  
// -----------------------------------------------------------------------------

TInt CTestclock::Testclockgettime2 (  )
	{
	int ret, ret1 = KErrGeneral, Clockid, Error;
	struct timespec tmspec;
	ret = GetIntFromConfig(ConfigSection(), _L("Clockid"), Clockid); 
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to clock id value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Error"), Error); 
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read expected error num")) ;
	 	goto close;
	  	}
	ret = clock_gettime(Clockid,&tmspec);
	if ((ret != -1) || (errno != Error)) 
		{
	 	ERR_PRINTF2(_L("clock_gettime() failed to return EINVAL for an invalid clock id and errno is %d"),errno);
	 	goto close;	
		}	
	INFO_PRINTF1(_L("clock_gettime() successfully returned EINVAL for an invalid clock id") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclockgettime3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_gettime()
// Description: Trying to get the current time value for a valid clock id with NULL as the timespec using clock_gettime()   
// -----------------------------------------------------------------------------

TInt CTestclock::Testclockgettime3 (  )
	{
	int ret, ret1 = KErrGeneral, Clockid;
	ret = GetIntFromConfig(ConfigSection(), _L("Clockid"), Clockid); 
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Unable to read clock id value")) ;
	 	goto close;
	  	}
	ret = clock_gettime(Clockid,NULL);
	if ((ret != -1) || (errno != EFAULT)) 
		{
	 	ERR_PRINTF2(_L("clock_gettime() failed to return EFAULT for NULL timespec parameter and errno is %d"),errno);
	 	goto close;	
		}	
	INFO_PRINTF1(_L("clock_gettime() successfully returned EFAULT for NULL timespec parameter ") );
	ret1 = KErrNone;
		
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocksettime1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_settime()
// Description: Trying to set the current time value for an invalid clock id using clock_settime()  
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocksettime1 (  )
	{
	int ret, ret1 = KErrGeneral, Clockid, Error;
	struct timespec tmspec;
	tmspec.tv_sec = 30;
	tmspec.tv_nsec = 2000;
	ret = GetIntFromConfig(ConfigSection(), _L("Clockid"), Clockid); 
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Unable to read clock id value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Error"), Error); 
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read expected error num")) ;
	 	goto close;
	  	}
	ret = clock_settime(Clockid,&tmspec);
	if ((ret != -1) || (errno != Error)) 
		{
	 	ERR_PRINTF2(_L("clock_settime() failed on negative test and errno is %d"),errno);
	 	goto close;	
		}	
	INFO_PRINTF1(_L("clock_settime() is successfull on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocksettime2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_settime()
// Description: clock_settime() with NULL as the timespec parameter  
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocksettime2 (  )
	{
	int ret, ret1 = KErrGeneral, Clockid;
	ret = GetIntFromConfig(ConfigSection(), _L("Clockid"), Clockid); 
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Unable to read clock id value")) ;
	 	goto close;
	  	}
	ret = clock_settime(Clockid,NULL);
	if ((ret != -1) || (errno != EFAULT)) 
		{
	 	ERR_PRINTF2(_L("clock_settime() failed to return EFAULT on negative test and errno is %d"),errno);
	 	goto close;	
		}	
	INFO_PRINTF1(_L("clock_settime() successfully returned EFAULT on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestclock::Testclocksettime3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: clock_settime()
// Description: Test case added to set the value of clock id current value to an invalid specified value using clock_settime()
// -----------------------------------------------------------------------------

TInt CTestclock::Testclocksettime3 (  )
	{
	int ret, ret1 = KErrGeneral, Clockid, Valuesec, Valuenanosec;
	struct timespec tmspec;
	ret = GetIntFromConfig(ConfigSection(), _L("Clockid"), Clockid); 
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Unable to read clock id value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	goto close;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	goto close;
	  	}
	tmspec.tv_sec = Valuesec;
	tmspec.tv_nsec = Valuenanosec;
	ret = clock_settime(Clockid,&tmspec);
	if ((ret != -1) || (errno != EINVAL)) 
		{
	 	ERR_PRINTF2(_L("clock_settime() failed to return EINVAL on negative test and errno is %d"),errno);
	 	goto close;	
		}	
	INFO_PRINTF1(_L("clock_settime() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

//End of a file
