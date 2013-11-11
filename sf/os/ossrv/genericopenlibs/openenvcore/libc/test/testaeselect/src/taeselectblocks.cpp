// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : taeselectblocks.cpp
//

#include "taeselect.h"

TInt CTestAESelect::TestTimeout()
    {
    // Create temporary variables in stack
    TInt timesecs;
    TInt timemicrosecs;  	
    timemicrosecs = 0;
    timesecs = 10;  	
    struct timeval tv;
    tv.tv_sec = timesecs;
    tv.tv_usec = timemicrosecs;    
    time_t time1,time2;
    int err = time(&time1);    
    int fd = eselect(1,NULL,NULL,NULL,&tv,0,NULL);  
    err = time(&time2);
    if ( ((time2 - time1) >= timesecs) && fd != -1 )
        {                    
        return KErrNone; 
        }
    else
        {       
        return KErrGeneral; 
        }        
    }

TInt CTestAESelect::TestAselect()
	{
	timeval time;
	time.tv_sec = 5;
	time.tv_usec = 0;
	TRequestStatus status;
	// Issue aselect request which complete after 5 seconds
	aselect(0, NULL, NULL, NULL,&time ,&status);
	// Catch the request completion event
	User::WaitForRequest(status);
	// The status is completed with negative value on failure
	if( status.Int() < 0 )
		{
		return KErrGeneral;
		}
	return KErrNone;
	}

TInt CTestAESelect::TestCancelAselect()
	{
	timeval time;
	time.tv_sec = 5;
	time.tv_usec = 0;
	TRequestStatus status[10];
	for( int i=0; i<10;i++ )
		{
		aselect(0, NULL, NULL, NULL,&time ,&status[i]);
		}
	for( int j=0; j<10;j++ )
		{
		int p = cancelaselect(&status[j]);
		if( p != 0)
			{
			return KErrGeneral;
			}
		// Consume the cancellation event
		User::WaitForRequest(status[j]);
		if( status[j] != KErrCancel )
			{
			return KErrGeneral;
			}
		}
	return KErrNone;
	}

TInt CTestAESelect::TestAselectRequestStatus()
	{
	timeval time;
	time.tv_sec = 5;
	time.tv_usec = 0;
	int return_val=0;
	TRequestStatus status;
	// Issue aselect request which complete after 5 seconds
	aselect(0, NULL, NULL, NULL,&time ,&status);
	// Check for the value of status
	if( status == KRequestPending )
		{
		return_val = KErrNone;
		}
	else
		{
		return_val = KErrGeneral;
		}
	// Cancel the aselect request
	cancelaselect(&status);
	// Consume the cancellation event
	User::WaitForRequest(status);
	// The status is completed with negative value on failure
	return return_val;	
	}

TInt CTestAESelect::TestMultipleAselect()
	{
	timeval tim;
	tim.tv_sec = 5;
	tim.tv_usec = 0;
	int j=0;
	TRequestStatus status[10];
	// Issue 10 aselect requests which complete after 5 seconds
	for( int i=0; i<10;i++ )
		{
		aselect(0, NULL, NULL, NULL,&tim ,&status[i]);
		}
	// Catch each of the request completion events
	while(j<10)
		{
		User::WaitForAnyRequest();
		j++;
		}
	for( int i=0;i<10;i++)
		{
		// The status is completed with negative value on failure
		if( status[i].Int() < 0 )
			{
			return KErrGeneral;
			}
		}
	return KErrNone;
	}

TInt CTestAESelect::TestSelectwithEselect()
	{
	TInt return_value = KErrGeneral;
	int fd;
	fd_set readfds;
	fd_set writefds;
	int write_stat=0;
	char *data="HelloWorld";
	char *filename="c:\\eselecttest.txt";
	int max;
	fd = open(filename,O_CREAT | O_RDWR);
	int err = errno;
	max = fd;
	FD_ZERO(&writefds);
	FD_SET(fd, &writefds);
	FD_ZERO(&readfds);
	FD_SET(fd,&readfds);		
	if ( eselect(max+1, &readfds, &writefds, NULL, NULL,0,NULL) > 0 )
		{
		if (FD_ISSET(fd, &writefds))
			{
			write_stat = write(fd, data,10 );
			if (write_stat != -1)
				{
				// Success  				
				return_value = KErrNone;
				}
			}
		}
	close(fd);
	return return_value;
	}

TInt CTestAESelect::TestEselect()
	{
	TRequestStatus waitarray[3];
	for (TInt i = 0; i < 3; i++)
		{
		waitarray[i] = KRequestPending;
		}
	RTimer timer;
	timer.CreateLocal();
	timer.After(waitarray[2],TTimeIntervalMicroSeconds32(3000000));
	if ( eselect(1, NULL, NULL, NULL, NULL,3,waitarray) == KErrNone )
		{
		timer.Close();
		if( waitarray[2] != KRequestPending )
			{
			return KErrNone;
			}
		else
			{
			return KErrGeneral;
			}
		}
	
	else
		{
		timer.Close();
		return KErrGeneral;
		}
	}

TInt CTestAESelect::TestEselectRequestStatus()
	{
	// Test to ensure eselect does not alter the value of TRequestStatus objects in
	// passed to eselect
	TRequestStatus waitarray[3];
	// Assign random values
	waitarray[0] = -24;
	waitarray[1] = -15;
	RTimer timer;
	timer.CreateLocal();
	timer.After(waitarray[2],TTimeIntervalMicroSeconds32(3000000));
	// eselect should return 0 if an event occurs on any the TRequestStatus 
	if ( eselect(1, NULL, NULL, NULL, NULL,3,waitarray) == KErrNone )
		{
		timer.Close();
		if( waitarray[0] == -24 && waitarray[1] == -15 )
			{
			return KErrNone;
			}
		else
			{
			return KErrGeneral;
			}
		}
	else
		{
		timer.Close();
		return KErrGeneral;
		}
	}

TInt CTestAESelect::TestEselectTimeout()
	{
    TInt timesecs;
    TInt timemicrosecs;  	
    timemicrosecs = 0;
    timesecs = 5;  	
    struct timeval tv;
    tv.tv_sec = timesecs;
    tv.tv_usec = timemicrosecs;    
    time_t time1,time2;
	TRequestStatus waitarray[3];
	for (TInt i = 0; i < 3; i++)
		{
		waitarray[i] = KRequestPending;
		}
	RTimer timer;
	timer.CreateLocal();
	timer.After(waitarray[2],TTimeIntervalMicroSeconds32(10000000));
    int err = time(&time1);
    // Upon time out from timeval eselect should return 0
	if ( eselect(1, NULL, NULL, NULL, &tv,3,waitarray) == KErrNone )
		{
	    err = time(&time2);
	    // errno should be set to ETIMEDOUT if eselect returns due to a timeval timeout
	    if( errno == ETIMEDOUT )
	    	{
	    	if ( ((time2 - time1) >= timesecs) && (waitarray[2] == KRequestPending) )
	    		{
	    		timer.Cancel();
	    		timer.Close();
	    		return KErrNone;
	    		}
	    	else
	    		{
	    		timer.Cancel();
	    		timer.Close();
	    		return KErrGeneral;
	    		}
	    	}
	    else
	    	{
	    	// errno not set to ETIMEDOUT, hence it is an error
	    	return KErrGeneral;
	    	}
		}
	else
		{
		timer.Cancel();
		timer.Close();
		return KErrGeneral;
		}
	}

TInt CTestAESelect::TestEselectInvalidInput_01()
	{
	TInt max = 0;
	TInt selectret = 0;
	selectret = eselect(max+1, NULL, NULL, NULL, NULL,3,NULL);
	if( selectret )
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	}

TInt CTestAESelect::TestEselectInvalidInput_02()
	{
	TInt max = 0;
	TInt selectret = 0;
	TRequestStatus status[3];
	selectret = eselect(max+1, NULL, NULL, NULL, NULL,0,status);
	if( selectret )
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	}

