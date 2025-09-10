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

Lexer::Lexer(DS::View<char> source, DS::Vector<Token>& tokens) : source(source), tokens(tokens) {
    this->left_pos = 0;
    this->right_pos = 0;
    this->line = 1;
    this->c = '\0';
};

void Lexer::generate_tokens(u8* data, byte_t file_size, DS::Vector<Token>& out_tokens) {
    Lexer lexer = Lexer(DS::View<char>((char*)data, file_size), out_tokens);
    while (!lexer.is_eof()) {
        lexer.consume_next_token();
    }
}

void Lexer::consume_next_char() {
    this->c = this->source.data[this->right_pos];

    if (this->c == '\n') {
        this->line += 1;
    }

    this->right_pos += 1;
}

bool Lexer::consume_whitespace() {
    if (is_whitespace(this->c)) {
        return true;
    }

    return false;
}

char Lexer::peek_nth_char(u64 n) {
    if ((this->right_pos + n) >= this->source.length) {
        return '\0';
    }

    return this->source.data[this->right_pos + n];
}

DS::View<char> Lexer::get_scratch_buffer() {
    return DS::View<char>((char*)this->source.data + this->left_pos, this->right_pos - this->left_pos);
}

void Lexer::report_error(const char* msg) {
    DS::View<char> scratch = this->get_scratch_buffer();
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

bool Lexer::consume_on_match(char expected) {
    if (this->peek_nth_char() != expected) {
        return false;
    }

    this->consume_next_char();
    return true;
}

void Lexer::consume_digit_literal() {
    if (this->consume_on_match('-')) {}
    else if (this->consume_on_match('+')) {}
    
    while (char_is_digit(peek_nth_char()) || peek_nth_char() == '.') {
        this->consume_next_char();
    }

    DS::View<char> sv = this->get_scratch_buffer();
    Token token = Token::LiteralTokenFromSourceView(sv, this->line);
    this->tokens.push(token);
}

void Lexer::consume_string_literal() {
    while (this->peek_nth_char() != '\"') {
        if (this->is_eof()) {
            this->report_error("String literal doesn't have a closing double quote!\n");
        }
        
        this->consume_next_char();
    }

    this->consume_next_char();

    DS::View<char> sv = this->get_scratch_buffer();
    Token token = Token::LiteralTokenFromSourceView(sv, this->line);
    this->tokens.push(token);
}

void Lexer::consume_character_literal() {
    if (this->consume_on_match('\'')) {
        this->report_error("character literal doesn't have any ascii data in between\n");
    }

    while (this->peek_nth_char() != '\'') {
        if (this->is_eof()) {
            this->report_error("String literal doesn't have a closing double quote!\n");
        }
        
        this->consume_next_char();
    }

    this->consume_next_char();

    DS::View<char> sv = this->get_scratch_buffer();
    Token token = Token::LiteralTokenFromSourceView(sv, this->line);
    this->tokens.push(token);
}

bool Lexer::consume_literal() {
    bool has_unary = (this->c == '-' || this->c == '+' );
    if (char_is_digit(this->c) || (has_unary && char_is_digit(peek_nth_char()))) {
        this->consume_digit_literal();
        return true;
    } else if (this->c == '\"') {
        this->consume_string_literal();
        return true;
    } else if (this->c == '\'') {
        this->consume_character_literal();
        return true;
    }

    return false;
}

bool Lexer::try_consume_word() {
    if (!char_is_alpha(this->c)) {
        return false;
    }

    while (char_is_alpha_numeric(peek_nth_char()) || peek_nth_char() == '_') {
        if (this->is_eof()) {
            break;
        }

        this->consume_next_char();
    }

    return true;
}

bool Lexer::consume_word() {
    if (!this->try_consume_word()) {
        return false;
    }

    DS::View<char> sv = this->get_scratch_buffer();

    Token token = Token::KeywordTokenFromSourceView(sv, this->line);
    if (token.type != TOKEN_ILLEGAL_TOKEN) {
        this->tokens.push(token);
        
        return true;
    }

    token.type = TOKEN_IDENTIFIER;
    this->tokens.push(token);

    return true;
}

void Lexer::consume_until_new_line() {
    while (!this->is_eof() && peek_nth_char() != '\n') {
        this->consume_next_char();
    }
}

bool Lexer::consume_syntax() {
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

    DS::View<char> sv = this->get_scratch_buffer();
    Token token = Token::SyntaxTokenFromSourceView(sv, this->line);
    if (token.type != TOKEN_ILLEGAL_TOKEN) {
        this->tokens.push(token);
        return true;
    }

    return false;
}

void Lexer::consume_next_token() {
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

bool Lexer::is_eof() {
    return this->right_pos >= this->source.length;
}