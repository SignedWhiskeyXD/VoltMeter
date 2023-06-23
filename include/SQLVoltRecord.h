//
// Created by WhiskeyXD on 2023/6/23.
//

#ifndef QVOLTMETER_SQLVOLTRECORD_H
#define QVOLTMETER_SQLVOLTRECORD_H

#include <Poco/DateTime.h>
#include <Poco/Data/TypeHandler.h>
#include <Poco/DateTimeFormatter.h>
#include <string>

struct SQLVoltRecord{
    SQLVoltRecord():
        id(0), value(0.0) {}

    SQLVoltRecord(std::string tagStr, Poco::DateTime time, double val):
        id(0), tag(tagStr), recordTime(time), value(val) {}

    int id;
    std::string tag;
    Poco::DateTime recordTime;
    double value;
};

namespace Poco::Data{

    template<>
    class TypeHandler<SQLVoltRecord>{
    public:
        static std::size_t size()
        {
            return 4;
        }

        static void bind(std::size_t pos, const SQLVoltRecord& voltRecord, AbstractBinder::Ptr pBinder, AbstractBinder::Direction dir)
        {
            TypeHandler<int>::bind(pos++, voltRecord.id, pBinder, dir);
            TypeHandler<std::string>::bind(pos++, voltRecord.tag, pBinder, dir);
            TypeHandler<Poco::DateTime>::bind(pos++, voltRecord.recordTime, pBinder, dir);
            TypeHandler<double>::bind(pos++, voltRecord.value, pBinder, dir);
        }

        static void extract(std::size_t pos, SQLVoltRecord& voltRecord, const SQLVoltRecord& deflt, AbstractExtractor::Ptr pExtr)
        {
            TypeHandler<int>::extract(pos++, voltRecord.id, deflt.id, pExtr);
            TypeHandler<std::string>::extract(pos++, voltRecord.tag, deflt.tag, pExtr);
            TypeHandler<Poco::DateTime>::extract(pos++, voltRecord.recordTime, deflt.recordTime, pExtr);
            TypeHandler<double>::extract(pos++, voltRecord.value, deflt.value, pExtr);
        }

        static void prepare(std::size_t pos, const SQLVoltRecord& voltRecord, AbstractPreparator::Ptr pPrep)
        {
            TypeHandler<int>::prepare(pos++, voltRecord.id, pPrep);
            TypeHandler<std::string>::prepare(pos++, voltRecord.tag, pPrep);
            TypeHandler<Poco::DateTime>::prepare(pos++, voltRecord.recordTime, pPrep);
            TypeHandler<double>::prepare(pos++, voltRecord.value, pPrep);
        }
    };

}

#endif //QVOLTMETER_SQLVOLTRECORD_H
