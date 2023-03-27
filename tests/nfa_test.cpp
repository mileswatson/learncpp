#include <gtest/gtest.h>
#include <learncpp/automata/nfa.hpp>
#include <learncpp/automata/nfa_node.hpp>
#include <memory>

using namespace std;
using namespace automata;

TEST(NfaTest, CreateNode)
{
    auto a = make_unique<NfaNode<char, int>>(0);
    auto b = make_unique<NfaNode<char, int>>(1);

    ASSERT_EQ(a->get_id(), 0);
    ASSERT_EQ(b->get_id(), 1);

    a->add_connection('a', &*b);
    a->add_connection('a', &*a);
    a->add_connection({}, &*b);

    auto aA = a->next('a');
    ASSERT_EQ(aA.size(), 2);
    ASSERT_EQ(aA.count(&*a), 1);
    ASSERT_EQ(aA.count(&*b), 1);

    auto aEpsilon = a->next({});
    ASSERT_EQ(aEpsilon.size(), 1);
    ASSERT_EQ(aEpsilon.count(&*b), 1);
}

TEST(NfaTest, EpsilonClosure)
{
    auto a = make_unique<NfaNode<char, int>>(0);
    auto b = make_unique<NfaNode<char, int>>(1);
    auto c = make_unique<NfaNode<char, int>>(2);
    auto d = make_unique<NfaNode<char, int>>(3);

    a->add_connection({}, &*a);
    a->add_connection({}, &*b);
    b->add_connection({}, &*c);
    b->add_connection('d', &*d);
    c->add_connection({}, &*a);

    auto epsilon_closure = a->epsilon_closure();

    ASSERT_EQ(epsilon_closure, unordered_set({&*a, &*b, &*c}));
}

void test_nfa(Nfa<char> &&nfa, const vector<string> &failInputs, const vector<pair<string, int>> &succeedInputs)
{
    for (auto &f : failInputs)
    {
        auto match = nfa.longest_match(f.begin(), f.end());
        if (match)
        {
            cout << f << endl;
            FAIL();
        }
    }

    for (auto &[s, l] : succeedInputs)
    {
        auto match = nfa.longest_match(s.begin(), s.end());
        if (!match || (*match != s.begin() + l))
        {
            cout << s << endl;
            cout << !!match << endl;
            FAIL();
        }
    }
}

TEST(NfaTest, empty)
{
    auto e = epsilon<char>();
    vector<pair<string, int>> succeedInputs = {make_pair("", 0), make_pair("a", 0)};
    test_nfa(move(e), {}, succeedInputs);
}

TEST(NfaTest, concat)
{
    auto ab = concat(match('a'), match('b'));
    vector<string> failInputs = {"", "a", "b", "aab", "c", "cab"};
    vector<pair<string, int>> succeedInputs = {make_pair("ab", 2), make_pair("abc", 2), make_pair("abbc", 2)};
    test_nfa(move(ab), failInputs, succeedInputs);
}

TEST(NfaTest, either)
{
    auto aOrB = either(match('a'), match('b'));
    vector<string> failInputs = {"c"};
    vector<pair<string, int>> succeedInputs = {make_pair("a", 1), make_pair("b", 1), make_pair("ab", 1), make_pair("ba", 1), make_pair("ac", 1), make_pair("bc", 1)};
    test_nfa(move(aOrB), failInputs, succeedInputs);
    test_nfa(matchAny<char>({'a', 'b'}), failInputs, succeedInputs);
}

TEST(NfaTest, zeroOrMore)
{
    auto abStar = zeroOrMore(concat(match('a'), match('b')));
    vector<string> failInputs = {};
    vector<pair<string, int>> succeedInputs = {
        make_pair("", 0), make_pair("c", 0), make_pair("ac", 0),
        make_pair("cb", 0), make_pair("ab", 2), make_pair("abc", 2), make_pair("aba", 2), make_pair("ababc", 4)};
    test_nfa(move(abStar), failInputs, succeedInputs);
}
