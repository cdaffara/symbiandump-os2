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

// class Int is defined here because the primitive 'int' does not have a 
// default constructor. The default constructor is used when map['z']
// accesses an uninitialized element.
struct Int
  {
  Int(int x = 0) : val(x) {};
  int val;
  bool operator != (struct Int& a)
  {
  	
  	return val != a.val;	
  }
  };

