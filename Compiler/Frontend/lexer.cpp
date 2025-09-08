#include "lexer.hpp"

static bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0';
}

#define char_is_digit(c) (c >= '0' && c <= '9')
#define char_is_upper(c) (c >= 'A' && c <= 'Z')
#define char_is_lower(c) (c >= 'a' && c <= 'z')

UNUSED_FUNCTION static bool char_is_alpha(char c) {
    c = (char)((int)c & (0b11011111)); // mask off the 32 bit
    return char_is_upper(c);
}

UNUSED_FUNCTION static bool char_is_alpha_numeric(char c) {
    return char_is_alpha(c) || char_is_digit(c);
}

struct T_Lexer {
    Container::View<char> source;
    Container::Vector<Token>& tokens;
    u32 left_pos;
    u32 right_pos;
    u32 line;
    char c;

    T_Lexer(Container::View<char> source, Container::Vector<Token>& tokens) : source(source), tokens(tokens) {
        this->left_pos = 0;
        this->right_pos = 0;
        this->line = 1;
        this->c = '\0';
    };

    void consume_next_char() {
        this->c = this->source.data[this->right_pos];

        if (this->c == '\n') {
            this->line += 1;
        }

        this->right_pos += 1;
    }

    bool consume_whitespace() {
        if (is_whitespace(this->c)) {
            return true;
        }

        return false;
    }

    char peek_nth_char(u64 n = 0) {
        if ((this->right_pos + n) >= this->source.length) {
            return '\0';
        }

        return this->source.data[this->right_pos + n];
    }

    Container::View<char> get_scratch_buffer() {
        return Container::View<char>((char*)this->source.data + this->left_pos, this->right_pos - this->left_pos);
    }

    void report_error(const char* msg) {
        Container::View<char> scratch = this->get_scratch_buffer();
        LOG_ERROR("String: %.*s\n", (int)scratch.length, scratch.data); 

        LOG_ERROR("Bytes: ");
        for (int i = 0; i < (int)scratch.length; i++) {
            if (i == scratch.length - 1) {
                LOG_TRACE("%d\n", scratch.data[i]);
            } else {
                LOG_TRACE("%d, ", scratch.data[i]);
            }

        }

        RUNTIME_ASSERT_MSG(false, "[LEXER ERROR] line: %d | %s", this->line, msg);
    }

    bool consume_on_match(char expected) {
        if (this->peek_nth_char() != expected) {
            return false;
        }

        this->consume_next_char();
        return true;
    }


    void add_token(TokenType token_type) {
        Container::View<char> sv = this->get_scratch_buffer();
        Token token = token_from_string(token_type, sv, line);
        this->tokens.push(token);
    }

    void consume_digit_literal() {
        TokenType token_type = TOKEN_INTEGER_LITERAL;

        this->consume_on_match('-');

        while (char_is_digit(peek_nth_char()) || peek_nth_char() == '.') {
            if (this->c == '.') {
                token_type = TOKEN_FLOAT_LITERAL;
            }

            this->consume_next_char();
        }

        this->add_token(token_type);
    }

    /*
    void consume_string_literal() {
        while (this->peek_nth_char() != '\"') {
            if (this->is_eof()) {
                this->report_error("String literal doesn't have a closing double quote!\n");
            }
            
            this->consume_next_char();
        }

        this->consume_next_char();
        // this->add_token(lexer, TOKEN_STRING_LITERAL);
    }

    void consume_character_literal() {
        if (consume_on_match(lexer, '\'')) {
            report_error(lexer, "character literal doesn't have any ascii data in between\n");
        }

        while (peek_nth_char(lexer, 0) != '\'') {
            if (is_EOF(lexer)) {
                report_error(lexer, "String literal doesn't have a closing double quote!\n");
            }
            
            consume_next_char(lexer);
        }

        consume_next_char(lexer);
        add_token(lexer, SPL_TOKEN_CHARACTER_LITERAL);
    }
    */

    bool consume_literal() {
        if (char_is_digit(this->c) || (this->c == '-' && char_is_digit(peek_nth_char()))) {
            this->consume_digit_literal();
            return true;
        }
        
        /*
        } else if (this->c == '\"') {
            this->consume_string_literal();
            return true;
        } else if (this->c == '\'') {
            this->consume_character_literal();
            return true;
        }
        */

        return false;
    }

    /*
    bool try_consume_word() {
        if (!ckg_char_is_alpha(this->c)) {
            return false;
        }

        while (ckg_char_is_alpha_numeric(peek_nth_char(lexer, 0)) || peek_nth_char(lexer, 0) == '_') {
            if (is_EOF(lexer)) {
                break;
            }

            consume_next_char(lexer);
        }

        return true;
    }

    bool consume_word() {
        if (!try_consume_word(lexer)) {
            return false;
        }

        Container::View<u8> scratch = get_scratch_buffer(lexer);

        SPL_TokenType token_type = token_get_keyword(scratch.data, scratch.length);
        if (token_type != SPL_TOKEN_ILLEGAL_TOKEN) {
            add_token(lexer, token_type);
            return true;
        }

        add_token(lexer, SPL_TOKEN_IDENTIFIER);
        return true;
    }
    */

    void consume_until_new_line() {
        while (!this->is_eof() && peek_nth_char() != '\n') {
            this->consume_next_char();
        }
    }

    bool consume_syntax() {
        if (this->c == '/') {
            if (this->consume_on_match('/')) {
                this->consume_until_new_line();
                // add_token(lexer, SPL_TOKEN_COMMENT);
                return true;
            } else if (this->consume_on_match('*')) {
                while (!(this->peek_nth_char() == '*' && peek_nth_char(1) == '/')) {
                    if (this->is_eof()) {
                        this->report_error("Multiline comment doesn't terminate\n");
                    }
                    
                    this->consume_next_char();
                }

                this->consume_next_char(); // consume '*'
                this->consume_next_char(); // consume '/'
                // add_token(lexer, SPL_TOKEN_COMMENT);
                return true;
            }
        } else if (this->c == '<') {
            this->consume_on_match('<');
            this->consume_on_match('=');
        } else if (this->c == '>') {
            this->consume_on_match('>');
            this->consume_on_match('=');
        } else if (this->c == '-') {
            if (!this->consume_on_match('-')) {
                this->consume_on_match('=');
            }
        } else if (this->c == '+') {
            if (!this->consume_on_match('+')) {
                this->consume_on_match('=');
            }
        } else if (this->c == '|') {
            if (!this->consume_on_match('|')) {
                this->consume_on_match('=');
            }
        } else if (this->c == '&') {
            if (!this->consume_on_match('&')) {
                this->consume_on_match('=');
            }
        } else if (this->c == '!' || this->c == '^' || this->c == '*' || this->c == '=') {
            this->consume_on_match('=');
        }

        Container::View<char> buffer = this->get_scratch_buffer();
        TokenType token_type = token_type_from_syntax(buffer.data, buffer.length);
        if (token_type != TOKEN_ILLEGAL_TOKEN) {
            this->add_token(token_type);
            return true;
        }

        return false;
    }

    void consume_next_token() {
        this->left_pos = this->right_pos;
        this->consume_next_char();

        if (this->consume_whitespace()) {}
        else if (this->consume_literal()) {}
        //else if (consume_word()) {}
        else if (this->consume_syntax()) {}
        else {
            this->report_error("Illegal token found\n");
        }
    }

    bool is_eof() {
       return this->right_pos >= this->source.length;
    }
};

namespace Frontend::Lexer {
    void generate_tokens(u8* data, byte_t file_size, Container::Vector<Token>& out_tokens) {
        T_Lexer lexer = T_Lexer(Container::View<char>((char*)data, file_size), out_tokens);
        while (!lexer.is_eof()) {
            lexer.consume_next_token();
        }

        // ckg_vector_push(lexer->tokens, SPL_TOKEN_CREATE_CUSTOM(SPL_TOKEN_EOF, CKG_SV_NULL(), lexer->line));
    }
}