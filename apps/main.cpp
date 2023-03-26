#include <iostream>

#include <learncpp/square/square.hpp>
#include <learncpp/automata/nfa.hpp>

using namespace std;

int main()
{
    cout << square(5) << endl;
    automata::NfaNode<int> node;
    automata::Nfa<int> n(2);

    node.add_connection(3, nullptr);
    return 0;
}
