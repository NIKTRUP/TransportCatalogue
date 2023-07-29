//#include <iostream>
//#include "include/tests.h"
//#include "include/stat_reader.h"
//
//using namespace std;
//using namespace std::filesystem;
//using namespace svg;
//using namespace transport;

//int main(){
//
//#ifndef NDEBUG
//    {
//        LOG_DURATION("TestProject");
//        tests::TestProject();
//    }
//    {
//        LOG_DURATION("TestExamples");
//        tests::TestExamples();
//    }
//#endif
//    TransportCatalogue catalogue;
//    auto [requests, render_settings, routing_settings] = ReadFile("../tests/tsI_case/tsI_case_input.json"s, catalogue);
//    // auto [requests, render_settings, routing_settings] = detail::ParseJson(std::cin, catalogue);
//    MapRenderer render(catalogue, render_settings);
//    if(routing_settings.has_value()){
//        TransportRouter router(catalogue, routing_settings.value());
//        PrintQuery(requests, catalogue, render, std::cout, &router);
//    }else{
//        PrintQuery(requests, catalogue, render, std::cout);
//    }
//
//    return 0;
//}

#include <iostream>
#include <string_view>
#include "include/serialization.h"
#include "include/json_reader.h"
#include "include/tests.h"

using namespace std::literals;
using namespace svg;
using namespace transport;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {

#ifndef NDEBUG
    {
        LOG_DURATION("TestProject");
        tests::TestProject();
    }
    {
        LOG_DURATION("TestExamples");
        tests::TestExamples();
    }
#endif

    auto error = [](const std::filesystem::path& file_path){
        std::cerr << "Что-то пошло не так: Файл для сериализации " << file_path.string() << " - не открылся" << std::endl;
    };

    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    TransportCatalogue catalogue;
    serialize::Serializer serializer;
    if (mode == "make_base"sv) {
        auto [requests, render_settings, routing_settings, serialize_settings] = detail::ParseJson(std::cin, catalogue);
        std::ofstream ofstream(serialize_settings->file_path, std::ios::binary);
        if(!ofstream.is_open()){ error(serialize_settings->file_path); }
            serializer.Serialize(catalogue, render_settings, routing_settings, ofstream);
        ofstream.close();

    } else if (mode == "process_requests"sv) {
        auto [requests, render_settings, routing_settings, serialize_settings] = detail::ParseJson(std::cin, catalogue);
        std::ifstream ifstream(serialize_settings->file_path, std::ios::binary);
        if(!ifstream.is_open()){ error(serialize_settings->file_path); }
            std::unique_ptr<transport::TransportRouter> router;
            serializer.Deserialize(catalogue, render_settings, router, ifstream);
            if(router){
                routing_settings = router->GetSettings();
            }


        MapRenderer render(catalogue, render_settings);
            if(routing_settings.has_value()){
                PrintQuery(requests, catalogue, render, std::cout, router.get());
            }else{
                PrintQuery(requests, catalogue, render, std::cout);
            }
        ifstream.close();
    } else {
        PrintUsage();
        return 1;
    }
}