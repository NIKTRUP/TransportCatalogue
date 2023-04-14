#include "include/request_handler.h"

namespace tc {

    namespace detail {

        using namespace std;

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

            std::pair<domain::RouteType, std::vector<domain::Stop>> ParseRouteQuery(std::string_view line){
                using namespace domain;

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
                    domain::Route route;
                    route.name = std::move(route_name);
                    route.route_type = type;
                    for(auto& stop : route_stops){
                        route.stops.push_back(catalogue.FindStop(stop.name));
                    }
                    catalogue.AddRoute(route);
                }

                ParseDistances(catalogue, stop_to_distances);
            }

            void HandleStop(const json::Dict& map, std::unordered_map<string, std::vector<std::pair<std::string, size_t>>>& stop_to_distances, TransportCatalogue& catalogue){
                domain::Stop stop = {map.at("name"s).AsString(), {map.at("latitude"s).AsDouble(), map.at("longitude"s).AsDouble()} };
                catalogue.AddStop(stop);
                std::vector<std::pair<std::string, size_t>> stops_distances;
                auto it_distances = map.find("road_distances"s);
                if(it_distances != map.end()){
                   auto distances = it_distances->second.AsDict();
                   for(auto& [stop_name, distance ]: distances){
                       stops_distances.push_back({stop_name, static_cast<size_t>(distance.AsInt())});
                   }
                   stop_to_distances[stop.name] = stops_distances;
                }
            }

            void HandleRound(const json::Dict& map, TransportCatalogue& catalogue){
                domain::Route route;
                route.name = map.at("name"s).AsString();
                route.route_type = map.at("is_roundtrip"s).AsBool() ? domain::RouteType::CIRCLE : domain::RouteType::LINEAR;

                json::Array stops = map.at("stops").AsArray();
                for(auto& stop_node: stops){
                    auto stop_name = stop_node.AsString();
                    route.stops.push_back(catalogue.FindStop(stop_name));
                }
                catalogue.AddRoute(route);
            }

            void HandleBaseRequest(const json::Array& array , TransportCatalogue& catalogue){
                std::vector<json::Node> routes;
                std::unordered_map<string, std::vector<std::pair<std::string, size_t>>> stop_to_distances;

                for(auto& node : array){
                    if(node.IsDict()){
                        auto map = node.AsDict();
                        std::string type = map.at("type"s).AsString();
                        if(type == "Stop"s || type == "stop"s){
                            HandleStop(map, stop_to_distances, catalogue);
                        }

                        if(type == "Bus"s || type == "bus"s){
                            routes.push_back(map);
                        }

                    }else{
                        JSON_STRUCTURAL_ERROR;
                    }
                }

                for(auto& [stop_name, distances] :stop_to_distances){
                    for(auto& stop_distance : distances){
                        catalogue.SetDistance({catalogue.FindStop(stop_name), catalogue.FindStop(stop_distance.first)},
                                              stop_distance.second);
                    }
                }

                for(auto& node : routes){
                    HandleRound(node.AsDict(), catalogue);
                }

            }

            std::vector<StatRequest> HandleStatRequest(const json::Array& array){
                std::vector<StatRequest> requests;

                auto init_request = [&](const QueryType& type, StatRequest& request, json::Dict& map){
                    request.type = type;
                    request.name = map.at("name").AsString();
                    requests.push_back(request);
                };

                for(auto& query: array){
                    auto map = query.AsDict();
                    StatRequest request;

                    auto type = map.at("type").AsString();
                    request.id = static_cast<size_t>(map.at("id").AsInt());
                    if(type == "Stop" || type == "stop" ){
                        init_request(QueryType::Stop, request, map);
                    }else if(type == "Bus" || type == "bus" ){
                        init_request(QueryType::Route, request, map);
                    }else if(type == "Map" || type == "map"){
                        request.type = QueryType::Map;
                        requests.push_back(request);
                    }
                }
                return requests;
            }

            void InitDoubleParams(RenderSettings& settings, const json::Dict& map){

                auto init_param = [&](const std::string& param_name, double& param_value){
                    param_value = map.count(param_name) ? map.at(param_name).AsDouble(): param_value;
                    assert(param_value >= 0);
                };

                init_param("width", settings.width);
                init_param("height", settings.height);
                init_param("padding", settings.padding);
                init_param("line_width", settings.line_width);
                init_param("stop_radius", settings.stop_radius);
                init_param("underlayer_width", settings.underlayer_width);

            }

            void InitIntParams(RenderSettings& settings, const json::Dict& map){

                auto init_param = [&](const std::string& param_name, int& param_value){
                    param_value = map.count(param_name) ? map.at(param_name).AsInt(): param_value;
                    assert(param_value >= 0);
                };

                init_param("bus_label_font_size", settings.bus_label_font_size);
                init_param("stop_label_font_size", settings.stop_label_font_size);
            }

            void InitArrayParams(RenderSettings& settings, const json::Dict& map){
                auto bus_offset = map.count("bus_label_offset") ? map.at("bus_label_offset").AsArray() : json::Array{};
                if(!bus_offset.empty()){
                    settings.bus_label_offset = {bus_offset[0].AsDouble(), bus_offset[1].AsDouble()};
                }
                auto stop_offset = map.count("stop_label_offset") ? map.at("stop_label_offset").AsArray() : json::Array{};
                if(!stop_offset.empty()){
                    settings.stop_label_offset = {stop_offset[0].AsDouble(), stop_offset[1].AsDouble()};
                }
            }

            RenderSettings HandleRenderSettings(const json::Dict& map){
                RenderSettings settings;
                InitDoubleParams(settings, map);
                InitIntParams(settings, map);
                InitArrayParams(settings, map);

                auto underlayer_color_map = map.count("underlayer_color") ? map.at("underlayer_color") : nullptr;

                if(!underlayer_color_map.IsNull()){
                    if(underlayer_color_map.IsArray()){
                        auto array_color = underlayer_color_map.AsArray();
                        if(array_color.size() == 4){
                            settings.underlayer_color = svg::Rgba(array_color[0].AsInt(), array_color[1].AsInt(), array_color[2].AsInt(), array_color[3].AsDouble());
                        }else if(array_color.size() == 3){
                            settings.underlayer_color = svg::Rgb(array_color[0].AsInt(), array_color[1].AsInt(), array_color[2].AsInt());
                        }else{
                            JSON_STRUCTURAL_ERROR;
                        }
                    }else if(underlayer_color_map.IsString()){
                        settings.underlayer_color = underlayer_color_map.AsString();
                    }
                }


                auto color_palette_map = map.count("color_palette") ? map.at("color_palette").AsArray() : json::Array{};
                if(!color_palette_map.empty()){
                    for(auto& color_palette: color_palette_map){
                        if(color_palette.IsArray()){
                            auto array_color = color_palette.AsArray();
                            if(array_color.size() == 4){
                                settings.color_palette.push_back(svg::Rgba(array_color[0].AsInt(), array_color[1].AsInt(), array_color[2].AsInt(), array_color[3].AsDouble()));
                            }else if(array_color.size() == 3){
                                settings.color_palette.push_back(svg::Rgb(array_color[0].AsInt(), array_color[1].AsInt(), array_color[2].AsInt()));
                            }else{
                                JSON_STRUCTURAL_ERROR;
                            }
                        }else if(color_palette.IsString()){
                            settings.color_palette.push_back(color_palette.AsString());
                        }
                    }
                }
                settings.is_init = true;

                return settings;
            }

            std::pair<std::vector<StatRequest>, RenderSettings> ParseJson(std::istream& in, TransportCatalogue& catalogue){
                std::vector<StatRequest> out;
                std::unordered_map<string, string> route_name_stops;
                std::unordered_map<string, string> stop_to_distances;
                RenderSettings renderer;

                json::Document base_stat_requests = json::Load(in);
                json::Node root = base_stat_requests.GetRoot();
                if(root.IsDict()){
                    json::Dict map_requests = root.AsDict();
                    auto it_base = map_requests.find("base_requests"s);
                    if(it_base != map_requests.end()){
                        if(it_base->second.IsArray()){
                            json::Array array_base = it_base->second.AsArray();
                            HandleBaseRequest(array_base, catalogue);
                        }
                    }
                    auto it_stat = map_requests.find("stat_requests"s);
                    if(it_stat != map_requests.end()){
                        if(it_stat->second.IsArray()){
                            json::Array array_stat = it_stat->second.AsArray();
                            out = HandleStatRequest(array_stat);
                        }
                    }
                    auto it_render = map_requests.find("render_settings"s);
                    if(it_render != map_requests.end()){
                        if(it_render->second.IsDict()){
                            json::Dict dict_setings = it_render->second.AsDict();
                            renderer = HandleRenderSettings(dict_setings);
                        }
                    }

                }else{
                    JSON_STRUCTURAL_ERROR;
                }
                return {out, renderer};
            }

            void StrToLower(std::string& s) {
                std::transform(s.begin(), s.end(), s.begin(),
                               [](unsigned c){ return std::tolower(c); }
                              );
            }

            // на будущее
            std::pair<std::vector<StatRequest>, RenderSettings> ReadFile(std::filesystem::path file_path,  TransportCatalogue& catalogue, FileType type){
                using namespace std;

                std::ifstream in(file_path);
                if(in.is_open()){

                    switch (type) {
                    case FileType::XML:
                                throw std::runtime_error(" Пока функциональность не реализована ");
                        break;
                    case FileType::JSON:
                                return ParseJson(in, catalogue);
                        break;
                    case FileType::INI:
                                throw std::runtime_error(" Пока функциональность не реализована ");
                        break;
                    default:
                        break;
                    }
                }else{
                    throw std::invalid_argument(" Ошибка: Такого файла не существует. Проверьте путь к файлу: "s +  file_path.string());
                }
                in.close();
                return {std::vector<StatRequest>{}, RenderSettings{}};
            }
    }

    void ReadByConsole(TransportCatalogue& catalogue){
        detail::ParseTxt(std::cin, catalogue);
    }

    std::pair<std::vector<StatRequest>,const RenderSettings> ReadFile(std::filesystem::path file_path, TransportCatalogue& catalogue){
        using namespace std;
        auto extension = file_path.extension().string();
        detail::StrToLower(extension);

        if(extension == ".xml"s){ return detail::ReadFile(file_path, catalogue, FileType::XML); }
        else if(extension == ".json"s){ return detail::ReadFile(file_path, catalogue, FileType::JSON); }
        else if(extension == ".ini"s){ return detail::ReadFile(file_path, catalogue, FileType::INI); }
        else{
            throw std::invalid_argument(" Ошибка: Данный формат не поддерживается. Используйте форматы: .xml, .json, .ini Ваш формат: "s + extension);
        }
    }
}
