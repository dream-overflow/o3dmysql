/**
 * @file mysqlexception.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MYSQLEXCEPTION_H
#define _O3D_MYSQLEXCEPTION_H

#include "mysql.h"
#include <o3d/core/error.h>

namespace o3d {
namespace mysql {

//! @class E_MySqlError Unable to connect to MySql server
class O3D_MYSQL_API E_MySqlError : public E_BaseException
{
    O3D_E_DEF_CLASS(E_MySqlError)

    //! Ctor
    E_MySqlError(const String& msg) : E_BaseException(msg)
        O3D_E_DEF(E_MySqlError,"MySql error")
};

} // namespace mysql
} // namespace o3d

#endif // _O3D_MYSQLEXCEPTION_H
