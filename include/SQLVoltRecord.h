//
// Created by WhiskeyXD on 2023/6/23.
//

#ifndef QVOLTMETER_SQLVOLTRECORD_H
#define QVOLTMETER_SQLVOLTRECORD_H

#include <Poco/DateTime.h>
#include <Poco/Data/TypeHandler.h>
#include <Poco/DateTimeFormatter.h>
#include <string>
#include <utility>

struct SQLVoltRecord{
    SQLVoltRecord():
        id(0), value(0.0), temp(0.0) {}

    SQLVoltRecord(std::string tagStr, const Poco::DateTime& time, double val, double tempVal):
        id(0), tag(std::move(tagStr)), recordTime(time), value(val), temp(tempVal) {}

    int id;
    std::string tag;
    Poco::DateTime recordTime;
    double value;
    double temp;
};

namespace Poco::Data{

    template<>
    class TypeHandler<SQLVoltRecord>{
    public:
        static std::size_t size()
        {
            return 5;
        }

        static void bind(std::size_t pos, const SQLVoltRecord& voltRecord,
                         AbstractBinder::Ptr pBinder, AbstractBinder::Direction dir)
        {
            TypeHandler<int>::bind(pos++, voltRecord.id, pBinder, dir);
            TypeHandler<std::string>::bind(pos++, voltRecord.tag, pBinder, dir);
            TypeHandler<Poco::DateTime>::bind(pos++, voltRecord.recordTime, pBinder, dir);
            TypeHandler<double>::bind(pos++, voltRecord.value, pBinder, dir);
            TypeHandler<double>::bind(pos++, voltRecord.temp, pBinder, dir);
        }

        static void extract(std::size_t pos, SQLVoltRecord& voltRecord,
                            const SQLVoltRecord& deflt, AbstractExtractor::Ptr pExtr)
        {
            TypeHandler<int>::extract(pos++, voltRecord.id, deflt.id, pExtr);
            TypeHandler<std::string>::extract(pos++, voltRecord.tag, deflt.tag, pExtr);
            TypeHandler<Poco::DateTime>::extract(pos++, voltRecord.recordTime, deflt.recordTime, pExtr);
            TypeHandler<double>::extract(pos++, voltRecord.value, deflt.value, pExtr);
            TypeHandler<double>::extract(pos++, voltRecord.temp, deflt.temp, pExtr);
        }

        static void prepare(std::size_t pos, const SQLVoltRecord& voltRecord, AbstractPreparator::Ptr pPrep)
        {
            TypeHandler<int>::prepare(pos++, voltRecord.id, pPrep);
            TypeHandler<std::string>::prepare(pos++, voltRecord.tag, pPrep);
            TypeHandler<Poco::DateTime>::prepare(pos++, voltRecord.recordTime, pPrep);
            TypeHandler<double>::prepare(pos++, voltRecord.value, pPrep);
            TypeHandler<double>::prepare(pos++, voltRecord.temp, pPrep);
        }
    };

}

#endif //QVOLTMETER_SQLVOLTRECORD_H
