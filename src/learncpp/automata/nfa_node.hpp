#ifndef NFA_NODE_HPP
#define NFA_NODE_HPP

#include <unordered_map>
#include <unordered_set>
#include <optional>

using namespace std;

namespace automata
{
    template <typename T, typename I>
    class NfaNode
    {
    private:
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
}

#endif
