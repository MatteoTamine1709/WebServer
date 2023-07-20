#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "libxml/HTMLparser.h"

class HtmlParser {
   public:
    HtmlParser() = default;
    HtmlParser(const std::string& filename) {
        m_doc = htmlReadFile(
            filename.c_str(), NULL,
            HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
        if (!m_doc)
            std::cerr << "Failed to parse file: " << filename << std::endl;
    }
    HtmlParser(const char* buffer, size_t size) {
        m_doc = htmlReadMemory(
            buffer, size, NULL, NULL,
            HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    }
    ~HtmlParser() {
        if (m_doc) xmlFreeDoc(m_doc);
    };

    std::string extractContent() {
        if (!m_doc) return "";

        xmlNode* root = xmlDocGetRootElement(m_doc);
        if (!root) return "";

        std::string extractedContent;
        std::function<void(xmlNode*)> recursive = [&](xmlNode* node) {
            for (xmlNode* child = node->children; child; child = child->next) {
                // Dont include script and style tags
                if (child->type == XML_ELEMENT_NODE &&
                    (std::string((const char*)child->name) ==
                         std::string("script") ||
                     std::string((const char*)child->name) ==
                         std::string("style")))
                    continue;
                if (child->type == XML_TEXT_NODE ||
                    child->type == XML_CDATA_SECTION_NODE) {
                    extractedContent +=
                        std::string((const char*)child->content) +
                        std::string("\n");
                } else if (child->type == XML_ELEMENT_NODE) {
                    recursive(child);
                }
            }
        };

        recursive(root);
        return extractedContent;
    }

    xmlNode* getRoot() {
        if (!m_doc) return nullptr;
        return xmlDocGetRootElement(m_doc);
    }

   private:
    htmlDocPtr m_doc;
};

#endif  // XML_PARSER_H