
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
     Build BVH tree recursively
     */
    template <typename primitive_type>
    std::unique_ptr<BvhNode<primitive_type>> buildBvhNode(const std::vector<const primitive_type*> &_primitives, const Bounds &_bounds, int _iDepth) {
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
                if ( (left.size() > 2) && (_iDepth > 0) ) {
                    node->m_left = buildBvhNode(left, boxes.first, _iDepth - 1);
                }
                else if (left.size() > 0) {
                    node->m_primitives.insert(node->m_primitives.end(), left.begin(), left.end());
                }
            }
            
            // right node: go down the tree
            if (right.size() <= _primitives.size()) {
                if ( (right.size() > 2) && (_iDepth > 0)  ) {
                    node->m_right = buildBvhNode(right, boxes.second, _iDepth - 1);
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
    template <typename primitive_type>
    std::unique_ptr<BvhNode<primitive_type>> buildBvhRoot(const std::vector<const primitive_type*> &_srcNodes) {
        Bounds bounds = findBounds(_srcNodes);
        return buildBvhNode(_srcNodes, bounds, 32);
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_BVH_H

