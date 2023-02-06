#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <filesystem>
#include <utility>
#include <execution>
#include <fstream>
#include <string>
#include <string_view>
#include <cctype>
#include <cassert>
#include <sstream>
#include <unordered_set>
#include "include/transport_catalogue.h"
#include "include/domain.h"
#include "include/json.h"
#include "include/map_renderer.h"

namespace tc {

    #define JSON_STRUCTURAL_ERROR throw json::ParsingError(" Неправильная структура файла Json "s);

    enum class QueryType{
      Route,
      Stop,
      Map
    };

    enum class FileType{
        JSON,
        XML,
        INI
    };

    struct StatRequest{
        size_t id;
        QueryType type;
        std::string  name;
    };

    namespace detail {

            void LeftTrim(std::string_view& line);

            void RightTrim(std::string_view& line);

            void Trim(std::string_view& line);

            std::vector<std::string_view> SplitIntoWords(std::string_view str);

            template<typename Iterator>
            std::string StrConcat(Iterator begin, Iterator end, const std::string& separator = ""){
                std::string str;
                size_t size = 0;
                for_each(begin, end, [&](auto ch){
                    size += ch.size() + separator.size();
                });
                str.reserve(size);
                bool is_first = true;
                for(auto it = begin; it != end; ++it){
                    if(is_first){
                        str.append(std::string(*it));
                        is_first = false;
                        continue;
                    }
                    str.append(separator).append(std::string(*it));
                }
                return str;
            }


            template<typename StringViewContainer>
            std::string StrConcat(StringViewContainer strs, const std::string& separator = ""){
                return StrConcat(strs.begin(), strs.end(), separator);
            }

            template <typename InputStream>
            size_t CheckNumberLines(InputStream& in){
                int64_t number_lines;
                std::string line;
                std::getline(in, line);
                auto vec = SplitIntoWords(line);
                if(vec.size() > 1){ throw std::invalid_argument(" Ошибка: Неправильный формат ввода "); };
                number_lines = std::stoi(std::string(vec[0]));
                number_lines >= 0 ? (0) : throw std::out_of_range(" Ошибка: Число команд не может быть меньше нуля " + std::to_string(number_lines) + " < 0 ");
                return number_lines;
            }

            std::pair<std::string, std::string> ParseTypeQueryAndName(std::string_view line);

            std::pair<std::string, geo::Coordinates> ParseStopQuery(std::string_view line);

            std::pair<domain::RouteType, std::vector<domain::Stop>> ParseRouteQuery(std::string_view line);

            void ParseDistances(TransportCatalogue& catalogue, const std::unordered_map<std::string, std::string>& stop_to_distances);

            void ParseTxt(std::istream& in, TransportCatalogue& catalogue);

            void HandleStop(const json::Dict& map, std::unordered_map<std::string, std::vector<std::pair<std::string, size_t>>>& stop_to_distances,
                            TransportCatalogue& catalogue);

            void HandleRound(const json::Dict& map, TransportCatalogue& catalogue);

            void HandleBaseRequest(const json::Array& array , TransportCatalogue& catalogue);

            std::vector<StatRequest> HandleStatRequest(const json::Array& array);

            void InitDoubleParams(RenderSettings& settings, const json::Dict& map);

            void InitIntParams(RenderSettings& settings, const json::Dict& map);

            void InitArrayParams(RenderSettings& settings, const json::Dict& map);

            RenderSettings HandleRenderSettings(const json::Dict& map);


            std::pair<std::vector<StatRequest>, RenderSettings> ParseJson(std::istream& in, TransportCatalogue& catalogue);

            void StrToLower(std::string& s);

            std::pair<std::vector<StatRequest>, RenderSettings> ReadFile(std::filesystem::path file_path,  TransportCatalogue& catalogue, FileType type);
    }

    void ReadByConsole(TransportCatalogue& catalogue);

    std::pair<std::vector<StatRequest>, const RenderSettings> ReadFile(std::filesystem::path path, TransportCatalogue& catalogue);
}

#endif // REQUEST_HANDLER_H
