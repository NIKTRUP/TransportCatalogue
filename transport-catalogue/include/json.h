#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <string_view>
#include <unordered_set>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        const Value& GetValue() const;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

    bool operator==(const Document& lhs, const Document& rhs);

    bool operator!=(const Document& lhs, const Document& rhs);

    bool operator==(const Node& lhs, const Node& rhs);

    bool operator!=(const Node& lhs, const Node& rhs);

}  // namespace json_impl

#endif // INPUT_READER_H
