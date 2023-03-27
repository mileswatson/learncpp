#include <gtest/gtest.h>
#include <learncpp/automata/nfa.hpp>
#include <memory>

using namespace std;
using namespace automata;

TEST(NfaTest, CreateNode)
{
    auto a = make_unique<NfaNode<char>>();
    auto b = make_unique<NfaNode<char>>();

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

TEST(NfaTest, AndThen)
{
    auto a = Nfa('a');
    auto b = Nfa('b');
    auto aAndB = Nfa<char>::and_then(move(a), move(b));
    vector<string> failInputs = {"", "a", "b", "aab", "c", "cab"};
    for (auto &f : failInputs)
    {
        auto match = aAndB.longest_match(f.begin(), f.end());
        if (match)
            FAIL();
    }
    vector<string> succeedInputs = {"ab", "abc", "abbc"};
    for (auto &s : succeedInputs)
    {
        auto match = aAndB.longest_match(s.begin(), s.end());
        if (!match || *match == s.begin() + 2)
            FAIL();
    }
}

TEST(NfaTest, EpsilonClosure)
{
    auto a = make_unique<NfaNode<char>>();
    auto b = make_unique<NfaNode<char>>();
    auto c = make_unique<NfaNode<char>>();
    auto d = make_unique<NfaNode<char>>();

    a->add_connection({}, &*a);
    a->add_connection({}, &*b);
    b->add_connection({}, &*c);
    b->add_connection('d', &*d);
    c->add_connection({}, &*a);

    auto epsilon_closure = a->epsilon_closure();

    ASSERT_EQ(epsilon_closure, unordered_set({&*a, &*b, &*c}));
}
