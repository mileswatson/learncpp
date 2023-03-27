#ifndef DFA_HPP
#define DFA_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <memory>
#include <iterator>

#include "dfa_node.hpp"

using namespace std;

namespace automata
{
    template <typename T, typename I>
    class Dfa
    {
    protected:
        DfaNode<T, I> *start;
        I end;
        vector<unique_ptr<DfaNode<T, I>>> nodes;

        Dfa(const Dfa<T, I> &) = delete;
        Dfa<T, I> &operator=(const Dfa<T, I> &) = delete;

    public:
        Dfa(Dfa<T, I> &&) = default;
        Dfa<T, I> &operator=(Dfa<T, I> &&) = default;

        Dfa(unordered_set<I> &&start, I &&end) : end(end)
        {
            auto s = make_unique<DfaNode<T, I>>(move(start));
            this->start = &*s;
            nodes.push_back(move(s));
        }

        template <typename Iter>
            requires input_iterator<Iter> &&
                     same_as<typename Iter::value_type, T>
        optional<Iter> longest_match(Iter b, Iter e) const
        {
            DfaNode<T, I> *current = start;
            optional<Iter> lastMatch = current->contains(end) ? optional(b) : nullopt;
            for (; b != e; b++)
            {
                auto next = current->next(*b);

                if (!next)
                {
                    break;
                }
                current = *next;
                if (current->contains(end))
                    lastMatch = optional(b + 1);
            }
            return lastMatch;
        }

        template <typename U, typename V>
        friend Dfa<U, V> concat(Dfa<U, V> &&, Dfa<U, V> &&);

        template <typename U, typename V>
        friend Dfa<U, V> either(Dfa<U, V> &&, Dfa<U, V> &&);

        template <typename U, typename V>
        friend Dfa<U, V> zeroOrMore(Dfa<U, V> &&);
    };

}

#endif