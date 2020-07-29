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

## Usage

~~~sh
$ lpp <filename>
~~~

