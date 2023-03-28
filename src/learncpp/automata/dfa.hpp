#ifndef DFA_HPP
#define DFA_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <memory>
#include <iterator>
#include <algorithm>

#include "dfa_node.hpp"
#include "nfa_node.hpp"
#include "nfa.hpp"

using namespace std;

namespace automata
{
    template <typename T, typename I = int>
    class Dfa
    {
    private:
        DfaNode<T, I> *start;
        I end;
        vector<unique_ptr<DfaNode<T, I>>> nodes;

        Dfa(const Dfa<T, I> &) = delete;
        Dfa<T, I> &operator=(const Dfa<T, I> &) = delete;

        int find_or_insert(const unordered_set<const NfaNode<T, I> *> &states)
        {
            unordered_set<I> subset;
            for (auto &s : states)
            {
                subset.emplace(s->get_id());
            }
            for (int i = 0; i < nodes.size(); i++)
            {
                if (nodes[i]->get_nfa_states() == subset)
                    return i;
            }
            nodes.push_back(make_unique<DfaNode<T, I>>(move(subset)));
            return nodes.size() - 1;
        }

    public:
        Dfa(Dfa<T, I> &&) = default;
        Dfa<T, I> &operator=(Dfa<T, I> &&) = default;

        Dfa(unordered_set<I> &&start, I &&end) : end(end)
        {
            auto s = make_unique<DfaNode<T, I>>(move(start));
            this->start = &*s;
            nodes.push_back(move(s));
        }

        int visit(const Nfa<T, I> &nfa, const unordered_set<const NfaNode<T, I> *> &states)
        {
            int index = find_or_insert(states);
            return index;
        }

        Dfa(Nfa<T, I> &nfa) : end(nfa.end->get_id())
        {
            visit(nfa, nfa.start->epsilon_closure());
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
    };

}

#endif