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
#include <unistd.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

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

    int c;
#ifdef HAVE_GETOPT_H
    static struct option longOptions[] =
    {
    //   NAME           ARGUMENT     FLAG  SHORTNAME
        {"print",       no_argument, NULL, 'p'},
        {"no-execute",  no_argument, NULL, 'n'},
        {"help",        no_argument, NULL, 'h'},
        {"version",     no_argument, NULL, 'v'},
        {NULL,          0,           NULL, 0}
    };
    int optionIndex = 0;
    while ((c = getopt_long(argc, argv, "pnhv", longOptions, &optionIndex)) != -1)
#else
    while ((c = getopt(argc, argv, "pnhv")) != -1)
#endif
    {
        switch (c)
        {
        case 'p':
            print = true;
            break;
        case 'n':
            noexecute = true;
            break;
        case 'h':
            std::cout << "LPP" << std::endl << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << argv[0] << " [options] <file> ..." << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
#ifdef HAVE_GETOPT_H
            std::cout << "  -p --print             Print generated source" << std::endl;
            std::cout << "  -n --no-execute        Don't execute" << std::endl;
            std::cout << "  -h --help              Show help" << std::endl;
            std::cout << "  -v --version           Show program version" << std::endl;
#else
            std::cout << "  -p              Print generated source" << std::endl;
            std::cout << "  -n              Don't execute" << std::endl;
            std::cout << "  -h              Show help" << std::endl;
            std::cout << "  -v              Show program version" << std::endl;
#endif
            exit(0);
        case 'v':
            std::cout << "LPP 1.0" << std::endl;
            exit(0);
        default:
            break;
        }
    }
    if (optind < argc)
    {
        while (optind < argc)
            files.push_back(argv[optind++]);
    }

    if (files.size() == 0)
    {
        bool ret = Execute(ReadStdIn());
        return ret ? 0 : 1;
    }

    bool ret = true;
    for (const auto& f : files)
    {
        if (!Execute(ReadFile(f)))
            ret = false;
    }
    return ret ? 0 : 1;
}
