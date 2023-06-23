//
// Created by WhiskeyXD on 2023/6/23.
//

#ifndef QVOLTMETER_SQLHANDLER_H
#define QVOLTMETER_SQLHANDLER_H

#include <Poco/Data/Session.h>
#include "SQLVoltRecord.h"
using namespace Poco::Data::Keywords;

class SQLHandler{
public:
    SQLHandler(): session(Poco::Data::Session("SQLite", "voltdata.db3"))
    {
        //session << "DROP TABLE IF EXISTS voltMeterRecord", now;
        session << "CREATE TABLE IF NOT EXISTS voltMeterRecord"
                   "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "tag VARCHAR, "
                   "recordTime TIME, "
                   "value DOUBLE)", now;
    }

    SQLHandler(const SQLHandler& rhs) = delete;

    static Poco::DateTime getCurrentTime(int local = 28800){
        Poco::DateTime ret;
        ret.makeLocal(28800);
        return ret;
    }

    SQLHandler& operator=(const SQLHandler& rhs) = delete;

    bool InsertOneRecord(SQLVoltRecord& newRecord);

    bool RemoveRecordByTag(std::string tag);

    std::vector<std::string> SelectTags();

    std::vector<SQLVoltRecord> SelectRecordByTag(std::string tag);

private:
    Poco::Data::Session session;
};

#endif //QVOLTMETER_SQLHANDLER_H
