/**
 * @file main.cpp
 * @brief Main entry of the mysql sample (need to O3DMySql module).
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2004-01-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include <o3d/core/memorymanager.h>

#include <o3d/core/appwindow.h>
#include <o3d/core/main.h>

#include <o3d/mysql/mysqldb.h>

#include <cstdio>
#include <iostream>

using namespace o3d;
using namespace o3d::mysql;

class MySqlTest
{
public:

// Program main
static Int32 main()
{
    MySql::init();

	MySqlDb mysql;

	std::cout << "Connecting to the MySql db..." << std::endl;

	if (!mysql.connect("localhost","o3dtest","o3dtest","o3dtest"))
	{
		std::cout << "Unable to connect to the DB" << std::endl;
		return -1;
	}

	std::cout << "Successfully connected:" << std::endl;

	std::cout << "Create an STMT Request..." << std::endl;
    DbQuery *query = mysql.registerQuery("test","SELECT Login, PlayersId FROM user WHERE UId = ?");

    query->setInt32(0, 2);

    query->execute();

    std::cout << "Result(s) for UId = 2 :" << std::endl;
    while (query->fetch())
    {
        std::cout << "String= " << query->getOut("Login").asCString().getData() << std::endl;
        std::cout << "BlobSize= " << (String() << query->getOut("PlayersId"/*bl*/).asArrayUInt8().getSize()).getData() << " Data= ";
        for (int i = 0; i < query->getOut("PlayersId").asArrayUInt8().getSize(); ++i)
        {
            std::cout << query->getOut("PlayersId").asArrayUInt8().get(i);
        }
        std::cout << std::endl;
    }

    query->setInt32(0, 3);
    query->execute();

    std::cout << "Result(s) for UId = 3 :" << std::endl;
    while (query->fetch())
    {
        std::cout << "String= " << query->getOut("Login").asCString().getData() << std::endl;
        std::cout << "BlobSize= " << (String() << query->getOut("PlayersId"/*bl*/).asArrayUInt8().getSize()).getData() << " Data= ";
        for (int i = 0; i < query->getOut("PlayersId").asArrayUInt8().getSize(); ++i)
        {
            std::cout << query->getOut("PlayersId").asArrayUInt8().get(i);
        }
        std::cout << std::endl;
    }

    std::cout << "Create another STMT Request..." << std::endl;
    DbQuery *query2 = mysql.registerQuery("test2","SELECT UId FROM test2 WHERE Login = ?");

    query2->setCString(0, "test");
    query2->execute();

    std::cout << "Result(s) for Login = test :" << std::endl;
    while (query2->fetch())
    {
        std::cout << "UId= " << query2->getOut("UId").asUInt32() << std::endl;
    }

    query2->setCString(0, "test0");
    query2->execute();

    std::cout << "Result(s) for Login = test0 :" << std::endl;
    while (query2->fetch())
    {
        std::cout << "UId= " << query2->getOut("UId").asUInt32() << std::endl;
    }

	mysql.disconnect();

    MySql::quit();
    return 0;
}
};

// We Call our application in console mode
O3D_CONSOLE_MAIN(MySqlTest, O3D_DEFAULT_CLASS_SETTINGS)
