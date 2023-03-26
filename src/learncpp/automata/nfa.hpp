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
        NfaNode(NfaNode &&) = delete;
        operator=(NfaNode<T> &&) = delete;

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
            s->add_connection(accept, &*e);
            nodes[s->get_id()] = move(s);
            nodes[e->get_id()] = move(e);
        }

        Nfa(Nfa<T> &&) = default;
        Nfa &operator=(Nfa<T> &&) = default;

        static Nfa<T> and_then(Nfa<T> &&first, Nfa<T> &&second)
        {
            first.nodes.merge(second.nodes);
            first.end->add_connection({}, second.start);
            return Nfa<T>(first.start, second.end, move(first.nodes));
        }
    };
}

#endif