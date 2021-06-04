
#ifndef LIBS_HEADER_BVH_H
#define LIBS_HEADER_BVH_H

#include "constants.h"
#include "vec3.h"
#include "ray.h"

#include <algorithm>
#include <vector>
#include <unordered_set>


namespace LNF
{
    // calculates bouds of nodes
    template <typename node_type>
    Bounds findBounds(const std::vector<const node_type*> &_nodes) {
        Bounds bounds = _nodes[0]->bounds();
                
        for (auto &pNode : _nodes) {
            const auto &nb = pNode->bounds();
            bounds.m_min = perElementMin(bounds.m_min, nb.m_min);
            bounds.m_max = perElementMax(bounds.m_max, nb.m_max);
        }
        
        return bounds;
    }


    // split nodes into 'inside' and 'outside' bounds
    template <typename node_type>
    void splitNodes(std::vector<const node_type*> &_nodesLeft, std::vector<const node_type*> &_nodesRight,
                    const std::vector<const node_type*> &_nodes,
                    const Bounds &_boundsLeft, const Bounds &_boundsRight)
    {
        for (auto &pNode : _nodes) {
            const auto &nb = pNode->bounds();
            
            if (aaboxIntersectCheck(nb, _boundsLeft) == true) {
                _nodesLeft.push_back(pNode);
            }

            if (aaboxIntersectCheck(nb, _boundsRight) == true) {
                _nodesRight.push_back(pNode);
            }
        }
    }


    /*
     Bounding volume hyrarchy nodes.
     */
    template <typename primitive_type>
    struct BvhNode
    {
        BvhNode() noexcept
        {}
        
        bool intersect(const Ray &_ray) const {
            return aaboxIntersectCheck(m_bounds, _ray.m_origin, _ray.m_invDirection);
        }
        
        bool empty() const {
            return m_primitives.empty();
        }
        
        Bounds                              m_bounds;
        std::unique_ptr<BvhNode>            m_left;
        std::unique_ptr<BvhNode>            m_right;
        std::vector<const primitive_type*>  m_primitives;       // primitives (leaf nodes)
    };


    /*
     Container to hold and manage unique instances of primitives.
     NOTE: using vector and linear search internally for best performance.
     */
    template <typename primitive_type>
    class PrimitiveSet
    {
     public:
        PrimitiveSet()
            :m_count(0)
        {}
        
        void clear() {
            m_count = 0;
        }
        
        bool has(const primitive_type *_pObj) {
            for (size_t i = 0; i < m_count; i++) {
                if (m_primeObjects[i] == _pObj) {
                    return true;
                }
            }
            
            return false;
        }
        
        void insert(const primitive_type *_pObj) {
            if (has(_pObj) == false) {
                if (m_count >= m_primeObjects.size()) {
                    m_primeObjects.resize(m_primeObjects.size() + 4);
                }
                
                m_primeObjects[m_count] = _pObj;
                m_count++;
            }
        }
        
        const primitive_type **begin() {
            return m_primeObjects.data();
        }
        
        const primitive_type **end() {
            return m_primeObjects.data() + m_count;
        }

     private:
        std::vector<const primitive_type*>           m_primeObjects;
        size_t                                       m_count;
    };


    /*
     Build BVH tree recursively
     */
    template <size_t BVH_MIN_NODE_SIZE, typename primitive_type>
    std::unique_ptr<BvhNode<primitive_type>> buildBvhNode(const std::vector<const primitive_type*> &_primitives,
                                                          const Bounds &_bounds,
                                                          int _iDepth)
    {
        // create node
        auto node = std::make_unique<BvhNode<primitive_type>>();
        node->m_bounds = _bounds;

        // split in left and right
        auto boxes = splitBox(_bounds);
        std::vector<const primitive_type*> left;
        std::vector<const primitive_type*> right;
        splitNodes(left, right, _primitives, boxes.first, boxes.second);
        
        // cannot split: just add to node
        if ( (left.size() == _primitives.size()) &&
             (right.size() == _primitives.size()) ) {
            node->m_primitives.insert(node->m_primitives.end(), _primitives.begin(), _primitives.end());
        }
        else {
            // left node: go down the tree
            if (left.size() <= _primitives.size()) {
                if ( (left.size() > BVH_MIN_NODE_SIZE) && (_iDepth > 0) ) {
                    node->m_left = buildBvhNode<BVH_MIN_NODE_SIZE>(left, boxes.first, _iDepth - 1);
                }
                else if (left.size() > 0) {
                    node->m_primitives.insert(node->m_primitives.end(), left.begin(), left.end());
                }
            }
            
            // right node: go down the tree
            if (right.size() <= _primitives.size()) {
                if ( (right.size() > BVH_MIN_NODE_SIZE) && (_iDepth > 0)  ) {
                    node->m_right = buildBvhNode<BVH_MIN_NODE_SIZE>(right, boxes.second, _iDepth - 1);
                }
                else if (right.size() > 0) {
                    node->m_primitives.insert(node->m_primitives.end(), right.begin(), right.end());
                }
            }
        }

        return node;
    }


    /*
     Build BVH tree root
     */
    template <size_t BVH_MIN_NODE_SIZE, typename primitive_type>
    std::unique_ptr<BvhNode<primitive_type>> buildBvhRoot(const std::vector<const primitive_type*> &_srcNodes,
                                                          const size_t _bvhMaxDepth)
    {
        Bounds bounds = findBounds(_srcNodes);
        return buildBvhNode<BVH_MIN_NODE_SIZE>(_srcNodes, bounds, (int)_bvhMaxDepth);
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_BVH_H

