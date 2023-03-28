#ifndef NFA_HPP
#define NFA_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <memory>
#include <iterator>

#include "nfa_node.hpp"

using namespace std;

namespace automata
{
    template <typename T, typename I>
    class Dfa;

    template <typename T, typename I = int, bool fully_defined = true>
    class Nfa;

    template <typename T, typename I>
    Nfa<T, I> concat(Nfa<T, I> &&first, Nfa<T, I> &&second)
    {
        for (auto &[k, v] : second.nodes)
        {
            first.nodes.emplace(k, move(v));
        }
        first.end->add_connection({}, second.start);
        first.end = second.end;
        return move(first);
    };

    template <typename T, typename I>
    Nfa<T, I> either(Nfa<T, I> &&left, Nfa<T, I> &&right)
    {
        for (auto &[k, v] : right.nodes)
        {
            left.nodes.emplace(k, move(v));
        }
        left.start->add_connection({}, right.start);
        right.end->add_connection({}, left.end);
        return move(left);
    }

    template <typename T, typename I>
    Nfa<T, I> zeroOrMore(Nfa<T, I> &&nfa)
    {
        nfa.start->add_connection({}, nfa.end);
        nfa.end->add_connection({}, nfa.start);
        return move(nfa);
    }

    template <typename T>
    Nfa<T> epsilon()
    {
        return Nfa<T>();
    }

    template <typename T>
    Nfa<T> match(T accept)
    {
        return Nfa<T>(accept);
    }

    template <typename T>
    Nfa<T> matchAny(const unordered_set<T> &acceptAny)
    {
        return Nfa<T>(acceptAny);
    }

    template <typename T, typename I>
    class Nfa<T, I, false>
    {
    protected:
        NfaNode<T, I> *start;
        NfaNode<T, I> *end;
        unordered_map<I, unique_ptr<NfaNode<T, I>>> nodes;

        Nfa() = default;

        Nfa(const Nfa<T, I> &) = delete;
        Nfa<T, I> &operator=(const Nfa<T, I> &) = delete;

    public:
        Nfa(Nfa<T, I, false> &&) = default;
        Nfa<T, I, false> &operator=(Nfa<T, I, false> &&) = default;

        template <typename Iter>
            requires input_iterator<Iter> &&
                     same_as<typename Iter::value_type, T>
        optional<Iter> longest_match(Iter b, Iter e) const
        {
            unordered_set<const NfaNode<T, I> *> current = start->epsilon_closure();
            optional<Iter> lastMatch = current.contains(end) ? optional(b) : nullopt;
            for (; b != e && !current.empty(); b++)
            {
                unordered_set<const NfaNode<T, I> *> next;
                for (auto &c : current)
                {
                    auto beforeClosureOpt = c->next(*b);
                    if (!beforeClosureOpt)
                        continue;
                    const unordered_set<NfaNode<T, I> *> &beforeClosure = *beforeClosureOpt;
                    for (auto &x : beforeClosure)
                    {
                        unordered_set<const NfaNode<T, I> *> afterClosure = x->epsilon_closure();
                        next.merge(afterClosure);
                    }
                }
                current = move(next);
                if (current.contains(end))
                    lastMatch = optional(b + 1);
            }
            return lastMatch;
        }

        template <typename U, typename V>
        friend Nfa<U, V> concat(Nfa<U, V> &&, Nfa<U, V> &&);

        template <typename U, typename V>
        friend Nfa<U, V> either(Nfa<U, V> &&, Nfa<U, V> &&);

        template <typename U, typename V>
        friend Nfa<U, V> zeroOrMore(Nfa<U, V> &&);

        template <typename U, typename V>
        friend class Dfa;
    };

    template <typename T, typename I>
    class Nfa<T, I> : public Nfa<T, I, false>
    {
    public:
        Nfa() : Nfa<T, I, false>::Nfa()
        {
        }
    };

    template <typename T>
    class Nfa<T, int> : public Nfa<T, int, false>
    {
    private:
        inline static int nextId = 0;

    public:
        Nfa()
        {
            auto s = make_unique<NfaNode<T, int>>(nextId++);
            this->start = &*s;
            this->end = &*s;
            this->nodes[s->get_id()] = move(s);
        }

        Nfa(T accept)
        {
            auto s = make_unique<NfaNode<T, int>>(nextId++);
            auto e = make_unique<NfaNode<T, int>>(nextId++);
            this->start = &*s;
            this->end = &*e;
            this->nodes[s->get_id()] = move(s);
            this->nodes[e->get_id()] = move(e);

            this->start->add_connection(accept, this->end);
        }

        Nfa(const unordered_set<T> &acceptAny)
        {
            auto s = make_unique<NfaNode<T, int>>(nextId++);
            auto e = make_unique<NfaNode<T, int>>(nextId++);
            this->start = &*s;
            this->end = &*e;
            this->nodes[s->get_id()] = move(s);
            this->nodes[e->get_id()] = move(e);

            for (const auto &accept : acceptAny)
            {
                this->start->add_connection(accept, this->end);
            }
        }
    };
}

#endif