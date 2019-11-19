#include <iostream>
#include <fstream>
#include "lexer.hh"

lexer::lexer(std::string f) {
    file.open(f, std::ifstream::in);
    std::cerr << "Opened " << f << std::endl;
    line = 0;
}

void lexer::error(std::string s) {
    std::cerr << "Error on line " << line << ": " << s << std::endl;
    throw s;
}

void lexer::emplace_buf(std::string &buf) {
    if (s == word) {
        tokens.emplace_back(token::word{buf,line});
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
    bool line_comment = false;

    //std::cout << file.good();

    while ((c = file.get())) {
        if (c == '\n') {
            line++;

            if (line_comment) {
                line_comment = false;
            }
        } else if (c == -1) {
            tokens.emplace_back(token::eof{line});
            return tokens;
        }

        if (line_comment)
            continue;

        if ((c >= 'a' && c <= 'z')
         || (c >= 'A' && c <= 'Z')
         || c == '_'
         || c == '+'
         || c == '-'
         || c == '/'
         || c == '*'
         || c == '^') {
            buf.push_back(c);
            s = word;
        } else if ((c >= '0' && c <= '9')) {
            buf.push_back(c);
            s = whole;
        } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            emplace_buf(buf);

            s = none;
            buf = "";
        } else if (c == ':') {
            emplace_buf(buf);

            tokens.emplace_back(token::start_fn{line});
            s = none;
            buf = "";
        } else if (c == ';') {
            emplace_buf(buf);

            tokens.emplace_back(token::end_fn{line});
            s = none;
            buf = "";
        } else if (c == '"') {
            if (s == string) {
                tokens.emplace_back(token::string{buf, line});
                s = none;
                buf = "";
            } else if (s == none) {
                s = string;
            } else if (s == string_escape) {
                buf.push_back(c);
                s = string;
            }
        } else if (c == '\\') {
            if (s == string) {
                s = string_escape;
            } else {
                line_comment = true;
            }
        } else {
            if (c == string) {
                buf.push_back(c);
            } else {
                error("Unexpected char `" + std::to_string(c) + "`");
            }
        }
    }

    return tokens;
}
