# LPP

A minimal Lua templates engine. Emits regular Lua code.

It is a bit like PHP, but without PHP.

## License

MIT

## Requirements

* Lua
* C++17
* CMake to build it

## Embedded Lua code

~~~
<% for i = 10,1,-1 do %>
~~~

## Shortcut for Lua print()

~~~
<%= 1 + 1 %>
~~~

Prints: 2

## Include a file

~~~
<? "test.lpp" ?>
~~~

## Integrating

See `src/main.cpp`.

~~~cpp
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
    sa::lpp::Tokens tokens = t.Parse(contents);

    // GEnerate Lua source code from the tokens
    std::stringstream ss;
    sa::lpp::Generate(tokens, [&ss](const std::string& value)
    {
        ss << value;
    });

    // Run the Lua source code.
    sa::lpp::Run(ss.str());
    return 0;
}
~~~

## Usage

~~~sh
$ lpp <filename>
~~~

