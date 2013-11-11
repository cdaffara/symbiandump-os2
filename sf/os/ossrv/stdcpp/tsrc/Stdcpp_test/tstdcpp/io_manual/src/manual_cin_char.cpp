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


#include<iostream>
using namespace std;

int char_test()
{
 char c,c1;
 cout<<"Test case for char "<<endl<<endl;

 cout<<"Enter following  char value  #"<<endl;
 cin>>c;
 cout<<"Expected output is #"<<endl;
 cout<<"Observed output is "<<c<<endl<<endl;


 cout<<"Enter following  char value  %"<<endl;
 cin>>c;
 cout<<"Expected output is %"<<endl;
 cout<<"Observed output is "<<c<<endl<<endl;



 cout<<"Enter following  char value  g"<<endl;
 cin>>c;
 cout<<"Expected output is g"<<endl;
 cout<<"Observed output is "<<c<<endl<<endl;

 cout<<"Enter following  char value  @"<<endl;
 cin>>c;
 cout<<"Expected output is @"<<endl;
 cout<<"Observed output is "<<c<<endl<<endl;

cout<<"Press 1 to PASS or 0 to FAIL this sub-case:\r\n";
 do{
    c1=getchar();
   }while(c1!='1' && c1!='0');
 
if('1'==c1)
 return 0;
else
 return 1;
}

int main()
{

int failures=0;

failures=char_test();
if(failures)
 cout<<"Fail"<<endl;
else
 cout<<"Pass"<<endl;
return 0;
}