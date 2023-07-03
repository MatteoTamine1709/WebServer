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

#endif  // CSS_BUILDER_H