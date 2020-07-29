#include <lpp/lpp.h>
#include <iostream>

int main()
{
    sa::lpp::Tokenizer t;
    static const char* TEST = R"(
<% for i = 10,1,-1 do %>
    <% if (i == 5) then %>
    five!
    <% end %>
    print this 10 times
    <% end %>)";
    sa::lpp::Tokens tokens = t.Parse(TEST);
    for (const auto& token : tokens)
    {
        std::cout << token.value << std::endl;
    }

    sa::lpp::Generator gen;
    std::cout << gen.Generate(tokens);
    return 0;
}
