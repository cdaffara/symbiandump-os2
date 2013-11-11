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

int int_test()
{
int x;
unsigned int y;
char c;

cout<<"Test case for int and unsigned int"<<endl;
x=2147483647;
//x=32 
cout<<"The following value should be  2147483647 "<<endl;
cout<<x<<endl;

x=-2147483648;
cout<<"The following value should be  -2147483648 "<<endl;
cout<<x<<endl;

y=4294967295;
cout<<"The following value should be 4294967295"<<endl;
cout<<y<<endl;

y=0;
cout<<"The following value should be 0"<<endl;
cout<<y<<endl;

 cout<<"Press 1 to PASS or 0 to FAIL this sub-case:\r\n";
do{
    c=getchar();
}while(c!='1' && c!='0');

if('1'==c)
 return 0;
else 
 return 1;

}
int main()
{
 int failures=0; 
 failures=int_test();
if(failures)
 cout<<"Fail"<<endl;
else
 cout<<"Pass"<<endl;

 return 0;
}


