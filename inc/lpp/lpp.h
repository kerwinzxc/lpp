/**
 * Copyright 2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <sstream>

namespace sa {
namespace lpp {

class Tokenizer;

struct Token
{
    enum class Type
    {
        Invalid,
        Literal,
        Code,
        Include,
    };

    Type type;
    size_t start;
    size_t end;
    std::string value;
};

using Tokens = std::vector<Token>;

class Tokenizer
{
private:
    size_t index_{ 0 };
    std::string_view source_;
    bool Eof() const { return index_ >= source_.size(); }
    char Next() { return source_.at(index_++); }
    char Peek(size_t offset = 1) const
    {
        if (source_.size() > index_ + offset)
            return source_.at(index_ + offset);
        return 0;
    }
    bool Matches(std::string_view value)
    {
        for (size_t i = 0; i < value.length(); ++i)
            if (Peek(i) != value[i])
                return false;
        return true;
    }
    void SkipWhite()
    {
        while (isspace(Peek(0)))
            ++index_;
    }
    bool ParseInclude(Token& token)
    {
        index_ += std::string_view("include ").length();
        SkipWhite();
        if (!Matches("\""))
            return false;
        ++index_;
        token.start = index_;
        while (Peek(0) != '\"')
            ++index_;
        token.type = Token::Type::Include;
        token.end = index_;
        ++index_;
        token.value = std::string(&source_[token.start], token.end - token.start);
        std::cout << token.value << std::endl;
        return true;
    }
    Token GetNextToken()
    {
        Token result;
        result.type = Token::Type::Invalid;
        result.start = index_;
        while (!Eof())
        {
            char c = Next();
            switch (c)
            {
            case '<':
            {
                char c2 = Peek(0);
                if (c2 == '%')
                {
                    if (result.type != Token::Type::Invalid)
                    {
                        result.end = index_ - 1;
                        result.value = std::string(&source_[result.start], result.end - result.start);
                        --index_;
                        return result;
                    }
                    result.type = Token::Type::Code;
                    result.start += 2;
                    ++index_;
                }
                break;
            }
            case '%':
            {
                char c2 = Peek(0);
                if (c2 == '>')
                {
                    if (result.type == Token::Type::Code || result.type == Token::Type::Include)
                    {
                        result.end = index_ - 1;
                        ++index_;
                        if (result.type != Token::Type::Include)
                            result.value = std::string(&source_[result.start], result.end - result.start);
                        return result;
                    }
                }
                break;
            }
            case '\0':
                result.end = index_;
                result.value = std::string(&source_[result.start], result.end - result.start);
                return result;
            case 'i':
            {
                if (result.type == Token::Type::Code)
                {
                    --index_;
                    if (Matches("include "))
                    {
                        ParseInclude(result);
                        break;
                    }
                }
                [[fallthrough]];
            }
            default:
                if (result.type == Token::Type::Invalid)
                    result.type = Token::Type::Literal;
                break;
            }
        }
        result.end = index_;
        result.value = std::string(&source_[result.start], result.end - result.start);
        return result;
    }
    void Reset()
    {
        index_ = 0;
    }
public:
    Tokens Parse(std::string_view source)
    {
        source_ = source;
        Reset();
        Tokens result;
        while (!Eof())
            result.push_back(GetNextToken());
        return result;
    }
    void Append(std::string_view source, Tokens& tokens)
    {
        source_ = source;
        Reset();
        while (!Eof())
            tokens.push_back(GetNextToken());
    }
};

template <typename charType>
std::basic_string<charType> Trim(const std::basic_string<charType>& str,
    const std::basic_string<charType>& whitespace = " \t")
{
    // Left
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return std::basic_string<charType>(); // no content

    // Right
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

template <typename charType>
bool ReplaceSubstring(std::basic_string<charType>& subject,
    const std::basic_string<charType>& search,
    const std::basic_string<charType>& replace)
{
    if (search.empty())
        return false;
    bool result = false;

    using string_type = std::basic_string<charType>;
    string_type newString;
    newString.reserve(subject.length());

    size_t lastPos = 0;
    size_t findPos;

    while ((findPos = subject.find(search, lastPos)) != string_type::npos)
    {
        newString.append(subject, lastPos, findPos - lastPos);
        newString += replace;
        lastPos = findPos + search.length();
        result = true;
    }

    newString += subject.substr(lastPos);

    subject.swap(newString);
    return result;
}

class Generator
{
private:
    static std::string EscapeLiteral(const std::string& value)
    {
        std::string result = Trim(value, std::string(" \t\n"));
        if (result.empty())
            return result;
        ReplaceSubstring(result, std::string("\n"), std::string("\\n"));
        return result;
    }
public:
    std::string Generate(const Tokens& tokens)
    {
        std::stringstream ss;
        for (const auto& token : tokens)
        {
            switch (token.type)
            {
            case Token::Type::Invalid:
                continue;
            case Token::Type::Literal:
            {
                std::string value = EscapeLiteral(token.value);
                if (!value.empty())
                    ss << "print(\"" << EscapeLiteral(token.value) << "\")" << std::endl;
                break;
            }
            case Token::Type::Code:
                ss << token.value << std::endl;
                break;
            case Token::Type::Include:
                break;
            }
        }
        return ss.str();
    }
};

}
}
