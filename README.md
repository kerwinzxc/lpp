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

## Template source

See `test/test.lpp`.

~~~
<%
local brand = "LPP"
local title = brand
local requirements = {
	"Lua",
	"C++17",
	"CMake to build it"
}
%>

<? "header.inc.lpp" ?>

<div class="container content">

<h1><%= brand %></h1>

<p>A minimal Lua templates engine. Emits regular Lua code.</p>
<p>It is a bit like PHP, but without PHP.</p>

<h2>License</h2>

<p>MIT</p>

<h2>Requirements</h2>
<ul>
<% for i = 1, #requirements do %>
  <li><%= requirements[i] %></li>
<% end %>
</ul>

<h2>Embedded Lua code</h2>
<pre><b>&lt;%</b> for i = 10,1,-1 do <b>%&gt;</b></pre>

<h2>Shortcut for Lua print()</h2>
<pre><b>&lt;%=</b> 1 + 1 <b>%&gt;</b></pre>
Prints: <%= 1 + 1 %>

<h2>Include a file</h2>
<pre><b>&lt;?</b> "test.lpp" <b>?&gt;</b></pre>

<h2>Usage</h2>

<pre>$ lpp &lt;filename&gt;</pre>

</div>

<? "footer.inc.lpp" ?>
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
    while (std::getline(f, line))
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
    sa::lpp::Run(ss.str());
    return 0;
}
~~~

## Build

To build the program:

~~~sh
mkdir build
cd build
cmake ..
make
~~~

## Usage

~~~sh
$ lpp <filename>
~~~

Examples

~~~sh
# Process test.lpp and print to stdout
$ lpp test.lpp
# Read from stdin and print to stdout
$ cat test.lpp | lpp
# Process test.lpp and write to test.html
$ lpp test.lpp > test.html
~~~
