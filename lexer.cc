#include <iostream>
#include <fstream>
#include "lexer.hh"

lexer::lexer(std::string f) {
    file.open(f, std::ifstream::in);
    std::cout << "Opened " << f << std::endl;
    line = 0;
}

void lexer::error(std::string s) {
    std::cerr << "Error near line " << line << ": " << s << std::endl;
    throw s;
}

void lexer::emplace_buf(std::string &buf) {
    if (s == word) {
        tokens.emplace_back(token::word{buf});
    } else if (s == whole) {
        try {
            tokens.emplace_back(token::whole{std::stol(buf), line});
        } catch (std::invalid_argument) {
            error("Could not convert `" + buf + "` to long.");
        } catch (std::out_of_range) {
            error("Integer too large: `" + buf + "`");
        }
    }
}

std::list<token::token> lexer::lex() {
    char c;
    std::string buf;

    //std::cout << file.good();

    while ((c = file.get())) {
        if (c == '\n') {
            line++;
            tokens.emplace_back(token::eol{});
        }
        else if (c == -1) {
            tokens.emplace_back(token::eof{});
            return tokens;
        }

        if ((c >= 'a' && c <= 'z')
         || (c >= 'A' && c <= 'Z')
         || c == '_') {
            buf.push_back(c);
            s = word;
        } else if ((c >= '0' && c <= '9')) {
            buf.push_back(c);
            s = whole;
        } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            emplace_buf(buf);

            s = none;
            buf = "";
        // TODO: Fix this and ; to not repeat code
        } else if (c == ':') {
            emplace_buf(buf);

            tokens.emplace_back(token::start_fn{});
            s = none;
            buf = "";
        } else if (c == ';') {
            emplace_buf(buf);

            tokens.emplace_back(token::end_fn{});
            s = none;
            buf = "";
        } else {
            error("Unexpected char `" + std::to_string(c) + "`");
        }
    }

    return tokens;
}
