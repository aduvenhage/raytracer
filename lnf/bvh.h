
#ifndef LIBS_HEADER_BVH_H
#define LIBS_HEADER_BVH_H

#include "constants.h"
#include "vec3.h"
#include "ray.h"

#include <algorithm>


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


    // sorted insert (and unique) into list
    template <typename node_type>
    void sortedInsert(std::vector<const node_type*> &_nodes, const node_type *_pNode) {
        auto it = std::lower_bound(_nodes.begin(), _nodes.end(), _pNode);
        if ( (it == _nodes.end()) ||
             (*it != _pNode) )
        {
            _nodes.insert(it, _pNode);
        }
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
        BvhNode()
        {}
        
        Bounds                              m_bounds;
        std::unique_ptr<BvhNode>            m_left;
        std::unique_ptr<BvhNode>            m_right;
        std::vector<const primitive_type*>  m_primitives;       // primitives (leaf nodes)
    };


    // create bounding volume tree
    template <typename primitive_type>
    class BvhTree
    {
     public:
        using node_ptr_type = std::unique_ptr<BvhNode<primitive_type>>;
        
     protected:
        static const int    MAX_DEPTH       = 16;
        static const int    MIN_LEAF_SIZE   = 1;
        
     public:
        // build BVH tree
        static node_ptr_type build(const std::vector<const primitive_type*> &_nodes) {
            Bounds bounds = findBounds(_nodes);
            return _buildBvhNode(_nodes, bounds, 0);
        }

        // find list of hittable nodes
        static bool intersect(std::vector<const primitive_type*> &_hitPrimitives, const node_ptr_type &_root, const Ray &_ray) {
            bool bHit = false;
            if (_root->m_primitives.empty() == false) {
                for (const auto &pObj : _root->m_primitives) {
                    sortedInsert(_hitPrimitives, pObj);
                }

                bHit = true;
            }

            if (_root->m_left != nullptr) {
                if (aaboxIntersectCheck(_root->m_left->m_bounds, _ray.m_origin, _ray.m_invDirection) == true) {
                    bHit |= intersect(_hitPrimitives, _root->m_left, _ray);
                }
            }
            
            if (_root->m_right != nullptr) {
                if (aaboxIntersectCheck(_root->m_right->m_bounds, _ray.m_origin, _ray.m_invDirection) == true) {
                    bHit |= intersect(_hitPrimitives, _root->m_right, _ray);
                }
            }
            
            return bHit;
        }
        
     private:
        static node_ptr_type _buildBvhNode(const std::vector<const primitive_type*> &_primitives, const Bounds &_bounds, int _iDepth) {
            // create node
            auto node = std::make_unique<BvhNode<primitive_type>>();
            node->m_bounds = _bounds;

            // split in left, right and outside
            auto boxes = splitBox(_bounds);
            std::vector<const primitive_type*> left;
            std::vector<const primitive_type*> right;
            splitNodes(left, right, _primitives, boxes.first, boxes.second);

            // left nodes go down the tree
            if ( (left.size() > MIN_LEAF_SIZE) && (_iDepth < MAX_DEPTH) ) {
                node->m_left = _buildBvhNode(left, boxes.first, _iDepth + 1);
            }
            else if (left.size() > 0) {
                node->m_primitives.insert(node->m_primitives.end(), left.begin(), left.end());
            }
            
            // right nodes go down the tree
            if ( (right.size() > MIN_LEAF_SIZE) && (_iDepth < MAX_DEPTH)  ) {
                node->m_right = _buildBvhNode(right, boxes.second, _iDepth + 1);
            }
            else if (right.size() > 0) {
                node->m_primitives.insert(node->m_primitives.end(), right.begin(), right.end());
            }

            return node;
        }
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_BVH_H

