/**
 * @file mysqldbvariable.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/mysql/mysqldbvariable.h"

using namespace o3d;
using namespace o3d::mysql;

MySqlDbVariable::MySqlDbVariable()
{

}

MySqlDbVariable::MySqlDbVariable(
        DbVariable::IntType intType,
        DbVariable::VarType varType,
        UInt8 *data) :
    DbVariable(intType, varType, data)
{
    // null
    if (!data)
    {
        m_object = m_objectPtr = nullptr;
        m_isNull = True;

        return;
    }
}

MySqlDbVariable::MySqlDbVariable(
        DbVariable::IntType intType,
        DbVariable::VarType varType,
        UInt32 maxSize) :
    DbVariable(intType, varType, maxSize)
{

}

MySqlDbVariable::~MySqlDbVariable()
{

}

void MySqlDbVariable::setNull(Bool isNull)
{
    m_isNull = (UInt8)isNull;
}

Bool MySqlDbVariable::isNull() const
{
    return m_isNull != 0;
}

UInt8 *MySqlDbVariable::getIsNullPtr()
{
    return (UInt8*)&m_isNull;
}

void MySqlDbVariable::setLength(UInt32 len)
{
    m_length = len;
}

UInt32 MySqlDbVariable::getLength() const
{
    return m_length;
}

UInt8 *MySqlDbVariable::getLengthPtr()
{
    return (UInt8*)&m_length;
}

UInt8 *MySqlDbVariable::getErrorPtr()
{
    return (UInt8*)&m_isError;
}
