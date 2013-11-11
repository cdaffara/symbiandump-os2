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


#ifndef _PROFILER_TOUCH_EVENT_CLIENT_DLL__
#define _PROFILER_TOUCH_EVENT_CLIENT_DLL__

#include <w32adll.h>

class CProfilerTouchEventControl;

class RProfilerTouchEventAnim : public RAnim
{
   public: 
      RProfilerTouchEventAnim( RAnimDll& aAnimDll );
      void ConstructL( const RWindow& aParent  );

	  void Activate();
	  void Deactivate();


      enum KAnimCommands
      {
         KActivate       = 70002,
         KDeactivate	 = 70003
      };
      /**
       * Closes the animation object
       */
   	  void Close();

   private:        
	  CProfilerTouchEventControl* iTouchEventControl;   
};

#endif
