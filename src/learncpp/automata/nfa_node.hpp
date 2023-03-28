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
        unordered_map<optional<T>, unordered_set<const NfaNode<T, I> *>> connections;

        NfaNode(const NfaNode<T, I> &) = delete;
        NfaNode<T, I> &operator=(const NfaNode<T, I> &) = delete;
        NfaNode(NfaNode<T, I> &&) = delete;
        NfaNode<T, I> &operator=(NfaNode<T, I> &&) = delete;

    public:
        NfaNode(I id) : id(id)
        {
        }

        const I &get_id() const
        {
            return id;
        }

        void add_connection(optional<T> input, const NfaNode<T, I> *destination)
        {
            unordered_set<const NfaNode<T, I> *> &s = connections[input];
            s.emplace(destination);
        }

        optional<reference_wrapper<const unordered_set<const NfaNode<T, I> *>>> next(optional<T> input) const
        {
            const auto found = connections.find(input);
            if (found != connections.end())
                return found->second;
            return {};
        }

        unordered_set<T> valid_inputs() const
        {
            unordered_set<T> inputs;
            for (auto &[k, _] : connections)
            {
                if (k)
                    inputs.emplace(*k);
            }
            return inputs;
        }

        void visit_epsilon_closure(unordered_set<const NfaNode<T, I> *> &visited) const
        {
            if (!visited.emplace(this).second)
                return;
            auto found = next({});
            if (!found)
                return;
            const unordered_set<const NfaNode<T, I> *> &toVisit = *found;
            for (auto &v : toVisit)
            {
                v->visit_epsilon_closure(visited);
            }
        };

        unordered_set<const NfaNode<T, I> *> epsilon_closure() const
        {
            unordered_set<const NfaNode<T, I> *> visited;
            visit_epsilon_closure(visited);
            return visited;
        }
    };

    template <typename T, typename I>
    unordered_set<const NfaNode<T, I> *> epsilon_closure(const unordered_set<const NfaNode<T, I> *> &nodes)
    {
        unordered_set<const NfaNode<T, I> *> closure;
        for (auto &n : nodes)
        {
            closure.merge(n->epsilon_closure());
        }
        return closure;
    }
}

#endif
