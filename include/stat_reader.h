
#ifndef STAT_READER_H
#define STAT_READER_H
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "include/request_handler.h"
#include "json/json_builder.h"

namespace tc {

    void GetRoute(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out);

    void GetStop(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out);

    void PrintQuery(const QueryType& type, const std::string& name, const TransportCatalogue& catalogue, std::ostream& out = std::cout);

    std::vector<std::pair<QueryType, std::string>> ParseOutputQuery(std::istream& in);

    void EqualLogFiles(std::filesystem::path my_log, std::filesystem::path test, std::ostream& out);

    json::Node GetNotFound(size_t id);

    void GetNotFound(size_t id, std::ostream& out);

    json::Node GetStop(size_t id, const std::string& name, const TransportCatalogue& catalogue);

    json::Node GetRoute(size_t id, const std::string& name,const TransportCatalogue& catalogue);

    json::Node GetMap(size_t id, const DictRenderer& renderer);

    void PrintQuery(const std::vector<StatRequest> requests,const TransportCatalogue& catalogue, const DictRenderer& renderer ,std::ostream& out = std::cout);
}

#endif // STAT_READER_H
