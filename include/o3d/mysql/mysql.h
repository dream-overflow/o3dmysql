/**
 * @file mysql.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-12-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MYSQL_H
#define _O3D_MYSQL_H

#include <objective3dconfig.h>

namespace o3d {

// If no object export/import mode defined suppose IMPORT
#if !defined(O3D_MYSQL_EXPORT_DLL) && !defined(O3D_MYSQL_STATIC_LIB)
    #ifndef O3D_MYSQL_IMPORT_DLL
        #define O3D_MYSQL_IMPORT_DLL
    #endif
#endif

//---------------------------------------------------------------------------------------
// API define depend on OS and dynamic library exporting type
//---------------------------------------------------------------------------------------

#if (defined(O3D_UNIX) || defined(O3D_MACOSX) || defined(SWIG))
    #define O3D_MYSQL_API
    #define O3D_MYSQL_API_TEMPLATE
#elif defined(O3D_WINDOWS)
    // export DLL
    #ifdef O3D_MYSQL_EXPORT_DLL
        #define O3D_MYSQL_API __declspec(dllexport)
        #define O3D_MYSQL_API_TEMPLATE
    #endif
    // import DLL
    #ifdef O3D_MYSQL_IMPORT_DLL
        #define O3D_MYSQL_API __declspec(dllimport)
        #define O3D_MYSQL_API_TEMPLATE
    #endif
    // static (no DLL)
    #ifdef O3D_MYSQL_STATIC_LIB
        #define O3D_MYSQL_API
        #define O3D_MYSQL_API_TEMPLATE
    #endif
#endif

} // namespace o3d

#endif // _O3D_MYSQL_H
