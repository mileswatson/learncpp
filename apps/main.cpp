#include <iostream>

#include <learncpp/square/square.hpp>
#include <learncpp/automata/nfa.hpp>

using namespace std;

int main()
{
    cout << square(5) << endl;
    automata::NfaNode<int, string> node("3");
    automata::Nfa<int> n(2);
    automata::Nfa<int, char> x();

    node.add_connection(3, &node);
    return 0;
}
