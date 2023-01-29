#include "include/input_reader.h"

namespace tc {
    using namespace std;

    namespace detail {

        void LeftTrim(string_view& line) {
            line.remove_prefix(std::min(line.find_first_not_of(' '), line.size()));
        }

        // Как это оптимизировать (долго будет работать)
        void RightTrim(string_view& line) {
            size_t pos = 0;
            for(auto it = line.rbegin(); it != line.rend(); ++it){
                if(!std::isspace(*it)){
                   pos = std::distance(line.rbegin(), it);
                   break;
                }
            }
            line.remove_suffix(std::min(pos, line.size()));
        }

        // Как это оптимизировать (долго будет работать)
        void Trim(string_view& line) {
            LeftTrim(line);
            RightTrim(line);
        }

        std::vector<std::string_view> SplitIntoWords(std::string_view str) {
            vector<string_view> result;
            str.remove_prefix(std::min(str.find_first_not_of(' '), str.size()));

            while (!str.empty()) {
                size_t space = str.find(' ');
                result.push_back(space == str.npos ? str.substr(0) : str.substr(0, space));
                str.remove_prefix(result.back().size());
                str.remove_prefix(std::min(str.find_first_not_of(' '), str.size()));
            }
            return result;
        }

        std::string StrToLower(std::string s) {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned c){ return std::tolower(c); } // correct
                          );
            return s;
        }

        pair<string, string> ParseTypeQueryAndName(string_view line){
            auto end_query_type_and_name = line.find(":"s);
            auto query_type_and_name = line.substr(0, end_query_type_and_name);
            LeftTrim(query_type_and_name); // удаляет пробелы слева
            auto end_query_type = query_type_and_name.find_first_of(" "s);
            string query_type = std::string(query_type_and_name.substr(0, end_query_type));
            auto raw_name = query_type_and_name.substr(end_query_type + 1);// подстрока с позиции после типа запроса и пробела
            Trim(raw_name);
            auto name = std::string(raw_name);  // StrConcat(SplitIntoWords(raw_name), " "s)
            return {query_type, name};
        }

        std::pair<std::string, geo::Coordinates> ParseStopQuery(string_view line){
            auto end_latitude = line.find(",");
            auto str_latitude = line.substr(0, end_latitude);

            auto trim = [](string_view str){
                str.remove_prefix(std::min(str.find_first_not_of(' '), str.size()));
                str.remove_suffix(std::min(str.find_first_of(' '), str.size()));
            };
            trim(str_latitude);

            line.remove_prefix(end_latitude + 1);

            auto eld_longitude = std::min(line.find(","), line.size());
            auto str_longitude = line.substr(0, eld_longitude);
            trim(str_longitude);
            line.remove_prefix(std::min(eld_longitude + 1, line.size()));

            double latitude = std::stod(std::string(str_latitude));
            double longitude = std::stod(std::string(str_longitude));

            if(latitude < -90.0 || latitude > 90.0){
                throw std::out_of_range("Ошибка: Широта не может выходить за пределы отрезка [-90.0, 90.0]. latitude = " + std::to_string(latitude));
            }

            if(longitude < -180.0 || longitude > 180.0){
                throw std::out_of_range("Ошибка: Долгота не может выходить за пределы отрезка [-180.0, 180.0]. longitude = " + std::to_string(longitude));
            }

            return {std::string(line), geo::Coordinates{latitude, longitude}};
        }

        std::pair<RouteType, std::vector<Stop>> ParseRouteQuery(std::string_view line){
            RouteType type = RouteType::UNKNOWN;
            std::vector<Stop> stops;
            char sep = ' ';
            size_t first_pos = 0;
            for (size_t i = 0; i < line.size(); ++i) {
                if(line[i] == '-'){
                    type = RouteType::LINEAR;
                    sep = '-';
                    first_pos = i;
                    break;
                }else if(line[i] == '>'){
                    type = RouteType::CIRCLE;
                    sep = '>';
                    first_pos = i;
                    break;
                }
            }
            string_view first_stop_name = line.substr(0, first_pos);
            Trim(first_stop_name);
            stops.push_back({std::string(first_stop_name) , geo::Coordinates{}}); // StrConcat(SplitIntoWords(first_stop_name), " "s)
            line.remove_prefix(std::min(first_pos + 1, line.size()));

            if(type != RouteType::UNKNOWN){
                while(!line.empty()){
                    size_t pos = std::min(line.find(sep), line.size());
                    string_view stop_name = line.substr(0, pos);
                    Trim(stop_name);
                    stops.push_back({std::string(stop_name), geo::Coordinates{}});  //  StrConcat(SplitIntoWords(stop_name), " "s)
                    line.remove_prefix(std::min(pos + 1, line.size()));
                }
            }else{
                throw std::invalid_argument(" Ошибка: Запрос на добавление в базу данных неправильно построен. Измените сепаратор для путей ");
            }
            return {type, stops};
        }

        void ParseDistances(TransportCatalogue& catalogue, const std::unordered_map<string, string>& stop_to_distances){

            for(auto& [stop_name, str_distances] : stop_to_distances){
                std::string_view str_view_distances = str_distances;

                while(!str_view_distances.empty()){
                    auto pos_sep = std::min(str_view_distances.find(","), str_view_distances.size());
                    std::string_view line = str_view_distances.substr(0, pos_sep);
                    std::string_view distance_stop = line;

                    auto pos_to = distance_stop.find("to"s);

                    std::string_view distance_view = distance_stop.substr(0,pos_to);
                    Trim(distance_view);
                    std::string_view name_to_stop = distance_stop.substr(pos_to +2);
                    Trim(name_to_stop);

                    catalogue.SetDistance({catalogue.FindStop(std::string(stop_name)), catalogue.FindStop(std::string(name_to_stop))},
                                          std::stoul(std::string(distance_view.substr(0, distance_view.size() - 1))));

                    str_view_distances.remove_prefix(std::min(pos_sep + 1, str_view_distances.size()));
                }
            }
        }

        void ParseTxt(std::istream& in, TransportCatalogue& catalogue){
            std::unordered_map<string, string> route_name_stops;
            std::unordered_map<string, string> stop_to_distances;
            std::string line;

            size_t number_lines = CheckNumberLines(in);
            for(size_t i = 0; i < number_lines; ++i){
                getline(in, line);
                auto [query_type, name] = ParseTypeQueryAndName(line);
                if(query_type == "Stop"s || query_type == "stop"){
                    auto [distance_stops, coordinate] = ParseStopQuery( line.substr(line.find(":"s)+1) );
                    if(!distance_stops.empty()){
                         stop_to_distances[name] = std::move(distance_stops);
                    }
                    catalogue.AddStop({std::move(name), std::move(coordinate)});
                }else if(query_type == "Bus"s || query_type == "bus"){
                    if(!route_name_stops.count(name)){
                        route_name_stops[std::move(name)] = line.substr(line.find(":"s)+1);
                    }
                }else{
                    throw std::invalid_argument(" Ошибка: Запрос неправлиьно построен. Нет такого типа запросов ");
                }
            }

            for(auto& [route_name, raw_stops] : route_name_stops){
                auto [type, route_stops] = ParseRouteQuery(raw_stops);
                Route route;
                route.name = std::move(route_name);
                route.route_type = type;
                for(auto& stop : route_stops){
                    route.stops.push_back(catalogue.FindStop(stop.name));
                }
                catalogue.AddRoute(route);
            }

            ParseDistances(catalogue, stop_to_distances);
        }

        // на будущее
        void ReadFile(std::filesystem::path file_path,  TransportCatalogue& catalogue, FileType type){
            using namespace std;


            std::ifstream in(file_path);
            if(in.is_open()){

                switch (type) {
                case FileType::TXT:{
                        ParseTxt(in, catalogue);
                    }
                    break;
                case FileType::XML:

                    break;
                case FileType::JSON:

                    break;
                case FileType::INI:

                    break;
                default:
                    break;
                }
            }else{
                throw std::invalid_argument(" Ошибка: Такого файла не существует. Проврьте путь к файлу: "s +  file_path.string());
            }
            in.close();
        }
    }

    void ReadByConsole(TransportCatalogue& catalogue){
        detail::ParseTxt(std::cin, catalogue);
    }

    // TODO ::
    void ReadFile(std::filesystem::path file_path, TransportCatalogue& catalogue){
        using namespace std;

        auto extension = detail::StrToLower(file_path.extension().string());

        if(extension == ".txt"s){ return ReadFile(file_path, catalogue, detail::FileType::TXT); }
        else if(extension == ".xml"s){ return ReadFile(file_path, catalogue, detail::FileType::XML); }
        else if(extension == ".json"s){ return ReadFile(file_path, catalogue, detail::FileType::JSON); }
        else if(extension == ".ini"s){ return ReadFile(file_path, catalogue, detail::FileType::INI); }
        else{
            throw std::invalid_argument(" Ошибка: Данный формат не поддерживается. Используйте форматы: .xml, .json, .ini, .txt. Ваш формат: "s + extension);
        }
    }
}
