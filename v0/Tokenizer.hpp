#pragma once
#include <cstddef>
#include <vector>
#include <stack>
#include <string>
#include <unordered_map>
#include "Tokens.hpp"



//TokenizerState Groups:
#define TokenizerState_Normal 0
#define TokenizerState_InComment 1
#define TokenizerState_InLiteral 2

//Get group from enum value. checks last 7 bits (of 32)
#define GetTokenizerStateGroup(INDEX) ((((unsigned int)INDEX) & 0xFE000000) >> 25)

enum class TokenizerState : uint_fast32_t
{

#define State (__COUNTER__ | ((GROUP << 25))) //Encode group into last 7 bits (of 32)

#define GROUP TokenizerState_Normal
    Normal,
#undef GROUP

#define GROUP TokenizerState_InComment
    InComment = State,
    InCommentMultiline = State,
#undef GROUP

#define GROUP TokenizerState_InLiteral
    InStringQuote = State,
    InStringDoubleQuote = State,
    InStringBacktick = State,
    InInteger = State,
    InDecimal = State,
    InIdentifier = State
#undef GROUP

#undef State
};

struct Tokenizer;


struct TokenPack
{

    //We will cast std::size_t to:
    //TokenType type;
    //void* data;
    /*
    //Vector structure example:
    TokenType (token type without data)
    TokenType (token type without data)
    TokenType (token type with data)
    data (index of data or )
    TokenType (token type without data)
    */
    std::vector<Token> tokens;

    //Identifier strings and their id, id gets used in tokenStream
    std::unordered_map<std::string, std::size_t> identifiers;
    //Literal string and identifier data
    std::vector<std::string> stringData;
    //decimals and integers are casted from std::size_t

    TokenPack(const std::string &code);

    void Debug();
    void DebugStep(Tokenizer *t);

    std::string GetIdentifierById(std::size_t id);
    
    inline void AddToken(Tokenizer *t, TokenType token);
    inline void AddToken(Tokenizer *t, Token token);
    inline Token StoreIdentifier(Tokenizer *t);
    inline Token StoreString(Tokenizer *t);
    inline bool AddIdentifier(Tokenizer *t);
    inline void AddLiteralInteger(Tokenizer *t);
    inline void AddLiteralDecimal(Tokenizer *t);
    inline void AddLiteralString(Tokenizer *t);
    inline void AddLiteralIdentifier(Tokenizer *t);
};

struct Tokenizer
{
    TokenPack *tokenPack;

    std::size_t identifierCounter;

    //char pc; // previous char//////////////////////Dok se ne ispravi pc == ':' || pc == '=' || pc =='^' nema!
    char c; // current char
    bool escapeNext;
    TokenizerState state;
    std::string charBuffer;

    //sum of all lower (inner) tokens.length()
    //used so you know where each tree ends
    std::vector<std::size_t> tokenTreeStack;
    /*
    //count is used for serializing all tokens into flat vector (if compiling)
    //not needed if going to be executed

    token
    token
    token(token tree)
    7 (num of nodes till closing brace/bracket/parenthese/identifier)
    {
        token(var 1)
        token(token tree)
        3 (num of nodes till closing brace/bracket/parenthese/identifier)
        {
            token(var a)
            token(var b)
            token(var c)
        }
        token(var 2)
    }
    token
    token

    */

    Tokenizer(TokenPack *_tokenPack);

    void FeedChar();
    bool HandleSpecial(unsigned int helperStateGroup);
    bool HandleComments(unsigned int helperStateGroup);
    bool HandleLiterals(unsigned int helperStateGroup);
    bool HandleFeedChar(unsigned int helperStateGroup);
    inline bool IsValidIdentifierChar(char c, bool first = true);
    inline void ResetAll();
    inline void ResetBuffer();
};