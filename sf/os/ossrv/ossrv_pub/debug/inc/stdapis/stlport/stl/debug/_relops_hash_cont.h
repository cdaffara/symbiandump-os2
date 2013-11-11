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
// This is an implementation file which
// is intended to be included multiple times with different _STLP_ASSOCIATIVE_CONTAINER
// setting

#ifdef _STLP_EXTRA_OPERATORS_FOR_DEBUG
_STLP_TEMPLATE_HEADER
inline bool _STLP_CALL 
operator==(const _STLP_TEMPLATE_CONTAINER& __hm1, const _STLP_TEMPLATE_CONTAINER& __hm2)
{
  return _STLP_TEMPLATE_CONTAINER_BASE::_M_equal(__hm1, __hm2);
}

#ifdef _STLP_USE_SEPARATE_RELOPS_NAMESPACE

_STLP_TEMPLATE_HEADER
inline bool _STLP_CALL 
operator!=(const _STLP_TEMPLATE_CONTAINER& __hm1, const _STLP_TEMPLATE_CONTAINER& __hm2) {
  return !(__hm1 == __hm2);
}

#endif /* _STLP_USE_SEPARATE_RELOPS_NAMESPACE */

#endif /* _STLP_EXTRA_OPERATORS_FOR_DEBUG */

#ifdef _STLP_FUNCTION_TMPL_PARTIAL_ORDER

_STLP_TEMPLATE_HEADER
inline void _STLP_CALL 
swap(_STLP_TEMPLATE_CONTAINER& __hm1, _STLP_TEMPLATE_CONTAINER& __hm2)
{
  __hm1.swap(__hm2);
}

#endif /* _STLP_FUNCTION_TMPL_PARTIAL_ORDER */
