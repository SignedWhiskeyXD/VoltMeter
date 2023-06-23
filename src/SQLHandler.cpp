//
// Created by WhiskeyXD on 2023/6/23.
//

#include <spdlog/spdlog.h>
#include "SQLHandler.h"

bool SQLHandler::InsertOneRecord(SQLVoltRecord& newRecord) {
    try{
        session << "INSERT INTO voltMeterRecord (tag, recordTime, value) VALUES(?, ?, ?)",
                use(newRecord.tag),
                use(newRecord.recordTime),
                use(newRecord.value),
                now;
    }catch (const Poco::Exception& exception){
        spdlog::error("{}: {}", exception.name(), exception.what());
        return false;
    }
    spdlog::info("Inserted into database");
    return true;
}

std::vector<std::string> SQLHandler::SelectTags() {
    std::vector<std::string> ret;
    try{
        session << "SELECT DISTINCT tag FROM voltMeterRecord",
                into(ret),
                now;
    }catch (const Poco::Exception& exception){
        spdlog::error("{}: {}", exception.name(), exception.what());
        ret.clear();
    }
    return ret;
}

std::vector<SQLVoltRecord> SQLHandler::SelectRecordByTag(std::string tag) {
    std::vector<SQLVoltRecord> ret;
    try{
        session << "SELECT * FROM voltMeterRecord WHERE tag = ?",
                use(tag),
                into(ret),
                now;
    }catch (const Poco::Exception& exception){
        spdlog::error("{}: {}", exception.name(), exception.what());
        ret.clear();
    }
    return ret;
}

bool SQLHandler::RemoveRecordByTag(std::string tag) {
    try{
        session << "DELETE FROM voltMeterRecord WHERE tag = ?",
                use(tag),
                now;
    }catch (const Poco::Exception& exception){
        spdlog::error("{}: {}", exception.name(), exception.what());
        return false;
    }
    spdlog::warn("Tag {} records removed!", tag);
    return true;
}
