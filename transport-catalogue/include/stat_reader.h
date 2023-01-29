
#ifndef STAT_READER_H
#define STAT_READER_H
#include <string>
#include <vector>
#include "include/input_reader.h"

namespace tc {

    enum class QueryType{
      Route,
      Stop
    };

    void PrintRoute(const std::string& name, TransportCatalogue& catalogue, std::ostream& out = std::cout);

    void PrintStop(const std::string& name, TransportCatalogue& catalogue, std::ostream& out = std::cout);

    void PrintQuery(const QueryType& type, const std::string& name, TransportCatalogue& catalogue, std::ostream& out = std::cout);

    std::vector<std::pair<QueryType, std::string>> ParseOutputQuery(std::istream& in);

    void EqualLogFiles(std::filesystem::path my_log, std::filesystem::path test, std::ostream& out);

}

#endif // STAT_READER_H
