#pragma once
#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace TAOW::http_client {

std::string encode_url(std::string_view input);

struct FormRequestItem {
    FormRequestItem(std::string val) : _value(std::move(val)) {}
    FormRequestItem(std::initializer_list<FormRequestItem> val) : _value(std::move(val)) {}
    FormRequestItem(std::initializer_list<std::pair<std::string, FormRequestItem>> val)
        : _value([obj = std::move(val)]() {
              std::unordered_map<std::string, FormRequestItem> result{};
              for (const auto& [k, v] : obj) {
                  result.emplace(k, v);
              }
              return result;
          }()) {}

    bool is_array() const { return std::holds_alternative<std::vector<FormRequestItem>>(this->_value); }
    bool is_object() const {
        return std::holds_alternative<std::unordered_map<std::string, FormRequestItem>>(this->_value);
    }
    bool is_scalar() const { return std::holds_alternative<std::string>(this->_value); }
    std::string& get();
    const std::string& get() const;
    FormRequestItem& get(std::size_t idx);
    const FormRequestItem& get(std::size_t idx) const;
    FormRequestItem& get(std::string_view key);
    const FormRequestItem& get(std::string_view key) const;

    std::string to_form_string(std::string_view parent_key) const;

  private:
    std::variant<std::string, std::vector<FormRequestItem>, std::unordered_map<std::string, FormRequestItem>> _value;
    std::string _handle_array_string(std::string_view parent_key) const;
    std::string _handle_object_string(std::string_view parent_key) const;
    std::string _handle_scalar_string(std::string_view parent_key) const;
};

struct FormRequest {
    FormRequest(std::initializer_list<std::pair<std::string, FormRequestItem>> val)
        : _items([obj = std::move(val)]() {
              std::unordered_map<std::string, FormRequestItem> result{};
              for (const auto& [k, v] : obj) {
                  result.emplace(k, v);
              }
              return result;
          }()) {}

    FormRequestItem& get(std::string_view key);
    const FormRequestItem& get(std::string_view key) const;

    std::string to_string() const;

  private:
    std::unordered_map<std::string, FormRequestItem> _items;
};

} // namespace TAOW::http_client