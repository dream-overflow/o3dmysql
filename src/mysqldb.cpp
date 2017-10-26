/**
 * @file mysqldb.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-12-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/mysql/mysqldb.h"
#include "o3d/mysql/mysqlexception.h"
#include "o3d/mysql/mysqldbvariable.h"

#include <o3d/core/application.h>
#include <o3d/core/objects.h>

using namespace o3d;
using namespace o3d::mysql;

static UInt32 ms_mySqlLibRefCount = 0;
static Bool ms_mySqlLibState = False;

//! Default ctor
MySqlDb::MySqlDb() :
    Database(),
    m_pDB(nullptr)
{
    if (!ms_mySqlLibState)
        O3D_ERROR(E_InvalidPrecondition("MySql::init() must be called before"));

    ++ms_mySqlLibRefCount;
}

MySqlDb::~MySqlDb()
{
    disconnect();
    --ms_mySqlLibRefCount;
}

// Connect to a database
Bool MySqlDb::connect(
        const String &host,
        const String &database,
        const String &login,
        const String &password,
        Bool keepPassord)
{
    UInt16 port = 3306; // default port
    Int32 pos;

    m_Server = host;
	m_Database = database;
	m_Login = login;

	if (keepPassord)
	{
		m_Password = password;
	}

    if ((pos = host.find(':')) != -1)
	{
		m_Server.truncate(pos);

        port = host.sub(pos+1).toUInt32();
    }

    m_pDB = mysql_init(m_pDB);
    O3D_ASSERT(m_pDB != nullptr);

    if (!mysql_real_connect(
                m_pDB,
                m_Server.toUtf8().getData(),
                m_Login.toUtf8().getData(),
                m_Password.toUtf8().getData(),
                m_Database.toUtf8().getData(),
                port,
                NULL,
                0)/*CLIENT_MULTI_STATEMENTS)*/)
    {
        //unsigned int erro = mysql_errno(m_pDB);
        O3D_ERROR(E_MySqlError(mysql_error(m_pDB)));
    }

	O3D_MESSAGE("Successfuly connected to the MySql database");

    m_IsConnected = True;

    return True;
}

// Disconnect from the database server
void MySqlDb::disconnect()
{
    if (m_IsConnected)
        m_IsConnected = False;

    if (m_pDB)
    {
        mysql_close(m_pDB);
        m_pDB = nullptr;
    }
}

// Try to maintain the connection established
void MySqlDb::pingConnection()
{
    if (m_pDB)
		mysql_ping(m_pDB);
}

// Instanciate a new DbQuery object
DbQuery* MySqlDb::newDbQuery(const String &name, const CString &query)
{
    return new MySqlQuery(m_pDB,name,query);
}

// Virtual destructor
MySqlQuery::~MySqlQuery()
{
    for (Int32 i = 0; i < m_inputs.getSize(); ++i)
    {
        deletePtr(m_inputs[i]);
    }

    for (Int32 i = 0; i < m_outputs.getSize(); ++i)
    {
        deletePtr(m_outputs[i]);
    }

	if (m_stmt)
    {
        if (m_prepareMetaResult)
            mysql_free_result(m_prepareMetaResult);

        //if (m_prepareMetaParam)
        //    mysql_free_result(m_prepareMetaParam);

		mysql_stmt_close(m_stmt);
    }
}

void MySqlQuery::setArrayUInt8(UInt32 attr, const ArrayUInt8 &v)
{
    if (attr >= m_numParam)
        O3D_ERROR(E_IndexOutOfRange("Input attribute id"));

    if (m_inputs[attr])
        deletePtr(m_inputs[attr]);

    if (v.getSize() < (1 << 8))
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_ARRAY_UINT8, DbVariable::TINY_ARRAY, (UInt8*)&v);
    else if (v.getSize() < (1 << 16))
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_ARRAY_UINT8, DbVariable::ARRAY, (UInt8*)&v);
    else if (v.getSize() < (1 << 24))
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_ARRAY_UINT8, DbVariable::MEDIUM_ARRAY, (UInt8*)&v);
    else
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_ARRAY_UINT8, DbVariable::LONG_ARRAY, (UInt8*)&v);

    DbVariable &var = *m_inputs[attr];

    enum_field_types dbtype = (enum_field_types)0;
    unsigned long dbsize = 0;

    mapType(var.getType(), dbtype, dbsize);

    memset(&m_param_bind[attr], 0, sizeof(MYSQL_BIND));

    m_param_bind[attr].buffer_type = (enum_field_types)dbtype;
    m_param_bind[attr].buffer = (void*)var.getObjectPtr();
    m_param_bind[attr].buffer_length = var.getObjectSize();

    m_param_bind[attr].is_null = 0;

    var.setLength(var.getObjectSize());
    m_param_bind[attr].length = (unsigned long*)var.getLengthPtr();

    m_needBind = True;
}

void MySqlQuery::setSmartArrayUInt8(UInt32 attr, const SmartArrayUInt8 &v)
{
    if (attr >= m_numParam)
        O3D_ERROR(E_IndexOutOfRange("Input attribute id"));

    if (m_inputs[attr])
        deletePtr(m_inputs[attr]);

    if (v.getSizeInBytes() < (1 << 8))
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_SMART_ARRAY_UINT8, DbVariable::TINY_ARRAY, (UInt8*)&v);
    else if (v.getSizeInBytes() < (1 << 16))
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_SMART_ARRAY_UINT8, DbVariable::ARRAY, (UInt8*)&v);
    else if (v.getSizeInBytes() < (1 << 24))
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_SMART_ARRAY_UINT8, DbVariable::MEDIUM_ARRAY, (UInt8*)&v);
    else
        m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_SMART_ARRAY_UINT8, DbVariable::LONG_ARRAY, (UInt8*)&v);

    DbVariable &var = *m_inputs[attr];

    enum_field_types dbtype = (enum_field_types)0;
    unsigned long dbsize = 0;

    mapType(var.getType(), dbtype, dbsize);

    memset(&m_param_bind[attr], 0, sizeof(MYSQL_BIND));

    m_param_bind[attr].buffer_type = (enum_field_types)dbtype;
    m_param_bind[attr].buffer = (void*)var.getObjectPtr();
    m_param_bind[attr].buffer_length = var.getObjectSize();

    m_param_bind[attr].is_null = 0;

    var.setLength(var.getObjectSize());
    m_param_bind[attr].length = (unsigned long*)var.getLengthPtr();

    m_needBind = True;
}

void MySqlQuery::setInStream(UInt32 attr, const InStream &v)
{
    O3D_ERROR(E_InvalidOperation("Not yet implemented"));
}

void MySqlQuery::setBool(UInt32 attr, Bool v)
{
    if (attr >= m_numParam)
        O3D_ERROR(E_IndexOutOfRange("Input attribute id"));

    if (m_inputs[attr])
        deletePtr(m_inputs[attr]);

    m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_BOOL, DbVariable::BOOLEAN, (UInt8*)&v);
    DbVariable &var = *m_inputs[attr];

    enum_field_types dbtype = (enum_field_types)0;
    unsigned long dbsize = 0;

    mapType(var.getType(), dbtype, dbsize);

    memset(&m_param_bind[attr], 0, sizeof(MYSQL_BIND));

    m_param_bind[attr].buffer_type = (enum_field_types)dbtype;
    m_param_bind[attr].buffer = (void*)var.getObjectPtr();
    m_param_bind[attr].buffer_length = var.getObjectSize();

    m_param_bind[attr].is_null = 0;

    var.setLength(dbsize);
    m_param_bind[attr].length = (unsigned long*)var.getLengthPtr();

    m_needBind = True;
}

void MySqlQuery::setInt32(UInt32 attr, Int32 v)
{
    if (attr >= m_numParam)
        O3D_ERROR(E_IndexOutOfRange("Input attribute id"));

    if (m_inputs[attr])
        deletePtr(m_inputs[attr]);

    m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_INT32, DbVariable::INT32, (UInt8*)&v);
    DbVariable &var = *m_inputs[attr];

    enum_field_types dbtype = (enum_field_types)0;
    unsigned long dbsize = 0;

    mapType(var.getType(), dbtype, dbsize);

    memset(&m_param_bind[attr], 0, sizeof(MYSQL_BIND));

    m_param_bind[attr].buffer_type = (enum_field_types)dbtype;
    m_param_bind[attr].buffer = (void*)var.getObjectPtr();
    m_param_bind[attr].buffer_length = var.getObjectSize();

    //m_param_bind[attr].is_null_value = False; @see if we support null value
    m_param_bind[attr].is_null = 0;

    var.setLength(dbsize);
    m_param_bind[attr].length = (unsigned long*)var.getLengthPtr();

    m_needBind = True;
}

void MySqlQuery::setUInt32(UInt32 attr, UInt32 v)
{
    if (attr >= m_numParam)
        O3D_ERROR(E_IndexOutOfRange("Input attribute id"));

    if (m_inputs[attr])
        deletePtr(m_inputs[attr]);

    m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_INT32, DbVariable::UINT32, (UInt8*)&v);
    DbVariable &var = *m_inputs[attr];

    enum_field_types dbtype = (enum_field_types)0;
    unsigned long dbsize = 0;

    mapType(var.getType(), dbtype, dbsize);

    memset(&m_param_bind[attr], 0, sizeof(MYSQL_BIND));

    m_param_bind[attr].buffer_type = (enum_field_types)dbtype;

    m_param_bind[attr].buffer = (void*)var.getObjectPtr();
    m_param_bind[attr].buffer_length = var.getObjectSize();

    m_param_bind[attr].is_null = 0;
    m_param_bind[attr].is_unsigned = True;

    var.setLength(dbsize);
    m_param_bind[attr].length = (unsigned long*)var.getLengthPtr();

    m_needBind = True;
}

void MySqlQuery::setCString(UInt32 attr, const CString &v)
{
    if (attr >= m_numParam)
        O3D_ERROR(E_IndexOutOfRange("Input attribute id"));

    if (m_inputs[attr])
        deletePtr(m_inputs[attr]);

    m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_CSTRING, DbVariable::VARCHAR, (UInt8*)&v);
    DbVariable &var = *m_inputs[attr];

    enum_field_types dbtype = (enum_field_types)0;
    unsigned long dbsize = 0;

    mapType(var.getType(), dbtype, dbsize);

    memset(&m_param_bind[attr], 0, sizeof(MYSQL_BIND));

    m_param_bind[attr].buffer_type = (enum_field_types)dbtype;
    m_param_bind[attr].buffer = (void*)var.getObjectPtr();
    m_param_bind[attr].buffer_length = var.getObjectSize()-1;

    m_param_bind[attr].is_null = 0;

    var.setLength(var.getObjectSize()-1);
    m_param_bind[attr].length = (unsigned long*)var.getLengthPtr();

    m_needBind = True;
}

void MySqlQuery::setTimestamp(UInt32 attr, const Date &v)
{
    if (attr >= m_numParam)
        O3D_ERROR(E_IndexOutOfRange("Input attribute id"));

    if (m_inputs[attr])
        deletePtr(m_inputs[attr]);

    m_inputs[attr] = new MySqlDbVariable(DbVariable::IT_DATE, DbVariable::TIMESTAMP, (UInt8*)&v);
    DbVariable &var = *m_inputs[attr];

    MYSQL_TIME *mysqlTime = (MYSQL_TIME*)var.getObjectPtr();
    memset(mysqlTime, 0, sizeof(MYSQL_TIME));

    mysqlTime->day = v.mday + 1;
    mysqlTime->hour = v.hour;
    mysqlTime->minute = v.minute;
    mysqlTime->month = v.month + 1;
    mysqlTime->second = v.second;
    mysqlTime->time_type = MYSQL_TIMESTAMP_DATETIME;
    mysqlTime->year = v.year;

    enum_field_types dbtype = (enum_field_types)0;
    unsigned long dbsize = 0;

    mapType(var.getType(), dbtype, dbsize);

    memset(&m_param_bind[attr], 0, sizeof(MYSQL_BIND));

    m_param_bind[attr].buffer_type = (enum_field_types)dbtype;
    m_param_bind[attr].buffer = (void*)var.getObjectPtr();
    m_param_bind[attr].buffer_length = dbsize;

    m_param_bind[attr].is_null = 0;

    var.setLength(dbsize);
    m_param_bind[attr].length = (unsigned long*)var.getLengthPtr();

    m_needBind = True;
}

UInt32 MySqlQuery::getOutAttr(const CString &name)
{
    auto it = m_outputNames.find(name);
    if (it != m_outputNames.end())
        return it->second;
    else
        O3D_ERROR(E_InvalidParameter("Uknown output attribute name"));
}

const DbVariable &MySqlQuery::getOut(const CString &name) const
{
    auto it = m_outputNames.find(name);
    if (it != m_outputNames.end())
        return *m_outputs[it->second];
    else
        O3D_ERROR(E_InvalidParameter("Uknown output attribute name"));
}

const DbVariable &MySqlQuery::getOut(UInt32 attr) const
{
    if (attr < (UInt32)m_outputs.getSize())
        return *m_outputs[attr];
    else
        O3D_ERROR(E_IndexOutOfRange("Output attribute is out of range"));
}

// Prepare the query. Can do nothing if not preparation is needed
void MySqlQuery::prepareQuery()
{
    O3D_ASSERT(m_pDB != nullptr);
	if (m_pDB)
	{
		m_stmt = mysql_stmt_init(m_pDB);
        O3D_ASSERT(m_stmt != nullptr);

        int result = mysql_stmt_prepare(m_stmt, m_query.getData(), (unsigned long)m_query.length());
		if (result != 0)
        {
            String err = mysql_stmt_error(m_stmt);

            mysql_stmt_close(m_stmt);
            m_stmt = nullptr;

            O3D_ERROR(E_MySqlError(err));
        }

        // inputs
        m_numParam = mysql_stmt_param_count(m_stmt);
        for (UInt32 i = 0; i < m_numParam; ++i)
        {
            memset(&m_param_bind[i], 0, sizeof(MYSQL_BIND));
        }

//        m_prepareMetaParam = mysql_stmt_param_metadata(m_stmt);
//        if (!m_prepareMetaParam)
//            O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

//        mysql_field_seek(m_prepareMetaParam, 0);

          MYSQL_FIELD *field;
          int id = 0;
//        while ((field = mysql_fetch_field(m_prepareMetaParam)) != nullptr)
//        {
//            memset(&m_param_bind[id], 0, sizeof(MYSQL_BIND));

//            m_param_length[id] = 0/*TODO*/;

//            m_param_bind[id].buffer = /*TODO*/nullptr;

//            m_param_bind[id].buffer_length = /*TODO*/0;//var.getObjectSize();

//            m_param_bind[id].is_null = 0;
//            m_param_bind[id].length = &m_param_length[id];

//            ++id;
//        }

        // outputs
        m_prepareMetaResult = mysql_stmt_result_metadata(m_stmt);
        if (m_prepareMetaResult)
        {
            mysql_field_seek(m_prepareMetaResult, 0);

            UInt32 maxSize;
            DbVariable::IntType intType;
            DbVariable::VarType varType;

            id = 0;
            while ((field = mysql_fetch_field(m_prepareMetaResult)) != nullptr)
            {
                memset(&m_result_bind[id], 0, sizeof(MYSQL_BIND));

                unmapType(field->type, maxSize, intType, varType);

                m_outputNames.insert(std::make_pair(field->name, id));

                m_outputs[id] = new MySqlDbVariable(intType, varType, maxSize);
                DbVariable &var = *m_outputs[id];

                m_result_bind[id].buffer = (void*)var.getObjectPtr();
                m_result_bind[id].buffer_type = field->type;
                m_result_bind[id].buffer_length = var.getObjectSize();

                m_result_bind[id].is_null = (my_bool*)var.getIsNullPtr();
                m_result_bind[id].error = (my_bool*)var.getErrorPtr();

                var.setLength(var.getObjectSize());
                m_result_bind[id].length = (unsigned long*)var.getLengthPtr();

                ++id;
            }
        }

        m_needBind = True;
	}
}

// Unbind the current bound DbAttribute
void MySqlQuery::unbind()
{
    if (m_stmt)
    {
        for (Int32 i = 0; i < m_inputs.getSize(); ++i)
        {
            deletePtr(m_inputs[i]);
        }

//		m_is_error.destroy();
//		m_param_bind.destroy();
//		m_length.destroy();
//		m_param_length.destroy();
//		m_param_bind.destroy();
//		m_result_bind.destroy();
    }
}

MySqlQuery::MySqlQuery(MYSQL *pDb, const String &name, const CString &query) :
    m_name(name),
    m_query(query),
    m_numParam(0),
    m_numRow(0),
    m_currRow(0),
    m_pDB(pDb),
    m_stmt(nullptr),
    //m_prepareMetaParam(nullptr),
    m_prepareMetaResult(nullptr)
{
    prepareQuery();
}

// Execute the query on the current bound DbAttribute and store the result in the DbAttribute
void MySqlQuery::execute()
{
    O3D_ASSERT(m_stmt != nullptr);

    if (m_stmt)
	{
        m_numRow = 0;
        m_currRow = 0;

        // bind if necessary
        if (m_needBind)
        {
            if (mysql_stmt_bind_param(m_stmt, &m_param_bind[0]) != 0)
                O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

            m_needBind = False;
        }

        if (mysql_stmt_execute(m_stmt) != 0)
            O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

        if (mysql_stmt_bind_result(m_stmt,&m_result_bind[0]) != 0)
            O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

        if (mysql_stmt_store_result(m_stmt) != 0)
            O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

        m_numRow = mysql_stmt_num_rows(m_stmt);
    }
}

void MySqlQuery::update()
{
    O3D_ASSERT(m_stmt != nullptr);

    if (m_stmt)
    {
        m_numRow = 0;
        m_currRow = 0;

        // bind if necessary
        if (m_needBind)
        {
            if (mysql_stmt_bind_param(m_stmt, &m_param_bind[0]) != 0)
                O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

            m_needBind = False;
        }

        if (mysql_stmt_execute(m_stmt) != 0)
            O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

        m_numRow = mysql_stmt_affected_rows(m_stmt);
    }
}

UInt32 MySqlQuery::getNumRows()
{
    return m_numRow;
}

UInt64 MySqlQuery::getGeneratedKey() const
{
    O3D_ASSERT(m_stmt != nullptr);
    if (m_stmt)
    {
        UInt64 id = mysql_stmt_insert_id(m_stmt);
        return id;
    }

    return 0;
}

// Fetch the results (outputs values) into the DbAttribute. Can be called in a while for each entry of the result.
Bool MySqlQuery::fetch()
{
    O3D_ASSERT(m_stmt != nullptr);
    if (m_stmt)
    {
        int res = mysql_stmt_fetch(m_stmt);

        if (res == MYSQL_NO_DATA)
            return False;
        else if (res == MYSQL_DATA_TRUNCATED)
            O3D_WARNING("MYSQL_DATA_TRUNCATED");
        else if (res != 0)
            O3D_ERROR(E_MySqlError(mysql_stmt_error(m_stmt)));

		// validate arrays and strings results
        UInt32 co = m_outputs.getSize();
        for (size_t i = 0; i < co; ++i)
		{
            DbVariable &var = *m_outputs[i];

            if (var.isNull())
                continue;

            // string
            if (var.getIntType() == DbVariable::IT_ARRAY_CHAR)
			{
                ArrayChar *array = (ArrayChar*)var.getObject();

                // add a terminal zero
                array->setSize(var.getLength()+1);
                (*array)[array->getSize()-1] = 0;
			}
            // array
            else if (var.getIntType() == DbVariable::IT_ARRAY_UINT8)
			{
                ArrayUInt8 *array = (ArrayUInt8*)var.getObject();
                array->setSize(var.getLength());
			}
            // date
            else if (var.getIntType() == DbVariable::IT_DATE)
            {
                Date *date = (Date*)var.getObject();
                MYSQL_TIME *mysqlTime = (MYSQL_TIME*)var.getObjectPtr();
                date->day = Day(mysqlTime->day - 1);
                date->hour = mysqlTime->hour;
                date->minute = mysqlTime->minute;
                date->month = Month(mysqlTime->month - 1);
                date->second = mysqlTime->second;
                date->year = mysqlTime->year;
            }
		}

        ++m_currRow;
        return True;
	}

    return False;
}

UInt32 MySqlQuery::tellRow()
{
    if (m_stmt)
        return m_currRow;
    else
        return 0;
}

void MySqlQuery::seekRow(UInt32 row)
{
    if (row >= m_numRow)
        O3D_ERROR(E_IndexOutOfRange("Row number"));

    if (m_stmt)
    {
        mysql_stmt_data_seek(m_stmt, row);
        m_currRow = row;
    }
}

void MySql::init()
{
    if (!ms_mySqlLibState)
    {
        mysql_server_init(0, nullptr, nullptr);

        Application::registerObject("o3d::MySql", nullptr);
        ms_mySqlLibState = True;
    }
}

void MySql::quit()
{
    if (ms_mySqlLibState)
    {
        if (ms_mySqlLibRefCount != 0)
            O3D_ERROR(E_InvalidOperation("Trying to quit mysql library but some database still exists"));

        mysql_server_end();

        Application::unregisterObject("o3d::MySql");
        ms_mySqlLibState = False;
    }
}

void MySqlQuery::mapType(
        DbVariable::VarType type,
        enum_field_types &mysqltype,
        unsigned long &mysqlsize)
{
    switch (type)
    {
    case DbVariable::BOOLEAN:
        mysqltype = MYSQL_TYPE_TINY;
        mysqlsize = 1;
        break;

    case DbVariable::CHAR:
        mysqltype = MYSQL_TYPE_TINY;
        mysqlsize = 1;
        break;

    case DbVariable::INT8:
        mysqltype = MYSQL_TYPE_TINY;
        mysqlsize = 1;
        break;

    case DbVariable::INT16:
        mysqltype = MYSQL_TYPE_SHORT;
        mysqlsize = 2;
        break;

    case DbVariable::INT24:
        mysqltype = MYSQL_TYPE_INT24;
        mysqlsize = 3;
        break;

    case DbVariable::INT32:
        mysqltype = MYSQL_TYPE_LONG;
        mysqlsize = 4;
        break;

        //MYSQL_TYPE_LONGLONG

    case DbVariable::FLOAT32:
        mysqltype = MYSQL_TYPE_FLOAT;
        mysqlsize = 4;
        break;

    case DbVariable::FLOAT64:
        mysqltype = MYSQL_TYPE_DOUBLE;
        mysqlsize = 8;
        break;

    case DbVariable::VARCHAR:
        mysqltype = MYSQL_TYPE_VARCHAR;
        mysqlsize = 1;
        break;

    case DbVariable::STRING:
        mysqltype = MYSQL_TYPE_VAR_STRING;
        mysqlsize = 1;
        break;

    case DbVariable::TINY_ARRAY:
        mysqltype = MYSQL_TYPE_TINY_BLOB;
        mysqlsize = 1;
        break;

    case DbVariable::ARRAY:
        mysqltype = MYSQL_TYPE_BLOB;
        mysqlsize = 1;
        break;

    case DbVariable::MEDIUM_ARRAY:
        mysqltype = MYSQL_TYPE_MEDIUM_BLOB;
        mysqlsize = 1;
        break;

    case DbVariable::LONG_ARRAY:
        mysqltype = MYSQL_TYPE_LONG_BLOB;
        mysqlsize = 1;
        break;

    case DbVariable::TIMESTAMP:
        mysqltype = MYSQL_TYPE_TIMESTAMP;
        mysqlsize = sizeof(MYSQL_TIME);
        break;

    default:
        O3D_ASSERT(0);
        break;
    };
}

void MySqlQuery::unmapType(
        enum_field_types mysqltype,
        UInt32 &maxSize,
        DbVariable::IntType &intType,
        DbVariable::VarType &varType)
{
    switch (mysqltype)
    {
    case MYSQL_TYPE_TINY:
        intType = DbVariable::IT_INT8;
        varType = DbVariable::INT8;
        maxSize = 1;
        break;

    case MYSQL_TYPE_SHORT:
        intType = DbVariable::IT_INT16;
        varType = DbVariable::INT16;
        maxSize = 2;
        break;

    case MYSQL_TYPE_INT24:
        intType = DbVariable::IT_INT32;
        varType = DbVariable::INT32;
        maxSize = 4;
        break;

    case MYSQL_TYPE_LONG:
        intType = DbVariable::IT_INT32;
        varType = DbVariable::INT32;
        maxSize = 4;
        break;

        //MYSQL_TYPE_LONGLONG

    case MYSQL_TYPE_FLOAT:
        intType = DbVariable::IT_FLOAT;
        varType = DbVariable::FLOAT32;
        maxSize = 4;
        break;

    case MYSQL_TYPE_DOUBLE:
        intType = DbVariable::IT_DOUBLE;
        varType = DbVariable::FLOAT64;
        maxSize = 8;
        break;

    case MYSQL_TYPE_VARCHAR:
        intType = DbVariable::IT_ARRAY_CHAR;
        varType = DbVariable::ARRAY;
        maxSize = 256;
        break;

    case MYSQL_TYPE_VAR_STRING:
        intType = DbVariable::IT_ARRAY_CHAR;
        varType = DbVariable::ARRAY;
        maxSize = 256;
        break;

    case MYSQL_TYPE_TINY_BLOB:
        intType = DbVariable::IT_ARRAY_UINT8;
        varType = DbVariable::TINY_ARRAY;
        maxSize = 256;
        break;

    case MYSQL_TYPE_BLOB:
        intType = DbVariable::IT_ARRAY_UINT8;
        varType = DbVariable::ARRAY;
        maxSize = 4096;
        break;

    case MYSQL_TYPE_MEDIUM_BLOB:
        intType = DbVariable::IT_ARRAY_UINT8;
        varType = DbVariable::MEDIUM_ARRAY;
        maxSize = 4096;
        break;

    case MYSQL_TYPE_LONG_BLOB:
        intType = DbVariable::IT_ARRAY_UINT8;
        varType = DbVariable::LONG_ARRAY;
        maxSize = 4096;
        break;

    case MYSQL_TYPE_TIMESTAMP:
        intType = DbVariable::IT_DATE;
        varType = DbVariable::TIMESTAMP;
        maxSize = sizeof(MYSQL_TIME);
        break;

    default:
        O3D_ASSERT(0);
        break;
    };
}
