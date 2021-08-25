
#ifndef CORE_BVH_H
#define CORE_BVH_H

#include "constants.h"
#include "vec3.h"
#include "ray.h"

#include <algorithm>
#include <vector>
#include <unordered_set>


namespace CORE
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


    // split nodes into 'left' or 'right' groups (using left as default if a node intersects with both)
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
            else if (aaboxIntersectCheck(nb, _boundsRight) == true) {
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
        MANAGE_MEMORY(MEM_POOL::SCENE)

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
    template <size_t BVH_MIN_NODE_SIZE, typename primitive_type>
    std::unique_ptr<BvhNode<primitive_type>> buildBvhNode(const std::vector<const primitive_type*> &_primitives,
                                                          const Bounds &_splitBounds,
                                                          int _iDepth)
    {
        // create node
        auto node = std::make_unique<BvhNode<primitive_type>>();

        // split in left and right
        auto boxes = splitBox(_splitBounds);
        std::vector<const primitive_type*> left;
        std::vector<const primitive_type*> right;
        splitNodes(left, right, _primitives, boxes.first, boxes.second);
        std::vector<Bounds> boundsList;
        
        // left node: go down the tree
        if ( (left.size() > BVH_MIN_NODE_SIZE) && (_iDepth > 0) ) {
            node->m_left = buildBvhNode<BVH_MIN_NODE_SIZE>(left, boxes.first, _iDepth - 1);
            boundsList.emplace_back(node->m_left->m_bounds);
        }
        else if (left.size() > 0) {
            node->m_primitives.insert(node->m_primitives.end(), left.begin(), left.end());
            boundsList.emplace_back(findBounds(node->m_primitives));
        }
        
        // right node: go down the tree
        if ( (right.size() > BVH_MIN_NODE_SIZE) && (_iDepth > 0)  ) {
            node->m_right = buildBvhNode<BVH_MIN_NODE_SIZE>(right, boxes.second, _iDepth - 1);
            boundsList.emplace_back(node->m_right->m_bounds);
        }
        else if (right.size() > 0) {
            node->m_primitives.insert(node->m_primitives.end(), right.begin(), right.end());
            boundsList.emplace_back(findBounds(node->m_primitives));
        }

        node->m_bounds = combineBoxes(boundsList);
        
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

};  // namespace CORE

#endif  // #ifndef CORE_BVH_H

