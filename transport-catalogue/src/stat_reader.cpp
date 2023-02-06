#include "include/stat_reader.h"

namespace tc {

    using namespace detail;

        void PrintRoute(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out){
            domain::RouteInfo info;

            bool check = false;
            try {
                info = catalogue.GetRouteInfo(name);
                check = true;
            }  catch (...) {
                out<< "Bus " << name << ": not found" << std::endl;
            }
            if(check == true){
                out << std::setprecision(6);
                out << "Bus " << info.name << ": "
                          << info.stop_count << " stops on route, "
                          << info.unique_stop_count << " unique stops, "
                          << info.route_length << " route length, "
                          << info.curvature << " curvature" << std::endl;
            }
        }

        void PrintStop(const std::string& name, const TransportCatalogue& catalogue, std::ostream& out){
            auto busses_on_stop = catalogue.GetBussesOnStop(name);

            if(busses_on_stop.has_value()){
                if((*busses_on_stop).empty()){
                    out<< "Stop " << name << ": no buses" << std::endl;
                }else{
                    out << std::setprecision(6);
                    out << "Stop " << name << ": buses";
                       for(auto& bus : *busses_on_stop){
                            out << " " << std::string(bus);
                        }
                         out << std::endl;
                }
            }else{
                out<< "Stop " << name << ": not found" << std::endl;
            }
        }

        void PrintQuery(const QueryType& type, const std::string& name, const TransportCatalogue& catalogue, std::ostream& out){
            switch (type) {
            case QueryType::Route:
                PrintRoute(name, catalogue, out);
                break;
            case QueryType::Stop:
                PrintStop(name, catalogue, out);
                break;
            default:
                break;
            }
        }

        std::vector<std::pair<QueryType, std::string>> ParseOutputQuery(std::istream& in){
            using namespace std::literals;

            std::string line;
            size_t number_queries = CheckNumberLines(in);
            std::vector<std::pair<QueryType, std::string>> queries;
            queries.reserve(number_queries);
            for(size_t i = 0; i < number_queries; ++i){
                 getline(in, line);
                 std::string_view line_view = line;
                 LeftTrim(line_view);
                 auto end_type = std::min(line_view.find_first_of(" "s), line_view.size());
                 std::string_view type = line_view.substr(0, end_type);
                 auto str_type = std::string(type);

                 QueryType type_mode;

                 if(str_type == "Bus"s || str_type == "bus"s){
                     type_mode = QueryType::Route;
                 } else if(str_type == "Stop"s || str_type == "stop"s){
                     type_mode = QueryType::Stop;
                 }else{
                     throw std::invalid_argument(" Ошибка: Запрос неправлиьно построен. Нет такого типа запросов "s);
                 }

                 line_view.remove_prefix(end_type+1);
                 Trim(line_view);

                 queries.push_back({type_mode ,std::string(line_view)});
            }
            return queries;
        }


        void EqualLogFiles(std::filesystem::path my_log, std::filesystem::path test, std::ostream& out){
            using namespace std::literals;
            std::ifstream in1(my_log.string());
            std::ifstream in2(test.string());

            std::string line1;
            std::string line2;
            if(in1.is_open() && in2.is_open()){
                out << "Файл: " << test.filename() << std::endl;
                for(size_t i = 1; !in1.eof(); ++i){
                    std::getline(in1, line1);
                    std::getline(in2, line2);
                    if(line2[line2.size()-1] == '\r'){
                        line2 = line2.substr(0, line2.size()-1);
                    }
                    if(line1.size() != line2.size() || line1 != line2){
                        out << "Строка: " << i << "\n"
                            << "Мой вывод: " << line1 << "\n"
                            << "Эталон: " << line2 << "\n";
                    }
                }
            }else{
                std::cerr << "Ошибка: файлы " << my_log.string() << " и " << test.string() << " не открыты " << std::endl;
            }
        }

        void DoSpace(std::ostream& out, size_t amount_t){
            for(size_t i = 0 ; i < amount_t; ++i){
                out << ' ';
            }
        }

        void PrintStop(size_t id, const std::string& name, const TransportCatalogue& catalogue, std::ostream& out){
            using namespace std::literals;
            DoSpace(out, 1);
            out <<"{\n";

            auto print_stops = [&](auto str, bool& first){
                if(first){
                    out << "\"" << str << "\"";
                    first = false;
                }else{
                    out << ", \"" << str << "\"";
                }
            };

            auto busses_on_stop = catalogue.GetBussesOnStop(name);
            if(busses_on_stop.has_value()) {
                DoSpace(out, 2);
                out << "\"buses\": [\n" ;
                if((*busses_on_stop).empty()){

                }else{
                    DoSpace(out, 5);
                    bool first = true;
                    for(auto& bus : *busses_on_stop){
                        print_stops(std::string(bus), first);
                    }
                }
                out << "\n";
                DoSpace(out, 3);
                out << "],\n";
                DoSpace(out, 3);
                out << "\"request_id\": " << id << "\n";
            }else{
                DoSpace(out, 2);
                out << "\"request_id\": " << id << ",\n";
                DoSpace(out, 2);
                out << "\"error_message\": \"not found\"\n";
            }
            DoSpace(out, 1);
            out << "}";
        }

        void PrintRoute(size_t id, const std::string& name, const TransportCatalogue& catalogue, std::ostream& out){
            using namespace std::literals;
            domain::RouteInfo info;
            bool check = false;

            DoSpace(out, 1);
            out << "{\n";
            try {
                info = catalogue.GetRouteInfo(name);
                check = true;
            }  catch (const std::out_of_range& ) {
                DoSpace(out, 2);
                out << "\"request_id\": " << id << ",\n";
                DoSpace(out, 2);
                out << "\"error_message\": \"not found\"\n";
            }
            if(check == true){
                out << std::setprecision(6);
                DoSpace(out, 3);
                out << "\"curvature\": " << info.curvature << ",\n";
                DoSpace(out, 3);
                out << "\"request_id\": " << id << ",\n";
                DoSpace(out, 3);
                out << "\"route_length\": " << info.route_length << ",\n";
                DoSpace(out, 3);
                out << "\"stop_count\": " << info.stop_count << ",\n";
                DoSpace(out, 3);
                out << "\"unique_stop_count\": " << info.unique_stop_count << "\n";
            }
            DoSpace(out, 1);
            out << "}";
        }

        void PrintMap(size_t id, const MapRenderer& renderer, std::ostream& out){
            DoSpace(out, 1);
            out << "{\n";
            DoSpace(out, 2);
            out << "\"map\": \"";
            std::ostringstream ss;
            renderer.Print(ss);
            out << svg::Escape(ss.str());
            out << "\",\n";
            DoSpace(out, 2);
            out << "\"request_id\": " << id << "\n";
            DoSpace(out, 1);
            out << "}";
        }

        void PrintQuery(const std::vector<StatRequest> requests, const TransportCatalogue& catalogue, const MapRenderer& renderer, std::ostream& out){
            using namespace std::literals;
            bool fisrt = true;

            auto print = [&](const StatRequest& request){
                if(request.type == QueryType::Stop){
                    PrintStop(request.id, request.name, catalogue, out);
                }else if(request.type == QueryType::Route){
                    PrintRoute(request.id, request.name, catalogue, out);
                }else if(request.type == QueryType::Map){
                    if(renderer.GetSettings().is_init == true){
                        PrintMap(request.id, renderer, out);
                    }
                }
            };

            out << "[\n"s;
            for(const auto& request: requests){
                if(fisrt){
                    print(request);
                    fisrt = false;
                    continue;
                }
                out << ",\n";
                print(request);
            }
            out << "\n]"s;
        }

}
