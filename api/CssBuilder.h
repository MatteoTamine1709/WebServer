#ifndef CSS_BUILDER_H
#define CSS_BUILDER_H

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

typedef std::pair<std::string, std::string> Property;
typedef struct Selector {
    Selector() = default;
    Selector(const std::string& selector) : m_selector(selector) {}
    Selector(const std::string& selector,
             const std::vector<Property>& properties)
        : m_selector(selector), m_properties(properties) {}
    ~Selector() = default;

    Selector& addProperty(const std::string& property,
                          const std::string& value);
    Selector& addProperties(const std::vector<Property>& properties);
    std::string toString() const;

   private:
    std::string m_selector;
    std::vector<Property> m_properties;
} Selector;

class CssBuilder {
   public:
    CssBuilder() = default;

    Selector& createSelector(const std::string& selector);
    Selector& createSelector(const std::string& selector,
                             const std::vector<Property>& properties);

    std::string toString() const;

   private:
    std::unordered_map<std::string, Selector> m_selectors;
};

#ifdef CSS_BUILDER_IMPLEMENTATION

Selector& Selector::addProperty(const std::string& property,
                                const std::string& value) {
    m_properties.push_back({property, value});
    return *this;
}

Selector& Selector::addProperties(const std::vector<Property>& properties) {
    for (const auto& property : properties) m_properties.push_back(property);
    return *this;
}

std::string Selector::toString() const {
    std::string selector = m_selector + " {\n";
    for (const auto& property : m_properties)
        selector += "    " + property.first + ": " + property.second + ";\n";
    selector += "}\n";
    return selector;
}

Selector& CssBuilder::createSelector(const std::string& selector) {
    m_selectors[selector] = Selector(selector);
    return m_selectors[selector];
}

Selector& CssBuilder::createSelector(const std::string& selector,
                                     const std::vector<Property>& properties) {
    m_selectors[selector] = Selector(selector, properties);
    return m_selectors[selector];
}

std::string CssBuilder::toString() const {
    std::string css = "";
    for (const auto& selector : m_selectors) css += selector.second.toString();
    return css;
}

#endif

#endif  // CSS_BUILDER_H