/**
 * @file mysqldb.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MYSQLDB_H
#define _O3D_MYSQLDB_H

#include "mysql.h"

#include <o3d/core/database.h>
#include <o3d/core/date.h>
#include <o3d/core/datetime.h>
#include <o3d/core/memorydbg.h>

#include <mysql/mysql.h>

namespace o3d {
namespace mysql {

/**
 * @brief MySql
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-02
 */
class O3D_PGSQL_API MySql
{
public:

    //! Init the mysql library. Must be called before creating a MySqlDb.
    static void init();

    //! Cleanup the mysql library. Must be called after deleting any MySqlDb.
    static void quit();
};

/**
 * @brief MySqlDb database client.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-11-08
 */
class O3D_MYSQL_API MySqlDb : public Database
{
public:

	//! Default ctor
    MySqlDb();

	//! Virtual dtor
    virtual ~MySqlDb();

	//! Connect to a database
    virtual Bool connect(
        const String &host,
        o3d::UInt32 port,
        const String &database,
        const String &user = "",
        const String &password = "",
        Bool keepPassord = True);

	//! Disconnect from the database server
    virtual void disconnect();

	//! Try to maintain the connection established
    virtual void pingConnection();

protected:

	//! Instanciate a new DbQuery object
    virtual DbQuery* newDbQuery(const String &name, const CString &query);

	MYSQL *m_pDB;
};

/**
 * @brief MySqlQuery database prepared query. Can't be deleted outside of the MySqlDb.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-11-08
 */
class O3D_MYSQL_API MySqlQuery : public DbQuery
{
	friend class MySqlDb;

public:

	//! Virtual destructor
	virtual ~MySqlQuery();

    //! Set an input variable as ArrayUInt8. The array is duplicated.
    virtual void setArrayUInt8(UInt32 attr, const ArrayUInt8 &v);

    //! Set an input variable as SmartArrayUInt8. The array is duplicated.
    virtual void setSmartArrayUInt8(UInt32 attr, const SmartArrayUInt8 &v);

    //! Set an input variable as InStream. Data are not duplicated. The stream must stay opened.
    virtual void setInStream(UInt32 attr, const InStream &v);

    //! Set an input variable as Bool.
    virtual void setBool(UInt32 attr, Bool v);

    //! Set an input variable as Int32.
    virtual void setInt32(UInt32 attr, Int32 v);

    //! Set an input variable as UInt32.
    virtual void setUInt32(UInt32 attr, UInt32 v);

    //! Set an input variable as CString.
    virtual void setCString(UInt32 attr, const CString &v);

    //! Set an input variable as Date.
    virtual void setDate(UInt32 attr, const Date &date);

	//! Set an input variable as Timestamp.
    virtual void setTimestamp(UInt32 attr, const DateTime &date);

    //! Get an output attribute id by its name.
    virtual UInt32 getOutAttr(const CString &name);

    //! Get an output variable by its name.
    const DbVariable& getOut(const CString &name) const;

    //! Get an output variable by its index.
    const DbVariable& getOut(UInt32 attr) const;

    //! Execute the query for a SELECT.
    virtual void execute();

    //! Execute the query for an UPDATE, INSERT, or DELETE.
    virtual void update();

    //! Get the number of affected or result rows after an execute or update.
    virtual UInt32 getNumRows();

    //! Get the result variable (ie for an auto increment).
    virtual UInt64 getGeneratedKey() const;

    /**
     * @brief fetch Fetch the results.
     * Can be called in a while for each entry of the result.
     * @return True until there is results row
     */
    virtual Bool fetch();

    //! Get the row position when fetching.
    virtual UInt32 tellRow();

    //! Set the row position when fetching (seek 0 for reset).
    virtual void seekRow(UInt32 row);

    //! Unbind the current input attributes.
    virtual void unbind();

protected:

	//! Default ctor
	MySqlQuery(
		MYSQL *pDb,
		const String &name,
        const CString &query);

	//! Prepare the query. Can do nothing if not preparation is needed
	void prepareQuery();

    String m_name;
    CString m_query;

    UInt32 m_numParam;
    UInt32 m_numRow;
    UInt32 m_currRow;

    std::map<CString, UInt32> m_outputNames;

    TemplateArray<DbVariable*> m_inputs;
    TemplateArray<DbVariable*> m_outputs;

	MYSQL *m_pDB;
	MYSQL_STMT *m_stmt;

	TemplateArray<MYSQL_BIND> m_param_bind;
	TemplateArray<MYSQL_BIND> m_result_bind;

    Bool m_needBind;

    //MYSQL_RES *m_prepareMetaParam;
    MYSQL_RES *m_prepareMetaResult;

    void mapType(DbVariable::VarType type, enum_field_types &mysqltype, unsigned long &mysqlsize);

    void unmapType(
            enum_field_types mysqltype,
            UInt32 &maxSize,
            DbVariable::IntType &intType,
            DbVariable::VarType &varType);
};

} // namespace mysql
} // namespace o3d

#endif // _O3D_MYSQLDB_H
