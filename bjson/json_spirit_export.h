// -*- C++ -*-
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl -n JSON_SPIRIT
// ------------------------------
#ifndef JSON_SPIRIT_EXPORT_H
#define JSON_SPIRIT_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (JSON_SPIRIT_HAS_DLL)
#  define JSON_SPIRIT_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && JSON_SPIRIT_HAS_DLL */

#if !defined (JSON_SPIRIT_HAS_DLL)
#  define JSON_SPIRIT_HAS_DLL 1
#endif /* ! JSON_SPIRIT_HAS_DLL */

#if defined (JSON_SPIRIT_HAS_DLL) && (JSON_SPIRIT_HAS_DLL == 1)
#  if defined (JSON_SPIRIT_BUILD_DLL)
#    define JSON_SPIRIT_Export ACE_Proper_Export_Flag
#    define JSON_SPIRIT_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define JSON_SPIRIT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#    if defined(ACE_HAS_CUSTOM_EXPORT_MACROS) || (defined (ACE_GCC_HAS_TEMPLATE_INSTANTIATION_VISIBILITY_ATTRS) && ACE_GCC_HAS_TEMPLATE_INSTANTIATION_VISIBILITY_ATTRS == 1)
#      define JSON_SPIRIT_SINGLETON_TEMPLATE_CLASS template class ACE_Proper_Export_Flag
#      define JSON_SPIRIT_SINGLETON_TEMPLATE_STRUCT template struct ACE_Proper_Export_Flag
#      define JSON_SPIRIT_GCC_VISIBILITY_PUSH_DEFAULT
#      define JSON_SPIRIT_GCC_VISIBILITY_POP
#    else
#      define JSON_SPIRIT_SINGLETON_TEMPLATE_CLASS template class
#      define JSON_SPIRIT_SINGLETON_TEMPLATE_STRUCT template struct
#      define JSON_SPIRIT_GCC_VISIBILITY_PUSH_DEFAULT _Pragma ("GCC visibility push(default)")
#      define JSON_SPIRIT_GCC_VISIBILITY_POP _Pragma ("GCC visibility pop")
#    endif
#  else /* JSON_SPIRIT_BUILD_DLL */
#    define JSON_SPIRIT_Export ACE_Proper_Import_Flag
#    define JSON_SPIRIT_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define JSON_SPIRIT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#    define JSON_SPIRIT_SINGLETON_TEMPLATE_CLASS extern template class
#    define JSON_SPIRIT_SINGLETON_TEMPLATE_STRUCT extern template struct
#    define JSON_SPIRIT_GCC_VISIBILITY_PUSH_DEFAULT
#    define JSON_SPIRIT_GCC_VISIBILITY_POP
#  endif /* JSON_SPIRIT_BUILD_DLL */
#else /* JSON_SPIRIT_HAS_DLL == 1 */
#  define JSON_SPIRIT_Export
#  define JSON_SPIRIT_SINGLETON_DECLARATION(T)
#  define JSON_SPIRIT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  define JSON_SPIRIT_GCC_VISIBILITY_PUSH_DEFAULT
#  define JSON_SPIRIT_GCC_VISIBILITY_POP
#endif /* JSON_SPIRIT_HAS_DLL == 1 */

// Set JSON_SPIRIT_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (JSON_SPIRIT_NTRACE)
#  if (ACE_NTRACE == 1)
#    define JSON_SPIRIT_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define JSON_SPIRIT_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !JSON_SPIRIT_NTRACE */

#if (JSON_SPIRIT_NTRACE == 1)
#  define JSON_SPIRIT_TRACE(X)
#else /* (JSON_SPIRIT_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define JSON_SPIRIT_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (JSON_SPIRIT_NTRACE == 1) */

#endif /* JSON_SPIRIT_EXPORT_H */

// End of auto generated file.
