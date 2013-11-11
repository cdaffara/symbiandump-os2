/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file
 @internalComponent
 @released 
*/

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

namespace UserPromptService
	{	
	class CPolicyEvaluator;
	class CDialogCreator;
	class CPolicyEvaluatorPlugin;
	class CDialogCreatorPlugin;
	class CPluginManager;
	
	/**
	Container class for a plug-in that notifies a manager object when 
	it is destroyed.
	*/
	template <class T> 
		class CPlugin : public CBase
			{
		public:
			CPlugin(CPluginManager* aManager, T* aImp);
			T& Imp();
			~CPlugin();
		private:
			CPluginManager* iManager;	///< The plug-in manager that tracks active plug-ins.
			T* iImp;					///< The policy evaluator or dialog creator owned by this object.
			};
			
	/**
	Plug-in manager that tracks the number of plug-ins enabling REComSession::FinalClose()
	to be deferred (after calling Unload) once all the plug-ins have been unloaded.
	
	N.B. REComSession::FinalClose CANNOT be called from within an ECOM plug-in.	
	*/
	NONSHARABLE_CLASS(CPluginManager) : public CBase
		{
		template <class T> friend class CPlugin;
	public:
		IMPORT_C static CPluginManager* NewL();		
		IMPORT_C CPlugin<CPolicyEvaluator>* CreateEvaluatorL(const TUid& aEvaluatorId);
		IMPORT_C CPlugin<CDialogCreator>* CreateDialogCreatorL(const TUid& aDialogCreatorId);
		IMPORT_C void Unload();	
				
		~CPluginManager();
	public:
		// For testing. Do not modify directly
		TInt iPluginCount;		///< The number of active CPlugin objects. (do not modify)
	private:
		CPluginManager();			
		IMPORT_C void ReleasePlugin();

		TBool iUnload;			///< Indicates that a deferred call to REComSession::FinalClose is required		
		};	
				
	template <class T>
	inline CPlugin<T>::CPlugin(CPluginManager* aManager, T* aImp) : iManager(aManager), iImp(aImp)	
	/**
	Constructor
	@param aManager	The plug-in manager that tracks the number of plug-ins in existance.
	@param aImp		The ECOM implementation that this class managers.
	*/
		{
		}

	template <class T>
	inline T& CPlugin<T>::Imp() 
	/**
	Gets a reference to the plug-in implementation
	@return The plug-in implementation.
	*/
		{
		return *iImp;
		}
	
	template <class T>
	inline CPlugin<T>::~CPlugin() 
	/**
	Destructor
	*/
		{		
		delete iImp;
		iManager->ReleasePlugin();
		}		
	}
#endif // PLUGINMANAGER_H
