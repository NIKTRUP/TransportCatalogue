
#ifndef STAT_READER_H
#define STAT_READER_H
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "include/request_handler.h"

namespace tc {

    void PrintRoute(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out);

    void PrintStop(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out);

    void PrintQuery(const QueryType& type, const std::string& name, const TransportCatalogue& catalogue, std::ostream& out = std::cout);

    std::vector<std::pair<QueryType, std::string>> ParseOutputQuery(std::istream& in);

    void EqualLogFiles(std::filesystem::path my_log, std::filesystem::path test, std::ostream& out);

    void DoSpace(std::ostream& out, size_t amount_t);

    void PrintStop(size_t id, const std::string& name, const TransportCatalogue& catalogue, std::ostream& out);

    void PrintRoute(size_t id, const std::string& name,const TransportCatalogue& catalogue, std::ostream& out);

    void PrintMap(size_t id, const MapRenderer& renderer, std::ostream& out);

    void PrintQuery(const std::vector<StatRequest> requests,const TransportCatalogue& catalogue, const MapRenderer& renderer ,std::ostream& out = std::cout);
}

#endif // STAT_READER_H
