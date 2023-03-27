#include <iostream>

#include <learncpp/square/square.hpp>
#include <learncpp/automata/nfa.hpp>
#include <learncpp/automata/dfa.hpp>

using namespace std;

int main()
{
    cout << square(5) << endl;
    automata::NfaNode<int, string> node("3");
    automata::Nfa<int> n(2);
    automata::Nfa<int, char> x;
    automata::Dfa<char, int> y({}, 0);

    string input = "123";
    y.longest_match(input.begin(), input.end());

    node.add_connection(3, &node);
    return 0;
}
