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

        pair<int, bool> find_or_insert(const unordered_set<const NfaNode<T, I> *> &states)
        {
            unordered_set<I> subset;
            for (auto &s : states)
            {
                subset.emplace(s->get_id());
            }
            for (int i = 0; i < nodes.size(); i++)
            {
                if (nodes[i]->get_nfa_states() == subset)
                    return make_pair(i, false);
            }
            nodes.push_back(make_unique<DfaNode<T, I>>(move(subset)));
            return make_pair(nodes.size() - 1, true);
        }

        int visit(const Nfa<T, I> &nfa, const unordered_set<const NfaNode<T, I> *> &states)
        {
            const auto [from, inserted] = find_or_insert(states);
            if (!inserted)
                return from;
            unordered_set<T> valid_inputs;
            for (const auto &state : states)
            {
                state->add_valid_inputs(valid_inputs);
            }

            for (const auto &input : valid_inputs)
            {
                unordered_set<const NfaNode<T, I> *> next;
                for (const auto &state : states)
                {
                    if (auto n = state->next(input))
                    {
                        auto copied = n->get();
                        next.merge(copied);
                    }
                }
                int to = visit(nfa, epsilon_closure(next));
                nodes[from]->add_connection(input, &*nodes[to]);
            }
            return from;
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

        Dfa(Nfa<T, I> &nfa) : end(nfa.end->get_id())
        {
            int s = visit(nfa, nfa.start->epsilon_closure());
            start = &*nodes[s];
        }

        template <typename Iter>
            requires input_iterator<Iter> &&
                     same_as<typename Iter::value_type, T>
        optional<Iter> longest_match(Iter b, Iter e) const
        {
            const DfaNode<T, I> *current = start;
            optional<Iter> lastMatch = current->contains(end) ? optional(b) : nullopt;
            for (; b != e; b++)
            {
                auto next = current->next(*b);

                if (!next)
                    break;
                current = *next;
                if (current->contains(end))
                    lastMatch = optional(b + 1);
            }
            return lastMatch;
        }
    };

}

#endif