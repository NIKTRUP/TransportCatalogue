
#ifndef STAT_READER_H
#define STAT_READER_H
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "../include/request_handler.h"
#include "../json/json_builder.h"
#include "../include/transport_router.h"

namespace transport {

    void GetRoute(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out);

    void GetStop(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out);

    void PrintQuery(const QueryType& type, const std::string& name, const TransportCatalogue& catalogue, std::ostream& out = std::cout);

    std::vector<std::pair<QueryType, std::string>> ParseOutputQuery(std::istream& in);

    void EqualLogFiles(std::filesystem::path my_log, std::filesystem::path test, std::ostream& out);

    json::Node GetNotFound(size_t id);

    json::Node GetStop(size_t id, const std::string& name, const TransportCatalogue& catalogue);

    json::Node GetBus(size_t id, const std::string& name, const TransportCatalogue& catalogue);

    json::Node GetDict(size_t id, const DictRenderer& renderer);

    json::Node GetRoute(size_t id, const std::string& from,
                        const std::string& to, const TransportCatalogue& catalogue, const TransportRouter* router );

    void PrintQuery(const std::vector<StatRequest> requests, const TransportCatalogue& catalogue,
                    const DictRenderer& renderer, std::ostream& out = std::cout, const TransportRouter* router = nullptr);
}

#endif // STAT_READER_H
