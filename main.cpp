#include <iostream>
#include "include/tests.h"
#include "include/stat_reader.h"

using namespace std;
using namespace std::filesystem;
using namespace svg;
using namespace transport;

int main(){

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
    TransportCatalogue catalogue;
    auto [requests, render_settings, routing_settings] = ReadFile("../tests/tsI_case/tsI_case_input.json"s, catalogue);
    // TODO:: Должен возвращать TransportCatalogue ? Попробуй поменять user interface после сдачи на тесты
    // auto [requests, render_settings, routing_settings] = detail::ParseJson(std::cin, catalogue);
    DictRenderer render(catalogue, render_settings);
    if(routing_settings.has_value()){
        TransportRouter router(catalogue, routing_settings.value());
        PrintQuery(requests, catalogue, render, std::cout, &router);
    }else{
        PrintQuery(requests, catalogue, render, std::cout);
    }

    return 0;
}