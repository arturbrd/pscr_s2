#pragma once

#include <nlohmann/json.hpp>
#include <ostream>

struct Record {
    int point_id;
    double lat;
    double lon;
    double temp_c;
    double wind_mps;
    int clouds_pct;
    long timestamp;
    int valid;
};

struct Data {
    int task_id;
    long timestamp;
    int count;
    std::vector<Record> records;
};

struct WeatherMap {
    long timestamp;
    std::vector<Record> records;
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Record,
    point_id, lat, lon, temp_c, wind_mps,
    clouds_pct, timestamp, valid)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Data,
    task_id, timestamp, count, records)

std::ostream& operator<<(std::ostream& os, const Record& r);
std::ostream& operator<<(std::ostream& os, const Data& r);