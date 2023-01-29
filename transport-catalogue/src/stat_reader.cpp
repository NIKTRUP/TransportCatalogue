#include "include/stat_reader.h"

namespace tc {

    using namespace detail;

    void PrintRoute(const std::string& name, TransportCatalogue& catalogue, std::ostream& out){
        RouteInfo info;

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
                      << info.num_of_stops << " stops on route, "
                      << info.num_of_unique_stops << " unique stops, "
                      << info.route_length << " route length, "
                      << info.curvature << " curvature" << std::endl;
        }
    }

    void PrintStop(const std::string& name, TransportCatalogue& catalogue, std::ostream& out){
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

    void PrintQuery(const QueryType& type, const std::string& name, TransportCatalogue& catalogue, std::ostream& out){
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
        size_t number_queries = detail::CheckNumberLines(in);
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
                line2 = line2.substr(0, line2.size()-1);
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

}
