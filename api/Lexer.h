#ifndef LEXER_H
#define LEXER_H

#include <algorithm>
#include <cctype>
#include <string>

struct Lexer {
    std::string content;
    size_t pos = 0;

    Lexer(std::string content) : content(content) {}

    void trim_left() {
        while (pos < content.size() &&
               (isspace(content[pos]) || !isprint(content[pos])))
            pos++;
    }

    std::string chop(size_t n) {
        size_t start = pos;
        pos += n;
        std::string result(content.substr(start, n));
        // To upper
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    std::string chopWhile(bool (*pred)(char)) {
        size_t n = 0;
        while (pos + n < content.size() && pred(content[pos + n])) n++;
        return chop(n);
    }

    std::string next_token() {
        trim_left();
        if (pos >= content.size()) return "";
        if (isdigit(content[pos]))
            return chopWhile([](char c) -> bool { return isdigit(c); });
        if (isalpha(content[pos]))
            return chopWhile([](char c) -> bool { return isalnum(c); });
        return chop(1);
    }

    // create iterator
    struct iterator {
        Lexer *lexer;
        std::string token;

        iterator(Lexer *lexer) : lexer(lexer) {
            if (lexer) token = lexer->next_token();
        }

        iterator &operator++() {
            token = lexer->next_token();
            return *this;
        }

        bool operator!=(const iterator &other) const {
            return token != other.token;
        }

        std::string operator*() const { return token; }
    };

    iterator begin() { return iterator(this); }

    iterator end() { return iterator(nullptr); }
};

#endif  // LEXER_H