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

#include <lpp/lpp.h>
#include <iostream>
#include <sstream>
#include <fstream>

static std::string ReadFile(const std::string& fileName)
{
    std::ifstream f(fileName);
    if (!f.is_open())
    {
        std::cerr << "Unable to open file " << fileName << std::endl;
        return "";
    }

    std::string line;
    std::stringstream ss;
    while (std::getline(f,line))
    {
        ss << line << '\n';
    }
    f.close();
    return ss.str();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: lpp <file>" << std::endl;
        return 1;
    }

    const std::string contents = ReadFile(argv[1]);

    sa::lpp::Tokenizer t;
    // When there is an include, read the file contents and return it.
    t.onGetFile_ = [](const std::string& f) -> std::string
    {
        return ReadFile(f);
    };
    // Create a list of tokens from the source file
    const sa::lpp::Tokens tokens = t.Parse(contents);

    // Generate Lua source code from the tokens
    std::stringstream ss;
    sa::lpp::Generate(tokens, [&ss](const std::string& value)
    {
        ss << value;
    });

    // Run the Lua source code.
    bool ret = sa::lpp::Run(ss.str());
    return ret ? 0 : 1;
}
