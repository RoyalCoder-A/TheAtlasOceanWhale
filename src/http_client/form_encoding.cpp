#include "taow/form_encoding.hpp"
#include <cctype>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace TAOW::http_client {

std::string encode_url(std::string_view input) {
    constexpr char HEX[] = "0123456789ABCDEF";
    std::string output{};
    output.reserve(input.size() * 3);
    for (const unsigned char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '*') {
            output.push_back(static_cast<char>(c));
        } else if (c == ' ') {
            output.push_back('+');
        } else {
            output.push_back('%');
            output.push_back(HEX[(c >> 4) & 0x0f]);
            output.push_back(HEX[c & 0x0f]);
        }
    }
    return output;
}

FormRequestItem& FormRequest::get(std::string_view key) { return this->_items.at(std::string{key}); }
const FormRequestItem& FormRequest::get(std::string_view key) const { return this->_items.at(std::string{key}); }
std::string FormRequest::to_string() const {
    std::string result{};
    for (const auto& [key, val] : this->_items) {
        const auto item = val.to_form_string(key);
        result.append(item);
        result.push_back('&');
    }
    result.pop_back();
    return result;
}

std::string FormRequestItem::to_form_string(std::string_view parent_key) const {
    if (this->is_array()) {
        return this->_handle_array_string(parent_key);
    }
    if (this->is_object()) {
        return this->_handle_object_string(parent_key);
    }
    return this->_handle_scalar_string(parent_key);
}

std::string& FormRequestItem::get() {
    if (!this->is_scalar())
        throw std::runtime_error("Not an scalar!");
    return std::get<std::string>(this->_value);
}

const std::string& FormRequestItem::get() const {
    if (!this->is_scalar())
        throw std::runtime_error("Not an scalar!");
    return std::get<std::string>(this->_value);
}

FormRequestItem& FormRequestItem::get(std::size_t idx) {
    if (!this->is_array())
        throw std::runtime_error("Not an array!");
    return std::get<std::vector<FormRequestItem>>(this->_value)[idx];
}

const FormRequestItem& FormRequestItem::get(std::size_t idx) const {
    if (!this->is_array())
        throw std::runtime_error("Not an array!");
    return std::get<std::vector<FormRequestItem>>(this->_value)[idx];
}

FormRequestItem& FormRequestItem::get(std::string_view key) {
    if (!this->is_object())
        throw std::runtime_error("Not an object!");
    return std::get<std::unordered_map<std::string, FormRequestItem>>(this->_value).at(std::string{key});
}

const FormRequestItem& FormRequestItem::get(std::string_view key) const {
    if (!this->is_object())
        throw std::runtime_error("Not an object!");
    return std::get<std::unordered_map<std::string, FormRequestItem>>(this->_value).at(std::string{key});
}

std::string FormRequestItem::_handle_scalar_string(std::string_view parent_key) const {
    if (!this->is_scalar())
        throw std::runtime_error("Not an scalar!");
    return std::string{encode_url(parent_key)} + "=" + encode_url(std::get<std::string>(this->_value));
}

std::string FormRequestItem::_handle_object_string(std::string_view parent_key) const {
    if (!this->is_object())
        throw std::runtime_error("Not an object!");
    const auto& objects = std::get<std::unordered_map<std::string, FormRequestItem>>(this->_value);
    std::string result{};
    for (const auto& [key, val] : objects) {
        const auto item_key = std::string{parent_key} + "[" + key + "]";
        const auto item = val.to_form_string(item_key);
        result.insert(result.end(), item.begin(), item.end());
        result.push_back('&');
    }
    result.pop_back();
    return result;
}

std::string FormRequestItem::_handle_array_string(std::string_view parent_key) const {
    if (!this->is_array())
        throw std::runtime_error("Not an array!");
    const auto& objects = std::get<std::vector<FormRequestItem>>(this->_value);
    std::string result{};
    for (int i = 0; i < objects.size(); ++i) {
        const auto item_key = std::string{parent_key} + "[" + std::to_string(i) + "]";
        const auto item = objects[i].to_form_string(item_key);
        result.insert(result.end(), item.begin(), item.end());
        result.push_back('&');
    }
    result.pop_back();
    return result;
}

} // namespace TAOW::http_client