#include "../include/tests.h"
#include "../include/stat_reader.h"
#include "../json/json.h"
#include "../json/json_builder.h"
using namespace std;

namespace transport {

    namespace tests {

    using namespace json;
    using namespace domain;
    using namespace detail;
    using namespace std::filesystem;

        void TestStrToLower(){

            {
            std::string t = " AB fG hcG _ fthFH"s;
            detail::StrToLower(t);
            ASSERT_EQUAL(t, " ab fg hcg _ fthfh"s);
            }

        }

        void TestStrConcat(){
            std::vector<std::string_view> t = {"Well"sv, "Hello"sv, "World."sv, "How"sv, "you"sv, "been?"sv};
            std::string res = StrConcat(t, " "s);
            std::string res2 = StrConcat(t);
            ASSERT_EQUAL(res, "Well Hello World. How you been?"s)
            ASSERT_EQUAL(res2, "WellHelloWorld.Howyoubeen?"s)
        }

        void TestLeftTrim(){

            {
            std::string_view t = ""sv;
            LeftTrim(t);
            ASSERT(t.data() == ""s);

            }

            {
            std::string_view t = "abc"sv;
            LeftTrim(t);
            ASSERT(t.data() == "abc"s);
            }

            {
            std::string_view t = "    abc bc  "sv;
            LeftTrim(t);
            ASSERT(t.data() == "abc bc  "s);

            }

            {
            std::string_view t = "abc bc  "sv;
            LeftTrim(t);
            ASSERT(t.data() == "abc bc  "s);
            }
        }

        void TestRightTrim(){
            {
            std::string_view t = ""sv;
            RightTrim(t);
            ASSERT(t.data() == ""s);

            }

            {
            std::string_view t = "abc"sv;
            RightTrim(t);
            ASSERT(t.data() == "abc"s);
            }

            {
            std::string_view t = "   abc bc  "sv;
            RightTrim(t);
            ASSERT(std::string(t) == "   abc bc"s);
            }

            {
            std::string_view t = "  abc bc"sv;
            RightTrim(t);
            ASSERT(t == "  abc bc"sv);
            }
        }

        void TestTrim(){
            {
            std::string_view t = ""sv;
            Trim(t);
            ASSERT(t.data() == ""s);

            }

            {
            std::string_view t = "abc"sv;
            Trim(t);
            ASSERT(t.data() == "abc"s);

            }

            {
            std::string_view t = "    abc  "sv;
            Trim(t);
            ASSERT_EQUAL(t, "abc"sv);
            }

            {
            std::string_view t = "   abc bc  "sv;
            Trim(t);
            ASSERT_EQUAL(t , "abc bc"sv);
            }

            {
            std::string_view t = "abc bc  "sv;
            Trim(t);
            ASSERT(t == "abc bc"sv);
            }
        }

        void TestSplitIntoWords(){
            assert((SplitIntoWords(""s) == vector<string_view>{}));
            assert((SplitIntoWords("     "s) == vector<string_view>{}));
            assert((SplitIntoWords("aaaaaaa"s) == vector{"aaaaaaa"sv}));
            assert((SplitIntoWords("a"s) == vector{"a"sv}));
            assert((SplitIntoWords("a b c"s) == vector{"a"sv, "b"sv, "c"sv}));
            assert((SplitIntoWords("a    bbb   cc"s) == vector{"a"sv, "bbb"sv, "cc"sv}));
            assert((SplitIntoWords("  a    bbb   cc"s) == vector{"a"sv, "bbb"sv, "cc"sv}));
            assert((SplitIntoWords("a    bbb   cc   "s) == vector{"a"sv, "bbb"sv, "cc"sv}));
            assert((SplitIntoWords("  a    bbb   cc   "s) == vector{"a"sv, "bbb"sv, "cc"sv}));

            auto doc0 = ""s;
            auto vec1 = SplitIntoWords(doc0);
            ASSERT_EQUAL(vec1.size() , 0);
            auto doc1 = "раз два три"s;
            auto vec2 = SplitIntoWords(doc1);
            ASSERT(vec2.size() == 3 &&
                   (vec2[0] == "раз"s));
            auto doc2 = "     раз      два      три     "s;
            auto vec3 = SplitIntoWords(doc2);
            ASSERT(vec3.size() == 3 &&
                   vec3[0] == "раз"s);
        }

        void TestCheckNumberLines(){
            {
                std::stringstream ss;
                ss << " 100 \n";
                ss << " 0 \n";
                ss << " -100 \n";
                ASSERT_EQUAL(CheckNumberLines(ss), 100);
                ASSERT_EQUAL(CheckNumberLines(ss), 0);
                ASSERT_THROWS(CheckNumberLines(ss), std::out_of_range);
            }
        }

        void TestParseTypeQueryAndName(){
            {
            std::string str = "    Stop    Tolstopaltsevo:"s;
            auto [type, name] = ParseTypeQueryAndName(str);
            ASSERT_EQUAL(type, "Stop"s);
            ASSERT_EQUAL(name, "Tolstopaltsevo"s);
            }

            {
            std::string str = "    Stop    Tolstopaltsevo    :"s;
            auto [type, name] = ParseTypeQueryAndName(str);
            ASSERT_EQUAL(type, "Stop"s);
            ASSERT_EQUAL(name, "Tolstopaltsevo"s);
            }

            {
            std::string str = "   Stop    Biryulyovo     Tovarnaya   :"s;
            auto [type, name] = ParseTypeQueryAndName(str);
            ASSERT_EQUAL(type, "Stop"s);
            ASSERT_EQUAL(name, "Biryulyovo     Tovarnaya"s);
            }

            {
                std::string str = "Bus tUDvCLK83e652  FUYRgRXc:"s;
                auto [type, name] = ParseTypeQueryAndName(str);
                ASSERT_EQUAL(type, "Bus"s);
                ASSERT_EQUAL(name, "tUDvCLK83e652  FUYRgRXc"s);
            }
        }

        void TestParseStopQuery(){
            {
            std::string str = "   55.611087, 37.208290  "s;
            auto [_, coord] = ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "55.611087   ,  37.208290 "s;
            auto [_, coord] = ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "55.611087   ,  37.208290"s;
            auto [_, coord] = ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "55.611087,37.208290"s;
            auto [_, coord] = ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "-91,60"s;
            ASSERT_THROWS(ParseStopQuery(str), std::out_of_range);
            }

            {
            std::string str = "91,60"s;
            ASSERT_THROWS(ParseStopQuery(str), std::out_of_range);
            }

            {
            std::string str = "50, 181"s;
            ASSERT_THROWS(ParseStopQuery(str), std::out_of_range);
            }

            {
            std::string str = "50,-181"s;
            ASSERT_THROWS(ParseStopQuery(str), std::out_of_range);
            }


            {
            std::string str = "90,180"s;
            ASSERT_DOESNT_THROW(ParseStopQuery(str));
            }

            {
            std::string str = "-90,-180"s;
            ASSERT_DOESNT_THROW(ParseStopQuery(str));
            }
        }

        void TestParseRouteQuery(){
            {
            std::string str = "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s;
            auto [type, res] = ParseRouteQuery(str);

            ASSERT(type == RouteType::CIRCLE);
            ASSERT_EQUAL(res.size(), 6);
            ASSERT_EQUAL(res[0].name, "Biryulyovo Zapadnoye"s);
            ASSERT_EQUAL(res[1].name, "Biryusinka"s);
            ASSERT_EQUAL(res[3].name, "Biryulyovo Tovarnaya"s);
            ASSERT_EQUAL(res[5].name, "Biryulyovo Zapadnoye"s);
            }

            {
            std::string str = "  Biryulyovo    Zapadnoye>Biryusinka>Universam>Biryulyovo    Tovarnaya>Biryulyovo     Passazhirskaya>Biryulyovo    Zapadnoye    "s;
            auto [type, res] = ParseRouteQuery(str);

            ASSERT(type == RouteType::CIRCLE);
            ASSERT_EQUAL(res.size(), 6);
            ASSERT_EQUAL(res[0].name, "Biryulyovo    Zapadnoye"s);
            ASSERT_EQUAL(res[1].name, "Biryusinka"s);
            ASSERT_EQUAL(res[3].name, "Biryulyovo    Tovarnaya"s);
            ASSERT_EQUAL(res[5].name, "Biryulyovo    Zapadnoye"s);
            }

            {
            std::string str = "Biryulyovo Zapadnoye - Biryusinka - Universam - Biryulyovo Tovarnaya - Biryulyovo Passazhirskaya"s;
            auto [type, res] = ParseRouteQuery(str);

            ASSERT(type == RouteType::LINEAR);
            ASSERT_EQUAL(res.size(), 5);
            ASSERT_EQUAL(res[0].name, "Biryulyovo Zapadnoye"s);
            ASSERT_EQUAL(res[1].name, "Biryusinka"s);
            ASSERT_EQUAL(res[3].name, "Biryulyovo Tovarnaya"s);
            ASSERT_EQUAL(res[4].name, "Biryulyovo Passazhirskaya"s);
            }

            {
            std::string str = "Biryulyovo Zapadnoye . Biryusinka . Universam . Biryulyovo Tovarnaya . Biryulyovo Passazhirskaya"s;
            ASSERT_THROWS(ParseRouteQuery(str), std::invalid_argument);
            }
        }

        void TestParseOutputQuery(){
            {
            std::stringstream ss;
            ss << " 4 \n";
            ss << " Bus   256 \n"s;
            ss << " Bus   750 \n"s;
            ss << " Bus   751 \n"s;

            ss << " Bus   751 - 26  \n"s;
            auto res = ParseOutputQuery(ss);
            ASSERT_EQUAL(res.size(), 4);
            ASSERT_EQUAL(res[0].second, "256"s);
            ASSERT_EQUAL(res[1].second, "750"s);
            ASSERT_EQUAL(res[2].second, "751"s);
            ASSERT_EQUAL(res[3].second, "751 - 26"s);

            ASSERT(res[0].first == QueryType::Bus);
            ASSERT(res[1].first == QueryType::Bus);
            ASSERT(res[2].first == QueryType::Bus);
            ASSERT(res[3].first == QueryType::Bus);
            }

            {
                std::stringstream ss;
                ss << "1 \n";
                ss << "Dus 256\n";
                ASSERT_THROWS(ParseOutputQuery(ss), std::invalid_argument);
            }
        }

        void TestParseDistances(){
            TransportCatalogue catalogue;
            std::unordered_map<string, string> stop_to_distances;

            auto add_stop = [&](std::string& name, std::string& coord){
                auto [distance_stops , coordinate] = ParseStopQuery(coord);
                if(!distance_stops.empty()){
                     stop_to_distances[name] = std::move(distance_stops);
                }
                catalogue.AddStop({std::move(name), std::move(coordinate)});
            };

            {
                std::string name1 = "Biryulyovo Zapadnoye"s;
                std::string coordinate1 = " 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam"s;
                add_stop(name1, coordinate1);

                std::string name2 = "Rossoshanskaya ulitsa"s;
                std::string coordinate2 = " 55.595579, 37.605757, 2500m to Biryulyovo Zapadnoye "s;
                add_stop(name2, coordinate2);


                std::string name3 = "Biryusinka"s;
                std::string coordinate3 = "  55.581065, 37.64839, 750m to Universam"s;
                add_stop(name3, coordinate3);


                std::string name4 = "Universam"s;
                std::string coordinate4 = " 55.587655, 37.645687, 250m to Biryusinka"s;
                add_stop(name4, coordinate4);

                ASSERT_EQUAL(catalogue.GetAmountStops(), 4);
                ASSERT_EQUAL(stop_to_distances.size() , 4);

                ASSERT_DOESNT_THROW(stop_to_distances.at("Biryulyovo Zapadnoye"s));
                ASSERT((catalogue.FindStop("Biryulyovo Zapadnoye"s)->name == "Biryulyovo Zapadnoye"s) );
                ASSERT((catalogue.FindStop("Biryusinka"s)->name == "Biryusinka"s) );
                ASSERT((catalogue.FindStop("Universam"s)->name == "Universam"s) );
                ASSERT((catalogue.FindStop("Rossoshanskaya ulitsa"s)->name == "Rossoshanskaya ulitsa"s) );

                std::string distanses =  "7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam"s;
                ParseDistances(catalogue, stop_to_distances);

                ASSERT_EQUAL(catalogue.AtStopsDistance("Biryulyovo Zapadnoye"s, "Rossoshanskaya ulitsa"s), 7500);
                ASSERT_EQUAL(catalogue.AtStopsDistance("Rossoshanskaya ulitsa"s, "Biryulyovo Zapadnoye"s), 2500);

                ASSERT_EQUAL(catalogue.AtStopsDistance("Biryulyovo Zapadnoye"s, "Biryusinka"s), 1800);
                ASSERT_EQUAL(catalogue.AtStopsDistance("Biryusinka"s, "Biryulyovo Zapadnoye"s), 1800);

                ASSERT_EQUAL(catalogue.AtStopsDistance("Biryusinka"s, "Universam"s), 750);
                ASSERT_EQUAL(catalogue.AtStopsDistance("Universam"s, "Biryusinka"s), 250);

                ASSERT_EQUAL(catalogue.AtStopsDistance("Biryulyovo Zapadnoye"s, "Universam"s), 2400);
                ASSERT_EQUAL(catalogue.AtStopsDistance("Universam"s, "Biryulyovo Zapadnoye"s), 2400);

            }

        }


        namespace {

        using namespace json;
        using namespace std::literals;

        json::Document LoadJSON(const std::string& s) {
            std::istringstream strm(s);
            return json::Load(strm);
        }

        std::string Print(const Node& node) {
            std::ostringstream out;
            Print(Document{node}, out);
            return out.str();
        }

        void MustFailToLoad(const std::string& s) {
            try {
                LoadJSON(s);
                std::cerr << "ParsingError exception is expected on '"sv << s << "'"sv << std::endl;
                assert(false);
            } catch (const json::ParsingError&) {
                // ok
            } catch (const std::exception& e) {
                std::cerr << "exception thrown: "sv << e.what() << std::endl;
                assert(false);
            } catch (...) {
                std::cerr << "Unexpected error"sv << std::endl;
                assert(false);
            }
        }

        template <typename Fn>
        void MustThrowLogicError(Fn fn) {
            try {
                fn();
                std::cerr << "logic_error is expected"sv << std::endl;
                assert(false);
            } catch (const std::logic_error&) {
                // ok
            } catch (const std::exception& e) {
                std::cerr << "exception thrown: "sv << e.what() << std::endl;
                assert(false);
            } catch (...) {
                std::cerr << "Unexpected error"sv << std::endl;
                assert(false);
            }
        }

        void TestNull() {
                Node null_node;
                ASSERT(null_node.IsNull());
                ASSERT(!null_node.IsInt());
                ASSERT(!null_node.IsDouble());
                ASSERT(!null_node.IsPureDouble());
                ASSERT(!null_node.IsString());
                ASSERT(!null_node.IsArray());
                ASSERT(!null_node.IsDict());

                Node null_node1{nullptr};
                ASSERT(null_node1.IsNull());

                ASSERT(Print(null_node) == "null"s);
                ASSERT(null_node == null_node1);
                ASSERT(!(null_node != null_node1));

                const Node node = LoadJSON("null"s).GetRoot();
                ASSERT(node.IsNull());
                ASSERT(node == null_node);
                // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
                ASSERT(LoadJSON(" \t\r\n\n\r null \t\r\n\n\r "s).GetRoot() == null_node);
            }

            void TestNumbers() {
                const Node int_node{42};
                ASSERT(int_node.IsInt());
                ASSERT(int_node.AsInt() == 42);
                // целые числа являются подмножеством чисел с плавающей запятой
                ASSERT(int_node.IsDouble());
                // Когда узел хранит int, можно получить соответствующее ему double-значение
                ASSERT(int_node.AsDouble() == 42.0);
                ASSERT(!int_node.IsPureDouble());
                ASSERT(int_node == Node{42});
                // int и double - разные типы, поэтому не равны, даже когда хранят
                ASSERT(int_node != Node{42.0});

                const Node dbl_node{123.45};
                ASSERT(dbl_node.IsDouble());
                ASSERT(dbl_node.AsDouble() == 123.45);
                ASSERT(dbl_node.IsPureDouble());  // Значение содержит число с плавающей запятой
                ASSERT(!dbl_node.IsInt());

                ASSERT(Print(int_node) == "42"s);
                ASSERT(Print(dbl_node) == "123.45"s);
                ASSERT(Print(Node{-42}) == "-42"s);
                ASSERT(Print(Node{-3.5}) == "-3.5"s);

                ASSERT(LoadJSON("42"s).GetRoot() == int_node);
                ASSERT(LoadJSON("123.45"s).GetRoot() == dbl_node);
                ASSERT(LoadJSON("0.25"s).GetRoot().AsDouble() == 0.25);
                ASSERT(LoadJSON("3e5"s).GetRoot().AsDouble() == 3e5);
                ASSERT(LoadJSON("1.2e-5"s).GetRoot().AsDouble() == 1.2e-5);
                ASSERT(LoadJSON("1.2e+5"s).GetRoot().AsDouble() == 1.2e5);
                ASSERT(LoadJSON("-123456"s).GetRoot().AsInt() == -123456);
                ASSERT(LoadJSON("0").GetRoot() == Node{0});
                ASSERT(LoadJSON("0.0").GetRoot() == Node{0.0});
                // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
                ASSERT(LoadJSON(" \t\r\n\n\r 0.0 \t\r\n\n\r ").GetRoot() == Node{0.0});
            }

            void TestStrings() {
                Node str_node{"Hello, \"everybody\""s};
                ASSERT(str_node.IsString());
                ASSERT(str_node.AsString() == "Hello, \"everybody\""s);

                ASSERT(!str_node.IsInt());
                ASSERT(!str_node.IsDouble());

                ASSERT(Print(str_node) == "\"Hello, \\\"everybody\\\"\""s);

                ASSERT(LoadJSON(Print(str_node)).GetRoot() == str_node);
                const std::string escape_chars
                    = R"("\r\n\t\"\\")"s;  // При чтении строкового литерала последовательности \r,\n,\t,\\,\"
                // преобразовываться в соответствующие символы.
                // При выводе эти символы должны экранироваться, кроме \t.
                ASSERT(Print(LoadJSON(escape_chars).GetRoot()) == "\"\\r\\n\t\\\"\\\\\""s);
                // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
                ASSERT(LoadJSON("\t\r\n\n\r \"Hello\" \t\r\n\n\r ").GetRoot() == Node{"Hello"s});
            }

            void TestBool() {
                Node true_node{true};
                ASSERT(true_node.IsBool());
                ASSERT(true_node.AsBool());

                Node false_node{false};
                ASSERT(false_node.IsBool());
                ASSERT(!false_node.AsBool());

                ASSERT(Print(true_node) == "true"s);
                ASSERT(Print(false_node) == "false"s);

                ASSERT(LoadJSON("true"s).GetRoot() == true_node);
                ASSERT(LoadJSON("false"s).GetRoot() == false_node);
                ASSERT(LoadJSON(" \t\r\n\n\r true \r\n"s).GetRoot() == true_node);
                ASSERT(LoadJSON(" \t\r\n\n\r false \t\r\n\n\r "s).GetRoot() == false_node);
            }

            void TestArray() {
                Node arr_node{Array{1, 1.23, "Hello"s}};
                ASSERT(arr_node.IsArray());
                const Array& arr = arr_node.AsArray();
                ASSERT(arr.size() == 3);
                ASSERT(arr.at(0).AsInt() == 1);

                ASSERT(LoadJSON("[1,1.23,\"Hello\"]"s).GetRoot() == arr_node);
                ASSERT(LoadJSON(Print(arr_node)).GetRoot() == arr_node);
                ASSERT(LoadJSON(R"(  [ 1  ,  1.23,  "Hello"   ]   )"s).GetRoot() == arr_node);
                // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
                ASSERT(LoadJSON("[ 1 \r \n ,  \r\n\t 1.23, \n \n  \t\t  \"Hello\" \t \n  ] \n  "s).GetRoot()
                       == arr_node);
            }

            void TestDict() {
                Node dict_node{Dict{{"key1"s, "value1"s}, {"key2"s, 42}}};
                ASSERT(dict_node.IsDict());
                const Dict& dict = dict_node.AsDict();
                ASSERT(dict.size() == 2);
                ASSERT(dict.at("key1"s).AsString() == "value1"s);
                ASSERT(dict.at("key2"s).AsInt() == 42);

                ASSERT(LoadJSON("{ \"key1\": \"value1\", \"key2\": 42 }"s).GetRoot() == dict_node);
                auto v = LoadJSON(Print(dict_node)).GetRoot();
                ASSERT(LoadJSON(Print(dict_node)).GetRoot() == dict_node);
                // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
                ASSERT(
                    LoadJSON(
                        "\t\r\n\n\r { \t\r\n\n\r \"key1\" \t\r\n\n\r: \t\r\n\n\r \"value1\" \t\r\n\n\r , \t\r\n\n\r \"key2\" \t\r\n\n\r : \t\r\n\n\r 42 \t\r\n\n\r } \t\r\n\n\r"s)
                        .GetRoot()
                    == dict_node);
            }

            void TestErrorHandling() {
                MustFailToLoad("["s);
                MustFailToLoad("]"s);

                MustFailToLoad("{"s);
                MustFailToLoad("}"s);

                MustFailToLoad("\"hello"s);  // незакрытая кавычка

                MustFailToLoad("tru"s);
                MustFailToLoad("fals"s);
                MustFailToLoad("nul"s);

                Node dbl_node{3.5};
                MustThrowLogicError([&dbl_node] {
                    dbl_node.AsInt();
                });
                MustThrowLogicError([&dbl_node] {
                    dbl_node.AsString();
                });
                MustThrowLogicError([&dbl_node] {
                    dbl_node.AsArray();
                });

                Node array_node{Array{}};
                MustThrowLogicError([&array_node] {
                    array_node.AsDict();
                });
                MustThrowLogicError([&array_node] {
                    array_node.AsDouble();
                });
                MustThrowLogicError([&array_node] {
                    array_node.AsBool();
                });
            }

            void Benchmark() {
                const auto start = std::chrono::steady_clock::now();
                Array arr;
                arr.reserve(1'000);
                for (int i = 0; i < 1'000; ++i) {
                    arr.emplace_back(Dict{
                        {"int"s, 42},
                        {"double"s, 42.1},
                        {"null"s, nullptr},
                        {"string"s, "hello"s},
                        {"array"s, Array{1, 2, 3}},
                        {"bool"s, true},
                        {"map"s, Dict{{"key"s, "value"s}}},
                    });
                }
                std::stringstream strm;
                json::Print(Document{arr}, strm);
                const auto doc = json::Load(strm);
                ASSERT(doc.GetRoot() == arr);
                const auto duration = std::chrono::steady_clock::now() - start;
                std::cerr << "Benchmark: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"sv
                          << std::endl;
            }

            void TestJsonBuilder(){
                {
                    std::stringstream ss_test ;
                    ss_test << "\"just a string\"";

                    std::stringstream ss_out;
                    json::Print(
                        json::Document{
                            json::Builder{}.
                            Value("just a string"s).
                                    Build()
                        }, ss_out
                    );
                    ASSERT_EQUAL(ss_out.str(), ss_test.str())
                }
                {
                    std::stringstream ss_test ;
                    ss_test <<  "[\n"
                                "    1,\n"
                                "    2,\n"
                                "    3,\n"
                                "    4\n"
                                "]";

                    std::stringstream ss_out;
                    json::Print(
                        json::Document{
                            json::Builder{}.
                                    StartArray().
                                    Value(1).
                                    Value(2).
                                    Value(3).
                                    Value(4).
                                    EndArray().
                                    Build()
                        }, ss_out
                    );
                    ASSERT_EQUAL(ss_out.str(), ss_test.str())
                }

                {
                    std::stringstream ss_test ;
                    ss_test <<  "{\n"
                                "    \"key1\": 123,\n"
                                "    \"key2\": \"value2\",\n"
                                "    \"key3\": \"643\"\n"
                                "}";

                    std::stringstream ss_out;
                    json::Print(
                        json::Document{
                            json::Builder{}.
                                    StartDict()
                                    .Key("key1"s).Value(123)
                                    .Key("key2"s).Value("value2"s)
                                    .Key("key3"s).Value("643"s)
                                    .EndDict()
                            .Build()
                        }, ss_out
                    );
                    ASSERT_EQUAL(ss_out.str(), ss_test.str())
                }

                {
                    std::stringstream ss_out;
                    json::Print(
                        json::Document{
                            json::Builder{}
                                    .Value(1)
                            .Build()
                        }, ss_out
                    );
                    ASSERT_EQUAL(ss_out.str(), "1");
                }

                {
                    std::stringstream ss_out;
                    json::Print(
                        json::Document{
                            json::Builder{}
                                    .Value(true)
                            .Build()
                        }, ss_out
                    );
                    ASSERT_EQUAL(ss_out.str(), "true");
                }

                {
                    std::stringstream ss_out;
                    json::Print(
                        json::Document{
                            json::Builder{}
                                    .Value(1e10)
                            .Build()
                        }, ss_out
                    );
                    ASSERT_EQUAL(ss_out.str(), "1e+10");
                }

                {
                    std::stringstream ss_test ;
                    ss_test <<  "{\n"
                                "    \"key1\": 123,\n"
                                "    \"key2\": \"value2\",\n"
                                "    \"key3\": [\n"
                                "        456,\n"
                                "        {\n"
                                "\n"
                                "        },\n"
                                "        {\n"
                                "            \"\": null\n"
                                "        },\n"
                                "        \"\"\n"
                                "    ]\n"
                                "}";

                    std::stringstream ss_out;
                    json::Print(
                        json::Document{
                            json::Builder{}
                            .StartDict()
                                .Key( "key1"s ).Value( 123 )
                                .Key( "key2"s ).Value( "value2"s )
                                .Key( "key3"s ).StartArray()
                                    .Value( 456 )
                                    .StartDict().EndDict()
                                    .StartDict()
                                        .Key( ""s )
                                        .Value( nullptr )
                                    .EndDict()
                                    .Value( ""s )
                                .EndArray()
                            .EndDict()
                            .Build()
                        },
                        ss_out
                    );
                    ASSERT_EQUAL(ss_out.str(), ss_test.str());
                }
            }

            void TestJsonBuilderThrows(){

                using std::cout;
                {
                    ASSERT_THROWS(
                        json::Document{
                            json::Builder{}
                                    .Value(1)
                                    .Value(2)
                                    .Value(3)
                                    .Value(4)
                                    .Value(5)
                            .Build()
                        }, std::logic_error)
                }

                {
                    ASSERT_THROWS(
                        json::Document{
                            json::Builder{}.Build()
                        }, std::logic_error)
                }

                {
                    ASSERT_THROWS(
                        json::Document{
                            json::Builder{}.Build()
                        }, std::logic_error)
                }

                {
                    ASSERT_THROWS(
                        json::Document{
                            json::Builder{}.
                                          StartArray()
                                          .Value(123)
                                          .EndArray()
                                          .Value(245)
                                          .Build()
                        }, std::logic_error)
                }

                {
                    ASSERT_DOESNT_THROW(
                        json::Document{
                            json::Builder{}.StartArray()
                                          .EndArray()
                                          .Build()
                        })
                }

                {
                    ASSERT_THROWS(
                        json::Document{
                            json::Builder{}.Value(123)
                                          .StartArray()
                                          .EndArray()
                                          .Build()
                        }, std::logic_error)
                }

                {
                    ASSERT_DOESNT_THROW(
                        json::Document{
                            json::Builder{}.StartDict()
                                          .EndDict()
                                          .Build()
                        })
                }
            }

        }

        void TestJSON(){
            TestNull();
            TestNumbers();
            TestStrings();
            TestBool();
            TestArray();
            TestDict();
            TestErrorHandling();
            Benchmark();
            TestJsonBuilder();
            TestJsonBuilderThrows();
        }

        void TestHandleStop(){
            std::stringstream ss;
            TransportCatalogue catalogue;
            ss << "{ "
               << " \"type\": \"Bus\", "
               << " \"name\": \"114\", "
               << " \"stops\": [\"Морской вокзал\", \"Ривьерский мост\"], "
               << " \"is_roundtrip\": false "
               << "}";
            auto document = LoadJSON(ss.str()).GetRoot();
            //HandleRound(document.AsMap());
        }

        void TestSetDistance(){

            {
            TransportCatalogue catalogue;
            std::string stop1 = "Ривьерский мост";
            std::string stop2 = "Морской вокзал";
            std::string stop3 = "Морской";
            catalogue.AddStop({stop1, {43.587795, 39.716901}});
            catalogue.AddStop({stop2, {43.581969, 39.716901}});
            catalogue.AddStop({stop3, {43.587795, 39.716901}});
            ASSERT(catalogue.GetAmountStops() == 3);

            catalogue.SetDistance(catalogue.FindStop(stop1), catalogue.FindStop(stop2), 350);
            ASSERT_EQUAL(catalogue.GetAmountStopsDistances(), 2);
            ASSERT(catalogue.AtStopsDistance(stop1, stop2) == 350);
            ASSERT(catalogue.AtStopsDistance(stop2, stop1) == 350);

            catalogue.SetDistance(catalogue.FindStop(stop2), catalogue.FindStop(stop1), 650);
            ASSERT_EQUAL(catalogue.GetAmountStopsDistances(), 2);
            ASSERT(catalogue.AtStopsDistance(stop1, stop2) == 350);
            ASSERT(catalogue.AtStopsDistance(stop2, stop1) == 650);

            catalogue.SetDistance(catalogue.FindStop(stop2), catalogue.FindStop(stop3), 550);
            ASSERT_EQUAL(catalogue.GetAmountStopsDistances(), 4);
            ASSERT(catalogue.AtStopsDistance(stop2, stop3) == 550);
            ASSERT(catalogue.AtStopsDistance(stop3, stop2) == 550);

            catalogue.SetDistance(catalogue.FindStop(stop3), catalogue.FindStop(stop1), 750);
            ASSERT_EQUAL(catalogue.GetAmountStopsDistances(), 6);
            ASSERT(catalogue.AtStopsDistance(stop3, stop1) == 750);
            ASSERT(catalogue.AtStopsDistance(stop1, stop3) == 750);
            }

            {
            TransportCatalogue catalogue;
            std::string stop1 = "Ривьерский мост";
            std::string stop2 = "Морской вокзал";
            std::string stop3 = "Морской";
            catalogue.AddStop({stop1, {43.587795, 39.716901}});
            catalogue.AddStop({stop2, {43.581969, 39.716901}});
            catalogue.AddStop({stop3, {43.587795, 39.716901}});
            ASSERT(catalogue.GetAmountStops() == 3);

            catalogue.SetDistance(catalogue.FindStop(stop1), catalogue.FindStop(stop2), 350);
            catalogue.SetDistance(catalogue.FindStop(stop1), catalogue.FindStop(stop3), 990);
            ASSERT_EQUAL(catalogue.GetAmountStopsDistances(), 4);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop1, stop2), 350);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop2, stop1), 350);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop1, stop3), 990);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop3, stop1), 990);

            catalogue.SetDistance(catalogue.FindStop(stop2), catalogue.FindStop(stop3), 550);
            catalogue.SetDistance(catalogue.FindStop(stop2), catalogue.FindStop(stop1), 650);
            ASSERT_EQUAL(catalogue.GetAmountStopsDistances(), 6);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop2, stop3), 550);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop3, stop2), 550);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop1, stop2), 350);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop2, stop1), 650);

            catalogue.SetDistance(catalogue.FindStop(stop3), catalogue.FindStop(stop1), 750);
            ASSERT_EQUAL(catalogue.GetAmountStopsDistances(), 6);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop3, stop1), 750);
            ASSERT_EQUAL(catalogue.AtStopsDistance(stop1, stop3), 990);
            }

        }

        void TestTxtFiles(path test_path, path standard_path, std::ostream& out_logs){
            LOG_DURATION(standard_path.filename().string());

            using namespace std::literals;

            TransportCatalogue catalogue;

            path result_path = test_path.parent_path().
                    append(test_path.stem().string()).string().
                    append("_my_result.log"s);
            std::vector<std::pair<QueryType, std::string>> queries;
            std::ifstream in(test_path);
            {
                LOG_DURATION(standard_path.filename().string() + " read data ");
                if(in.is_open()){
                    ParseTxt(in, catalogue);
                    queries = ParseOutputQuery(in);
                }else{
                    std::cout << " Файл тестовых данных " << test_path.string() << " не открыт " << std::endl;
                }
            }
            in.close();
            std::ofstream out(result_path.string());
            if(out.is_open()){
                for(auto& [type ,query] : queries){
                    PrintQuery(type, query, catalogue, out);
                }
            }else{
                std::cerr << "Ошибка: файл для вывода результатов " << result_path.string() << " не открыт " << std::endl;
            }
            out.close();

            EqualLogFiles(result_path, standard_path, out_logs);
        }

        void TestJsonFiles(path test_path, path standard_path, std::ostream& out_logs){
            LOG_DURATION(standard_path.filename().string());
            TransportCatalogue catalogue;
            path result_path = test_path.parent_path().
                    append(test_path.stem().string()).string().
                    append("_my_result.log"s);

            auto [out, render_settings, routing_settings] = ReadFile(test_path, catalogue);
            DictRenderer render(catalogue, render_settings);
            std::ofstream out_stream(result_path.string());
            if(out_stream.is_open()){
                PrintQuery(out , catalogue, render, out_stream);
            }else{
                std::cerr << "Ошибка: файл для вывода результатов " << result_path.string() << " не открыт " << std::endl;
            }
            out_stream.close();

            EqualLogFiles(result_path, standard_path, out_logs);

        }

        void TestSvgFiles(path test_path, path standard_path, std::ostream& out_logs){
            LOG_DURATION(standard_path.filename().string());
            TransportCatalogue catalogue;
            path result_path = test_path.parent_path().
                    append(test_path.stem().string()).string().
                    append("_my_result.log"s);


            auto [out, render_settings, routing_settings] = ReadFile(test_path, catalogue);
            DictRenderer render(catalogue, render_settings);
            std::ofstream out_stream(result_path.string());
            if(out_stream.is_open()){
                render.Print(out_stream);
            }else{
                std::cerr << "Ошибка: файл для вывода результатов " << result_path.string() << " не открыт " << std::endl;
            }
            out_stream.close();

            EqualLogFiles(result_path, standard_path, out_logs);

        }

        std::vector<double> LoadRouteTime(std::istream& in){
            std::vector<double> route_times;
            json::Document base_stat_requests = json::Load(in);
            json::Node root = base_stat_requests.GetRoot();
            if(root.IsArray()){
                json::Array requests = root.AsArray();
                for (auto item : requests) {
                    auto map = item.AsDict();
                    auto it_time = map.find("total_time");
                    if(it_time != map.end()){
                        route_times.push_back(it_time->second.AsDouble());
                    }
                }
            }else{
                JSON_STRUCTURAL_ERROR;
            }
            return route_times;
        }

        std::vector<double> LoadRouteTime(std::filesystem::path test_path){
            std::ifstream in(test_path);
            if(!in.is_open()){
                throw std::invalid_argument(" Ошибка: Такого файла не существует. Проверьте путь к файлу: "s +  test_path.string());
            }

            std::vector<double> route_times = LoadRouteTime(in);
            in.close();
            return route_times;
        }

        void CheckFindingOptimalRoute(path test_in, path standard_out, path out_log){
            double eps = 1e-16;
            TransportCatalogue catalogue;
            auto [requests, render_settings, routing_settings] = ReadFile(test_in, catalogue);
            DictRenderer render(catalogue, render_settings);
            std::stringstream ss;
            if(routing_settings.has_value()){
                TransportRouter router(catalogue, routing_settings.value());
                PrintQuery(requests, catalogue, render, ss, &router);
            }else{
                std::cerr << "параметр routing_settings не был обнаружен в файле " + test_in.string() << std::endl;
            }

            std::ofstream out(out_log);
            out << ss.str();
            out.close();

            auto route_times = LoadRouteTime(ss);
            auto test_route_time = LoadRouteTime(standard_out);
            bool check = true;
            for (size_t i = 0; i <  route_times.size(); ++i) {
                if(std::abs(route_times[i] - test_route_time[i]) >= eps){
                    std::cerr << " В " << i << " запросе на оптимальный маршрут есть различия в файлах " <<
                    out_log.string() << " и " << standard_out.string() << "."
                    << " Мой результат = " << route_times[i]
                    << ", В тестах = " << test_route_time[i] << std::endl;
                    check = false;
                }
            }
            ASSERT(check);
        }

        void TestFindingOptimalRoute(){
            CheckFindingOptimalRoute("../tests/tsD_case/tsD_case_input.json"s,
                                     "../tests/tsD_case/tsD_case_output.json"s,
                                     "../tests/tsD_case/tsD_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsE_case/tsE_case_input.json"s,
                                     "../tests/tsE_case/tsE_case_output.json"s,
                                     "../tests/tsE_case/tsE_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsF_case/tsF_case_input.json"s,
                                     "../tests/tsF_case/tsF_case_output.json"s,
                                     "../tests/tsF_case/tsF_case_my_result.json"s);

//            CheckFindingOptimalRoute("../tests/tsG_case/tsG_case_input.json"s,
//                                     "../tests/tsG_case/tsG_case_output.json"s,
//                                     "../tests/tsG_case/tsG_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsH_case/tsH_case_input.json"s,
                                     "../tests/tsH_case/tsH_case_output.json"s,
                                     "../tests/tsH_case/tsH_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsI_case/tsI_case_input.json"s,
                                     "../tests/tsI_case/tsI_case_output.json"s,
                                     "../tests/tsI_case/tsI_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsJ_case/tsJ_case_input.json"s,
                                     "../tests/tsJ_case/tsJ_case_output.json"s,
                                     "../tests/tsJ_case/tsJ_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsK_case/tsK_case_input.json"s,
                                     "../tests/tsK_case/tsK_case_output.json"s,
                                     "../tests/tsK_case/tsK_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsL_case/tsL_case_input.json"s,
                                     "../tests/tsL_case/tsL_case_output.json"s,
                                     "../tests/tsL_case/tsL_case_my_result.json"s);

            CheckFindingOptimalRoute("../tests/tsM_case/tsM_case_input.json"s,
                                     "../tests/tsM_case/tsM_case_output.json"s,
                                     "../tests/tsM_case/tsM_case_my_result.json"s);
        }

        void TestProject(){
            TestRunner test;
            RUN_TEST(test, TestJSON);
            RUN_TEST(test, TestSetDistance);
            RUN_TEST(test, TestStrToLower);
            RUN_TEST(test, TestStrConcat);
            RUN_TEST(test, TestLeftTrim);
            RUN_TEST(test, TestRightTrim);
            RUN_TEST(test, TestTrim);
            RUN_TEST(test, TestSplitIntoWords);
            RUN_TEST(test, TestCheckNumberLines);
            RUN_TEST(test, TestParseTypeQueryAndName);
            RUN_TEST(test, TestParseStopQuery);
            RUN_TEST(test, TestParseRouteQuery);
            RUN_TEST(test, TestParseOutputQuery);
            RUN_TEST(test, TestParseDistances);
            RUN_TEST(test, TestFindingOptimalRoute);
        }

        void TestExamples(){
            std::string out_path = "../test_difference.log";
            std::filesystem::remove(out_path);
            std::ofstream out_logs(out_path, std::ios::app);
            {
                if(out_logs.is_open()){

                    TestRunner test;
                    auto tsC_case1 = [&](){ tests::TestTxtFiles("../tests/tsC_case/tsC_case1_input.txt"s, "../tests/tsC_case/tsC_case1_output1.txt"s, out_logs); };
                    RUN_TEST(test, tsC_case1);

                }else{
                    std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
                }
            }

            {
                if(out_logs.is_open()){
                    TestRunner test;
                    auto example1 = [&](){ tests::TestJsonFiles("../examples/example1/example1.json"s, "../examples/example1/output1.json"s, out_logs); };
                    RUN_TEST(test, example1);

                }else{
                    std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
                }
            }

            {
                if(out_logs.is_open()){
                    TestRunner test;
                    auto example2 = [&](){ tests::TestJsonFiles("../examples/example2/example2.json"s, "../examples/example2/output2.json"s, out_logs); };
                    RUN_TEST(test, example2);

                }else{
                    std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
                }
            }

            {
                if(out_logs.is_open()){
                    TestRunner test;
                    auto example3 = [&](){ tests::TestJsonFiles("../examples/example3/example3.json"s, "../examples/example3/output3.json"s, out_logs); };
                    RUN_TEST(test, example3);

                }else{
                    std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
                }
            }
            out_logs.close();
        }

    }
}
