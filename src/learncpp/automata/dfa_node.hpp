#ifndef DFA_NODE_HPP
#define DFA_NODE_HPP

#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <stdexcept>

using namespace std;

namespace automata
{
    template <typename T, typename I>
    class DfaNode
    {
    private:
        unordered_set<I> nfaStates;
        unordered_map<T, const DfaNode<T, I> *> connections;

        DfaNode(const DfaNode<T, I> &) = delete;
        DfaNode<T, I> &operator=(const DfaNode<T, I> &) = delete;
        DfaNode(DfaNode<T, I> &&) = delete;
        DfaNode<T, I> &operator=(DfaNode<T, I> &&) = delete;

    public:
        DfaNode(unordered_set<I> &&nfaStates) : nfaStates(move(nfaStates))
        {
        }

        const unordered_set<I> &get_nfa_states() const
        {
            return nfaStates;
        }

        void add_connection(const T &input, const DfaNode<T, I> *destination)
        {
            const auto &[_, inserted] = connections.try_emplace(input, destination);
            if (!inserted)
                throw runtime_error("Edge already exists!");
        }

        bool contains(const I &nfaState) const
        {
            return nfaStates.contains(nfaState);
        }

        optional<const DfaNode<T, I> *> next(const T &input) const
        {
            auto found = connections.find(input);
            if (found != connections.end())
                return found->second;
            else
                return {};
        }
    };
}

#endif
