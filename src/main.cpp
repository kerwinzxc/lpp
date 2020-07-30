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
#include <cstring>

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
    while (std::getline(f, line))
    {
        ss << line << '\n';
    }
    f.close();
    return ss.str();
}

static std::string ReadStdIn()
{
    std::string line;
    std::stringstream ss;
    while (std::getline(std::cin, line))
    {
        ss << line << '\n';
    }
    return ss.str();
}

static bool print = false;
static bool noexecute = false;

bool Execute(const std::string& contents)
{
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
    if (print)
        std::cout << ss.str();

    if (noexecute)
        return true;

    // Run the Lua source code.
    return sa::lpp::Run(ss.str());
}

int main(int argc, char** argv)
{
    std::vector<std::string> files;

    for (int i = 1; i < argc; ++i)
    {
        char* arg = argv[i];
        if (strcmp(arg, "-p") == 0 || strcmp(arg, "--print") == 0)
            print = true;
        else if (strcmp(arg, "-n") == 0 || strcmp(arg, "--no-execute") == 0)
            noexecute = true;
        else if (strcmp(arg, "-h" ) == 0 || strcmp(arg, "-?" ) == 0 || strcmp(arg, "--help" ) == 0)
        {
            std::cout << "LPP" << std::endl << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << argv[0] << " [options] <file> ..." << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -h --help              Show help" << std::endl;
            std::cout << "  -p --print             Print generated source" << std::endl;
            std::cout << "  -n --no-execute        Don't execute" << std::endl;
            exit(0);
        }
        else if (strcmp(arg, "--version" ) == 0)
        {
            std::cout << "LPP 1.0" << std::endl;
            exit(0);
        }
        else
            files.push_back(arg);
    }

    if (files.size() == 0)
    {
        bool ret = Execute(ReadStdIn());
        return ret ? 0 : 1;
    }

    bool ret = true;
    for (const auto& f : files)
    {
        ret = ret && Execute(ReadFile(f));
    }
    return ret ? 0 : 1;
}
