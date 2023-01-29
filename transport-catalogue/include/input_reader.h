
#ifndef INPUT_READER_H
#define INPUT_READER_H

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

namespace tc {

    namespace detail {

        void LeftTrim(std::string_view& line);

        void RightTrim(std::string_view& line);

        void Trim(std::string_view& line);

        std::vector<std::string_view> SplitIntoWords(std::string_view str);

        enum class FileType{
            TXT,
            JSON,
            XML,
            INI
        };

        std::string StrToLower(std::string s);


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

        std::pair<RouteType, std::vector<Stop>> ParseRouteQuery(std::string_view line);

        void ParseDistances(TransportCatalogue& catalogue, const std::unordered_map<std::string, std::string>& stop_to_distances);

        void ParseTxt(std::istream& in, TransportCatalogue& catalogue);

        void ReadFile(std::filesystem::path file_path,  TransportCatalogue& catalogue, FileType type);
    }

    void ReadByConsole(TransportCatalogue& catalogue);

    void ReadFile(std::filesystem::path path, TransportCatalogue& catalogue);

}

#endif // INPUT_READER_H
