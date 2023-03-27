#ifndef NFA_HPP
#define NFA_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <memory>
#include <iterator>

using namespace std;

namespace automata
{
    template <typename T, typename I = int, bool fully_defined = true>
    class NfaNode;

    template <typename T, typename I>
    class NfaNode<T, I, false>
    {
    protected:
        I id;
        unordered_map<optional<T>, unordered_set<NfaNode<T, I> *>> connections;

        NfaNode(const NfaNode<T, I> &) = delete;
        NfaNode<T, I> &operator=(const NfaNode<T, I> &) = delete;
        NfaNode(NfaNode<T, I> &&) = delete;
        NfaNode<T, I> &operator=(NfaNode<T, I> &&) = delete;

    public:
        NfaNode(I id) : id(id)
        {
        }

        const I &get_id()
        {
            return id;
        }
        void add_connection(optional<T> input, NfaNode<T, I> *destination)
        {
            unordered_set<NfaNode<T, I> *> &s = connections[input];
            s.emplace(destination);
        }

        const unordered_set<NfaNode<T, I> *> &next(optional<T> input)
        {
            return connections[input];
        }

        void visit_epsilon_closure(unordered_set<NfaNode<T, I> *> &visited)
        {
            if (!visited.emplace(static_cast<NfaNode<T, I> *>(this)).second)
                return;
            const unordered_set<NfaNode<T, I> *> &toVisit = this->connections[{}];
            for (auto &v : toVisit)
            {
                v->visit_epsilon_closure(visited);
            }
        };

        unordered_set<NfaNode<T, I> *> epsilon_closure()
        {
            unordered_set<NfaNode<T, I> *> visited;
            visit_epsilon_closure(visited);
            return visited;
        }
    };

    template <typename T, typename I>
    class NfaNode<T, I, true> : public NfaNode<T, I, false>
    {
        using NfaNode<T, I, false>::NfaNode;
    };

    template <typename T>
    class NfaNode<T, int, true> : public NfaNode<T, int, false>
    {
        using NfaNode<T, int, false>::NfaNode;
        static inline int nextId = 0;

    public:
        NfaNode() : NfaNode<T, int, false>(nextId++) {}
    };

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
    Nfa<T, int> epsilon()
    {
        return Nfa<T, int>();
    }

    template <typename T>
    Nfa<T, int> match(T accept)
    {
        return Nfa<T, int>(accept);
    }

    template <typename T>
    Nfa<T, int> matchAny(const unordered_set<T> &acceptAny)
    {
        return Nfa<T, int>(acceptAny);
    }

    template <typename T, typename I>
    class Nfa<T, I, false>
    {
    protected:
        NfaNode<T> *start;
        NfaNode<T> *end;
        unordered_map<int, unique_ptr<NfaNode<T>>> nodes;

        Nfa() = default;

        Nfa(NfaNode<T> *start, NfaNode<T> *end, unordered_map<int, unique_ptr<NfaNode<T>>> &&nodes) : start(start), end(end), nodes(move(nodes))
        {
        }

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
            unordered_set<NfaNode<T> *> current = start->epsilon_closure();
            optional<Iter> lastMatch = current.contains(end) ? optional(b) : nullopt;
            for (; b != e && !current.empty(); b++)
            {
                unordered_set<NfaNode<T> *> next;
                for (auto &c : current)
                {
                    const unordered_set<NfaNode<T> *> &beforeClosure = c->next(*b);
                    for (auto &x : beforeClosure)
                    {
                        unordered_set<NfaNode<T> *> afterClosure = x->epsilon_closure();
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
    };

    template <typename T, typename I>
    class Nfa<T, I> : public Nfa<T, I, false>
    {
    public:
        using Nfa<T, I, false>::Nfa;
    };

    template <typename T>
    class Nfa<T, int> : public Nfa<T, int, false>
    {
    public:
        Nfa()
        {
            auto s = make_unique<NfaNode<T>>();
            this->start = &*s;
            this->end = &*s;
            this->nodes[s->get_id()] = move(s);
        }

        Nfa(T accept)
        {
            auto s = make_unique<NfaNode<T>>();
            auto e = make_unique<NfaNode<T>>();
            this->start = &*s;
            this->end = &*e;
            this->nodes[s->get_id()] = move(s);
            this->nodes[e->get_id()] = move(e);

            this->start->add_connection(accept, this->end);
        }

        Nfa(const unordered_set<T> &acceptAny)
        {
            auto s = make_unique<NfaNode<T>>();
            auto e = make_unique<NfaNode<T>>();
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