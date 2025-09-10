#pragma once

#include "../DataStructure/ds.hpp"
#include "token.hpp"

struct Lexer {
    static void generate_tokens(u8* data, byte_t file_size, DS::Vector<Token>& out_tokens);

    private:
        DS::View<char> source;
        DS::Vector<Token>& tokens;
        u32 left_pos;
        u32 right_pos;
        u32 line;
        char c;

        Lexer(DS::View<char> source, DS::Vector<Token>& tokens);

        void consume_next_char();
        bool consume_whitespace();
        char peek_nth_char(u64 n = 0);
        DS::View<char> get_scratch_buffer();
        void report_error(const char* msg);
        bool consume_on_match(char expected);

        void consume_digit_literal();
        void consume_string_literal();
        void consume_character_literal();
        bool consume_literal();
        bool try_consume_word();
        bool consume_word();
        void consume_until_new_line();
        bool consume_syntax();
        void consume_next_token();
        bool is_eof();
    };