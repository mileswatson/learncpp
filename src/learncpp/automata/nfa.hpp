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
    template <typename T>
    class NfaNode
    {
    private:
        static inline int nextId = 0;
        int id = nextId++;
        unordered_map<optional<T>, unordered_set<NfaNode<T> *>> connections;
        NfaNode(const NfaNode<T> &) = delete;
        operator=(const NfaNode<T> &) = delete;
        NfaNode(NfaNode<T> &&) = delete;
        operator=(NfaNode<T> &&) = delete;

    private:
        void visit_epsilon_closure(unordered_set<NfaNode<T> *> &visited)
        {
            if (!visited.emplace(this).second)
                return;
            const unordered_set<NfaNode<T> *> &toVisit = connections[{}];
            for (auto &v : toVisit)
            {
                v->visit_epsilon_closure(visited);
            }
        }

    public:
        NfaNode() {}
        int get_id()
        {
            return id;
        }
        void add_connection(optional<T> input, NfaNode<T> *destination)
        {
            unordered_set<NfaNode<T> *> &s = connections[input];
            s.emplace(destination);
        }

        const unordered_set<NfaNode<T> *> &next(optional<T> input)
        {
            return connections[input];
        }

        unordered_set<NfaNode<T> *> epsilon_closure()
        {
            unordered_set<NfaNode<T> *> visited;
            visit_epsilon_closure(visited);
            return visited;
        }
    };

    template <typename T>
    class Nfa
    {
    private:
        NfaNode<T> *start;
        NfaNode<T> *end;
        unordered_map<int, unique_ptr<NfaNode<T>>> nodes;

        Nfa(NfaNode<T> *start, NfaNode<T> *end, unordered_map<int, unique_ptr<NfaNode<T>>> &&nodes) : start(start), end(end), nodes(move(nodes))
        {
        }

        Nfa(const Nfa<T> &) = delete;
        operator=(const Nfa<T> &) = delete;

    public:
        Nfa(T accept)
        {
            auto s = make_unique<NfaNode<T>>();
            auto e = make_unique<NfaNode<T>>();
            start = &*s;
            end = &*e;
            s->add_connection(accept, &*e);
            nodes[s->get_id()] = move(s);
            nodes[e->get_id()] = move(e);
        }

        static Nfa<T> concat(Nfa<T> &&first, Nfa<T> &&second)
        {
            for (auto &[k, v] : second.nodes)
            {
                first.nodes.emplace(k, move(v));
            }
            first.end->add_connection({}, second.start);
            return Nfa<T>(first.start, second.end, move(first.nodes));
        }

        static Nfa<T> either(Nfa<T> &&first, Nfa<T> &&second)
        {
            for (auto &[k, v] : second.nodes)
            {
                first.nodes.emplace(k, move(v));
            }
            first.start->add_connection({}, second.start);
            second.end->add_connection({}, first.end);
            return Nfa<T>(first.start, first.end, move(first.nodes));
        }

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
                    lastMatch = optional(b);
            }
            return lastMatch;
        }
    };
}

#endif