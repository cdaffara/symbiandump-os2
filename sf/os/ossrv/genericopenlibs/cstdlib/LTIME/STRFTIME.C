/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* NAME:
* strftime - convert date and time to a string 
* SYNOPSIS:
* size_t strftime (char *array, size_t limitsize,const char*pattern,const struct tm *tme)
* Formats  Time into string for  the given tags[options] with a  limit of  Limitsize bytes.
* Options:
* %a - Abbreviated weekday name (eg:Mon)
* %A - Full weekday name (eg:Monday)
* %b - Abbreviated month name (eg:Jan)
* %B - full month name (eg:January)
* %c - preferred date and time representation (eg:Tue 06 12:34:45 2009)
* %C - century number (the year divided by 100 and truncated to an integer, range 00 to 99)
* %d - Day of the month as a decimal number (range 01 to 31)
* %D - Same as %m/%d/%y
* %e - Day of the month as a decimal number, a single digit is preceded by a space (range ' 1' to '31')
* %F- Equivalent to %Y-%m-%d 
* %g - Same As %G, but without the century.
* %G - The 4-digit year corresponding to the ISO week number (see %V). This has the same format and value as %Y, except that if the ISO week number belongs to the previous or next year, that year is used instead.
* %h - Same as %b
* %H - Hour as a decimal number using a 24-hour clock (range 00 to 23)
* %I - Hour as a decimal number using a 12-hour clock (range 01 to 12)
* %j - Day of the year as a decimal number (range 001 to 366)
* %m - Month as a decimal number (range 01 to 12)
* %M - Minute as a decimal number
* %n - Newline character
* %p - UPPER-CASE `AM' or `PM' according to the given time value
* %P - Lower-case `am' or `pm' according to the given time value
* %r - Time in a.m. and p.m. notation
* %R - Time in 24 hour notation
* %S - Second as a decimal number
* %t - Tab character
* %T - Current time, Equal to %H:%M:%S
* %u - Weekday as a decimal number [1,7], with 1 representing Monday
* %U - Week number of the current year as a decimal number, starting with the first Sunday as the first day of the first week
* %V - The ISO 8601:1988 week number of the current year as a decimal number, range 01 to 53, where week 1 is the first week that has at least 4 days in the current year, and with Monday as the first day of the week. (Use %G or %g for the year component that corresponds to the week number for the specified timestamp.)
* %W - Week number of the current year as a decimal number, starting with the first Monday as the first day of the first week
* %w - Day of the week as a decimal, Sunday being 0
* %x - Preferred date representation for the current locale without the time
* %X - Preferred time representation for the current locale without the date
* %y - Year as a decimal number without a century (range 00 to 99)
* %Y - Year as a decimal number including the century
* %Z or %z - Time zone offset or name or abbreviation (Operating System dependent)
* %% - percent sign
* Maximum length of this parameter is 1023 characters. 
* 
*
*/



#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>  
#include <string.h>

static const int dayname_length[7] ={6, 6, 7, 9, 8, 6, 8};
static const char*const  adayname[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

static const char *const dayname[7] ={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static const int monthname_length[12] ={7, 8, 5, 5, 3, 4, 4, 6, 9, 7, 8, 8};

static const char *const amonthname[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

static const char *const monthname[12] ={"January", "February", "March", "April","May", "June", "July", "August", "September", "October", "November","December"};


//  The Function Stores all characters  in Char * array,until it finds %.
int splitter(char *array,size_t limitsize,const char** format,size_t* counter)
{
	while(**format!='%' && **format)
	   {
		 if(*counter<limitsize-1 )
			{
				array[*counter]=**format;
				 (*counter)++;
				(*format)++;
			}
     
		 else
			{
				return  1;
			}		
	  }
	 array[*counter]='\0';
	 return 0;
}
// The Funtion is Used for Storing Abbreviated WeekDay Nname in Place of %a. (eg:Mon)
int funa(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	 if(*counter<limitsize-1)
		{   
			strcat(array,adayname[tme->tm_wday]);
			*counter+=3;
		}
	else
		{
			return 1;
		}	
	return 0;
}
//The Function is Used for Stroring WeekDay Name in Place of %A. (eg:Monday)
int funA(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	 if(*counter<limitsize-1)
		{
			strcat(array,dayname[tme->tm_wday]);
			*counter+=dayname_length[tme->tm_wday];                         
		}
	else
		{
			return 1;
		}
	return 0;
}  
// The  Function  is Used for Storing Abbreviated Month Name in Place of %b.(eg:Jan)
int funb(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-1)
		{
			strcat(array,amonthname[tme->tm_mon]);
			*counter+=3;                        
		}
	else
		{
			return  1;
		}	
	return 0;
}
// The  Function  is Used for Storing Month Name in Place of %B .(eg:January) 
int funB(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-1)
		{
			strcat(array,monthname[tme->tm_mon]);
			*counter+=monthname_length[tme->tm_mon];                         
		}
	else
		{
			return 1;
		}
	return 0;
}
// The Function is Used for Storing preferred date and time representation  in Place of %c.(eg:Tue 06 12:34:45 2009)
int func(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-24)
		{
			strcat(array,adayname[tme->tm_wday]);
			strcat(array," ");
			strcat(array,amonthname[tme->tm_mon]);
			*counter+=7;
			sprintf(&array[*counter]," %.2d %.2d %.2d %.2d %.4d",tme->tm_mday,tme->tm_hour,tme->tm_min,tme->tm_sec,1900+tme->tm_year);
			*counter+=17;
			array[*counter]='\0';
		}  	
	else
		{	
			return  1;
		}
	return 0; 
}
// The Function is Used for Storing  Century number in place of %C .(eg:he year divided by 100 and truncated to an integer, range 00 to 99)
int funC(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-1)
		{
			sprintf(&array[*counter],"%.2d",(1900+tme->tm_year)/100);
			*counter+=2;
			array[*counter]='\0';
		}
	else
		{
		return  1;
		}
	return 0;
}
// The Function is Used for Storing Day of the month as a decimal number in Place of %d.(eg:range 01 to 31)	
int fund(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2)
		{
		sprintf (&array[*counter], "%.2d", tme->tm_mday);
		*counter += 2;
		array[*counter]='\0';
		}
	else
		{
		return  1;
		}
	return 0;    	
}
// The Function is Used for Storing  Date in form of %m/%d/%y in Place of %D.
int funD(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-8)
		{
			sprintf(&array[*counter],"%.2d/%.2d/%.2d",tme->tm_mon,tme->tm_mday,(1900+tme->tm_year)%100);  
			*counter+=8;
			array[*counter]='\0';        
		}
	else
		{
			return  1;
		}	
	return 0;
}
// The Function is Used for Storing Day of the month as a Decimal number, a single digit is preceded by a space in Place of %e. (eg:range ' 1' to '31')
int fune(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-2)
		{
			if(tme->tm_mday<10)
				sprintf(&array[*counter]," %d",tme->tm_mday);
			else
				sprintf(&array[*counter],"%2d",tme->tm_mday);  
			*counter+=2; 
			array[*counter]='\0';                      
		}
	else
		{
			return  1;
		}
	return 0; 
                            
}
// The Function is Used for Storing date in form of  %Y-%m-%d 
int funF(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-10)
		{
			sprintf(&array[*counter],"%4d/%.2d/%.2d",1900+tme->tm_year,tme->tm_mon,tme->tm_mday);
			*counter+=10;
			array[*counter]='\0';
		}   
	else
		{
		return  1;
		}
	return 0;
}
// The Function is  same as that of funG but without Century
int fung(char*array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-2)
		{
			sprintf(&array[*counter],"%.2d",(1900+tme->tm_year)%100);
			*counter+=2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}	
	return 0;
}
// The Function is Used For Storing  4-digit year corresponding to the ISO week number  in place of %G
int funG(char*array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-4)
		{
			sprintf(&array[*counter],"%4d",1900+tme->tm_year);
			*counter+=4;
			array[*counter]='\0';
		}
	else
		{	
			return  1;
		}	
	return 0;
}
// The Function is same as funb
int funh(char*array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-1)
		{
		strcat(array,amonthname[tme->tm_mon]);
		*counter+=3;
		array[*counter]='\0';
		}
	else
		{
			return  1;
		}	
	return 0;   
}
// The Function is Used for Storing Hour as a decimal number using a 24-hour clock in Place of %H (eg:range 00 to 23)	
int funH(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{	
	if (*counter < limitsize - 2)
		{
			sprintf (&array[*counter], "%.2d",
			tme->tm_hour);
			*counter += 2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;  	
}
// The Function is Used for Storing Hour as a decimal number using a 12-hour clock in Place of %I(eg:range 01 to 12)
int funI(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2)
		{
			if(tme->tm_hour%12)
				sprintf(&array[*counter],"%.2d",tme->tm_hour%12);
			else
				strcat(array,"12");
			*counter+=2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;   
}
// The Function is Used for Storing Day of the year as a decimal number in Place of %j (eg:range 001 to 366)
int funj(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 3)
		{
			sprintf (&array[*counter], "%.3d", tme->tm_yday + 1);
			*counter += 3;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;  	
}
// The Function is Used for StoringThe hour (24-hour clock) as a decimal number (range  0  to  23);single digits are preceded by a blankin Place of %k
int funk(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if(*counter<limitsize-2)
		{
			if(tme->tm_hour<10)
				sprintf(&array[*counter]," %d",tme->tm_hour);
			else
				sprintf(&array[*counter],"%2d",tme->tm_hour);
			*counter+=2; 
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}	
	return 0;    
}
// The Function is Used for Storing The  hour  (12-hour  clock) as a decimal number (range 1 to 12);single digits are preceded by a blank in Place of %l
int funl(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2)
		{
			if(tme->tm_hour%12)
				{
					if(tme->tm_hour%12<10)
						sprintf(&array[*counter],"% d",tme->tm_hour%12);
					else
						sprintf(&array[*counter],"%d",tme->tm_hour%12);
				}
			else
				strcat(array,"12");
			*counter+=2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;       
}
// The Function is Used for Storing Month as a decimal number in Place of %m (eg:range 01 to 12)
int funm(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2)
		{
			sprintf (&array[*counter], "%.2d", tme->tm_mon + 1);
			*counter += 2;
			array[*counter]='\0';
		}
	else
		{
		return  1;
		}
	return 0;   	
}
// The Function is Used for Storing Minute as a decimal number in Place of %M
int funM(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2)
		{
			sprintf (&array[*counter], "%.2d", tme->tm_min);
			*counter += 2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;  	
}
// The Function is Used for Storing `AM' or `PM' according to the given time  value,  or  the corresponding  strings  for the current locale.  Noon is treated as `pm' and midnight as `am'.
int funp(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter< limitsize - 2)
		{
			if (tme->tm_hour < 12)
				strcat(array,"A");
			else
				strcat(array,"P");
			strcat(array,"M");
			*counter+=2;
		}
	else
		{
			return  1;
		}
	return 0;   	
}
// The Function is Used for Storing `am' or `pm' according to the given time  value,  or  the corresponding  strings  for the current locale.  Noon is treated as `pm' and midnight as `am'.
int funP(char*array,size_t limitsize,size_t *counter,const struct tm*tme)
{	
	if (*counter< limitsize - 2)
		{
			if (tme->tm_hour < 12)
				strcat(array,"a");
			else
				strcat(array,"p");
			strcat(array,"m");
			*counter+=2;
		}
	else
		{
			return  1;
		}
	return 0;
}
//The Function is Used for Storing  Hour,minute,second,time value in place of %r,same as %I:%M:%S %p'
int funr(char*array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter< limitsize - 11)
		{
			if(tme->tm_hour%12)
				{
					if(tme->tm_hour%12<10)
						sprintf(&array[*counter],"% d",tme->tm_hour);
					else
						sprintf(&array[*counter],"%d",tme->tm_hour);
				}
			else
				strcat(array,"12");  
			*counter+=2;
			array[*counter]='\0';
			strcat(array,":");
			*counter+=1;
			sprintf (&array[*counter], "%.2d", tme->tm_min);
			*counter += 2;
			array[*counter]='\0';
			strcat(array,":");
			*counter+=1;
			sprintf (&array[*counter], "%2.2d",tme->tm_sec);
			*counter += 2;
			array[*counter]='\0';
			strcat(array," ");
			*counter+=1;
			if (tme->tm_hour < 12)
				strcat(array,"A");
			else
				strcat(array,"P");
			strcat(array,"M");
			*counter+=2;
		}
	else
		{
			return  1;
		}
	return 0; 
     
}
//   The Function is Used for Storing  The time in 24-hour notation in Place of %R .Same as (%H:%M)..
int funR(char*array,size_t limitsize,size_t *counter,const struct tm*tme)
{
    if (*counter< limitsize - 5)
    {
     sprintf (&array[*counter], "%.2d",
		       tme->tm_hour);
     strcat(array,":");
     *counter+=3;
      sprintf (&array[*counter], "%.2d", tme->tm_min);
	      *counter += 2;
	      array[*counter]='\0';
       }
       else
	       {
		   return  1;
		}	   
    return 0;
    
}
//   The Function is Used for Storing The  number  of  seconds since the Epoch, i.e., since 1970-01-0  0:00:00 UTC.in place of %s
int funs(char*array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	size_t count=0;
	size_t temptime;
	time_t timeinseconds;
	timeinseconds = time (NULL);
	temptime=timeinseconds;
	while(temptime>0)
		{
			count++;
			temptime/=10;
		}
	if(*counter<limitsize-count)
		{
			sprintf(&array[*counter],"%ld",timeinseconds);
			*counter+=count;
			array[*counter]='\0';
		}
	else
		{
		return  1;
		}
	return 0;
    
}
//   The Function is Used for Storing   The second as a decimal number (eg:range 00 to 60)
int funS(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2)
		{
			sprintf (&array[*counter], "%.2d", tme->tm_sec);
			*counter += 2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;     	
}
//   The Function is Used for Storing  The time in 24-hour notation (%H:%M:%S) in place of %T
int funT(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	 if (*counter < limitsize - 2)
		 {
			 sprintf (&array[*counter], "%.2d",tme->tm_hour);
			 *counter+=2;
			 array[*counter]='\0';
			 strcat(array,":");
			 *counter += 1; 
			 sprintf (&array[*counter], "%.2d", tme->tm_min);
			 *counter+=2;
			 array[*counter]='\0';
			 strcat(array,":");
			 *counter += 1;
			 sprintf (&array[*counter], "%.2d", tme->tm_sec);
			 *counter += 2;
			 array[*counter]='\0';
		} 
	else
		{
			return  1;
		}
	return 0;
}
//   The Function is Used for Storing The  day of the week as a decimal, range 1 to 7, Monday being 1 in Place of %U.
	
int funu(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 1)
		{
			sprintf (&array[*counter], "%.1d",tme->tm_wday);
			*counter+=1;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;    
}
//   The Function is Used for Storing The week number of the current year as a decimal  number,  range  00  to  53,  starting  with the first Sunday as the first day of week 01 in Place of %U
int funU(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2) 
		{
			int  temp= tme->tm_yday/7;
			if (tme->tm_yday%7 >tme->tm_wday)
				temp++;
			sprintf(&array[*counter],"%.2d",temp);
			*counter+=2;
			array[*counter]='\0';
		}
	else
		{
			return   1;
		}
	return 0;      	
}
//   The Function is Used for Storing The  day of the week as a decimal, range 0 to 6, Sunday being 0 in Place of %w
int funw(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 1)
		{
			sprintf (&array[*counter], "%.1d",tme->tm_wday);
			*counter+=1;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;    	
}
//   The Function is Used for Storing The week number of the current year as a decimal  number,  range 00  to  53,  starting  with the first Monday as the first day of week 01 in Place of %W

int funW(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2) 
		{
			int temp= tme->tm_yday/7;
			if (tme->tm_yday%7 > (tme->tm_wday+6)%7)
				temp++;
			sprintf(&array[*counter],"%.2d",temp);
			*counter+=2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;
}
// The Function is Used for Storing preferred date representation for the current locale without   the time in Place of %x
int funx(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 15)
		{
			strcat(array,adayname[tme->tm_wday]);
			*counter+=3;
			array[*counter]='\0';
			strcat(array," ");
			*counter+=1;
			strcat(array,amonthname[tme->tm_mon]);
			strcat(array," ");
			*counter+=4;
			sprintf (&array[*counter]," %.2d %.4d", tme->tm_mday,1900 + tme->tm_year);
			*counter += 8;
			array[*counter]='\0';
		}
	else
		{	
			return  1;
		}
	return 0;    	
}
// The Function is Used for Storing  preferred time representation for the current locale without the date in Place of %X
int funX(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 8)
		{
			sprintf (&array[*counter],"%2.2d:%2.2d:%2.2d",tme->tm_hour, tme->tm_min,tme->tm_sec);
			*counter += 8;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;     	
}
// The Function is Used for Storing  year as a decimal number without a century (range 00 to 99). in Place of %y
int funy(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 2)
		{
			sprintf (&array[*counter], "%.2d",tme->tm_year);
			*counter += 2;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;   	
}
// The Function is Used for Storing  The year as a decimal number including the century in Place of %Y
int funY(char *array,size_t limitsize,size_t *counter,const struct tm*tme)
{
	if (*counter < limitsize - 4)
		{
			sprintf (&array[*counter], "%.4d",1900 + tme->tm_year);
			*counter += 4;
			array[*counter]='\0';
		}
	else
		{
			return  1;
		}
	return 0;   	
}

//- convert/Formats date and time to a string for  the given tags[options] with a  limit of  Limitsize bytes
// It formats the broken-down time  according to the format specification format and places the result in the  character  array of  Limitsize .The Function returns the number of characters written in the array,
// if successful and 0 if  the size exceeeds the limit .

EXPORT_C size_t strftime (char *array, size_t limitsize,const char*pattern,const struct tm *tme)
{          
	size_t* counter=(size_t* )malloc(sizeof(size_t));
	size_t error=0,funerror=0;
	*counter=0;
	array[*counter]='\0';
	while(1)
		{
			error=splitter(array,limitsize,&pattern,counter);
			if(!error)
				{
					if(*pattern=='\0')
						break;
					else
						{
							pattern++;
							switch(*pattern)
								{
									case '%':
										if(*counter<limitsize-1)
											array[(*counter)++]='%';
										else
											return  0;
										break;
									case 'a':
										funerror= funa(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'A':
										funerror=funA(array,limitsize,counter,tme);
										if(funerror)
											return 0;
										break;
									case 'b':
										funerror=funb(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'B':
										funerror=funB(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'c':
										funerror=func(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'C':
										funerror=funC(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'd':
										funerror=fund(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'D':
										funerror=funD(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break; 
									case 'e':
										funerror=fune(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'F':
										funerror=funF(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;         
									case 'g':
										funerror=fung(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;  
									case 'G':
										funerror=funG(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'H':
										funerror=funH(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'I':
										funerror=funI(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'j':
										funerror=funj(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'k':
										funerror=funk(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'l':
										funerror=funl(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'm':
										funerror=funm(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'M':
										funerror=funM(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'p':
										funerror=funp(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'P':
										funerror=funP(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'r':
										funerror=funr(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'R':
										funerror=funR(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break; 
									case 's':
										funerror=funs(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'S':
										funerror=funS(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'T':
										funerror=funT(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'u':
										funerror=funu(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'U':
										funerror=funU(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'w':
										funerror=funw(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'W':
										funerror=funW(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'x':
										funerror=funx(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'X':
										funerror=funX(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case'y':
										funerror=funy(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'Y':
										funerror=funY(array,limitsize,counter,tme);
										if(funerror)
											return  0;
										break;
									case 'Z':
										break;
								}		
						}
				}		
			else
				{
					return  0;
				}
			if (*pattern)
				pattern++;
			else
				break;
		}
	array[*counter]='\0';
	error=*counter;
	free (counter);
	counter=NULL;
	return error;
}
