#include "../CssBuilder.h"

Selector &Selector::addProperty(const std::string &property,
                                const std::string &value) {
    m_properties.push_back({property, value});
    return *this;
}

Selector &Selector::addProperties(const std::vector<Property> &properties) {
    for (const auto &property : properties) m_properties.push_back(property);
    return *this;
}

std::string Selector::toString() const {
    std::string selector = m_selector + " {\n";
    for (const auto &property : m_properties)
        selector += "    " + property.first + ": " + property.second + ";\n";
    selector += "}\n";
    return selector;
}

Selector &CssBuilder::createSelector(const std::string &selector) {
    m_selectors[selector] = Selector(selector);
    return m_selectors[selector];
}

Selector &CssBuilder::createSelector(const std::string &selector,
                                     const std::vector<Property> &properties) {
    m_selectors[selector] = Selector(selector, properties);
    return m_selectors[selector];
}

std::string CssBuilder::toString() const {
    std::string css = "";
    for (const auto &selector : m_selectors) css += selector.second.toString();
    return css;
}