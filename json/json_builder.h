#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H
#include <string>
#include <optional>
#include "json.h"

namespace json{

    class Builder{
            class DictItemContext;
            class ArrayItemContext;
            class KeyItemContext;

        public:
            KeyItemContext Key( const std::string& key );
            Builder& Value(json::Node&& value );

            DictItemContext StartDict();
            Builder& EndDict();

            ArrayItemContext StartArray();
            Builder& EndArray();

            json::Node Build();

        private:

        class DefaultItemContext {
              public:
                DefaultItemContext(Builder &builder) : builder_(builder) {}

              protected:
                Builder &Get() {
                    return builder_;
                }

              private:
                Builder &builder_;
            };

            class DictItemContext : public DefaultItemContext {
              public:
                DictItemContext(Builder &builder) : DefaultItemContext(builder) {}

                KeyItemContext Key(std::string &&value) {
                    return Get().Key(std::move(value));
                }
                Builder &EndDict() {
                    return Get().EndDict();
                }
            };

            class KeyItemContext : public DefaultItemContext {
              public:
                KeyItemContext(Builder &builder) : DefaultItemContext(builder) {}

                DictItemContext Value(Node &&value) {
                    Get().Value(std::move(value));
                    return DictItemContext{Get()};
                }
                DictItemContext StartDict() {
                    return Get().StartDict();
                }
                ArrayItemContext StartArray() {
                    return Get().StartArray();
                }
            };

            class ArrayItemContext : public DefaultItemContext {
              public:
                ArrayItemContext(Builder &builder) : DefaultItemContext(builder) {}

                ArrayItemContext Value(Node &&value) {
                    Get().Value(std::move(value));
                    return ArrayItemContext{Get()};
                }
                DictItemContext StartDict() {
                    return Get().StartDict();
                }
                ArrayItemContext StartArray() {
                    return Get().StartArray();
                }
                Builder &EndArray() {
                    return Get().EndArray();
                }
            };

        private:
            template<typename Container>
            void PushStart();
            bool ObjectIsReady();
            void ThrowReadyException();

        private:
            std::optional<std::string> last_key_;
            bool key_is_used = false;

            std::vector<Node*> nodes_stack_;
            Node root_ = nullptr;
    };

    template<typename Container>
    void Builder::PushStart(){
        if(!nodes_stack_.empty() && nodes_stack_.back()->IsDict() && !key_is_used){
            throw std::logic_error("Вызов Builder::Value в неположенном месте");
        }

        if(root_ == nullptr){
            root_ = Container{};
            nodes_stack_.emplace_back(&root_);
        }else{
            if(nodes_stack_.back()->IsArray()){
                const_cast<Array &>(nodes_stack_.back()->AsArray()).emplace_back(Container());
                nodes_stack_.push_back(&const_cast<Array&>(nodes_stack_.back()->AsArray()).back());
            }else if(nodes_stack_.back()->IsDict()){
                if(!last_key_.has_value()){
                    throw std::logic_error("Вызов Builder::Value в неположенном месте");
                }
                const_cast<Dict&>(nodes_stack_.back()->AsDict())[last_key_.value()] = Container();
                nodes_stack_.push_back(&const_cast<Dict&>(nodes_stack_.back()->AsDict())[last_key_.value()]);
            }else{
                throw std::logic_error("Вызов Builder::PushStart в неположенном месте");
            }
        }
    }
}
#endif // JSON_BUILDER_H
