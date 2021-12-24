
#pragma once

#include "constants.h"
#include "vec3.h"
#include "ray.h"
#include "queue.h"

#include <algorithm>
#include <vector>
#include <unordered_set>


namespace CORE
{
    /*
     Bounding volume hyrarchy nodes
     */
    template <typename primitive_type>
    struct BvhNode
    {
        MANAGE_MEMORY('BVHN');
        BvhNode() noexcept
        {}
        
        bool intersect(const Ray &_ray) const {
            return aaboxIntersectCheck(m_bounds, _ray.m_origin, _ray.m_invDirection);
        }
        
        bool empty() const {
            return m_primitives.empty();
        }

        std::vector<const primitive_type*>  m_primitives;       // primitives (leaf nodes)
        Bounds                              m_bounds;
        BvhNode                             *m_pLeft = nullptr;
        BvhNode                             *m_pRight = nullptr;
    };


    /*
     Build BVH tree recursively
     */
    template <size_t BVH_MIN_NODE_SIZE, typename primitive_type, typename create_func>
    BvhNode<primitive_type> *buildBvhNode(const std::vector<const primitive_type*> &_primitives,
                                          const Bounds &_splitBounds,
                                          int _iDepth,
                                          const create_func &_create)
    {
        // create node
        auto pNode = _create();

        // split in left and right
        auto boxes = splitBox(_splitBounds);
        std::vector<const primitive_type*> left;
        std::vector<const primitive_type*> right;
        splitItemsByBounds(left, right, _primitives, boxes.first, boxes.second, [](const primitive_type *_pItem){
            return _pItem->bounds();
        });

        std::vector<Bounds> boundsList;
        
        // left node: go down the tree
        if ( (left.size() > BVH_MIN_NODE_SIZE) && (_iDepth > 0) ) {
            pNode->m_pLeft = buildBvhNode<BVH_MIN_NODE_SIZE>(left, boxes.first, _iDepth - 1, _create);
            boundsList.emplace_back(pNode->m_pLeft->m_bounds);
        }
        else if (left.size() > 0) {
            pNode->m_primitives.insert(pNode->m_primitives.end(), left.begin(), left.end());
            boundsList.emplace_back(findBounds(pNode->m_primitives, [](const primitive_type *_pItem){
                return _pItem->bounds();
            }));
        }
        
        // right node: go down the tree
        if ( (right.size() > BVH_MIN_NODE_SIZE) && (_iDepth > 0)  ) {
            pNode->m_pRight = buildBvhNode<BVH_MIN_NODE_SIZE>(right, boxes.second, _iDepth - 1, _create);
            boundsList.emplace_back(pNode->m_pRight->m_bounds);
        }
        else if (right.size() > 0) {
            pNode->m_primitives.insert(pNode->m_primitives.end(), right.begin(), right.end());
            boundsList.emplace_back(findBounds(pNode->m_primitives, [](const primitive_type *_pItem){
                return _pItem->bounds();
            }));
        }

        // update with actual bounds of primitives on node
        pNode->m_bounds = combineBoxes(boundsList);
        return pNode;
    }


    /*
     Build BVH tree root
     */
    template <size_t BVH_MIN_NODE_SIZE, typename primitive_type, typename create_func>
    BvhNode<primitive_type> *buildBvhRoot(const std::vector<const primitive_type*> &_srcNodes,
                                          const size_t _bvhMaxDepth,
                                          const create_func &_create)
    {
        Bounds bounds = findBounds(_srcNodes, [](const primitive_type *_pItem){
            return _pItem->bounds();
        });

        return buildBvhNode<BVH_MIN_NODE_SIZE>(_srcNodes, bounds, (int)_bvhMaxDepth, _create);
    }


    /* Search for best hit through BVH */
    template <typename primitive_type, typename hit_func>
    void checkBvhHit(const BvhNode<primitive_type> *_pRoot, const Ray &_ray, const hit_func &_hit)
    {
        CORE::Stack<BvhNode<primitive_type>*> nodes(32);
        
        // start with root
        _hit(_pRoot, _ray);
        nodes.push(_pRoot->m_pLeft);
        nodes.push(_pRoot->m_pRight);

        // process nodes
        while (nodes.empty() == false) {
            const auto &pNode = nodes.pop();
            if ( (pNode != nullptr) &&
                 (pNode->intersect(_ray) == true) )
            {
                _hit(pNode, _ray);
                
                if (pNode->m_pLeft != nullptr){
                    nodes.push(pNode->m_pLeft);
                }
                    
                if (pNode->m_pRight != nullptr){
                    nodes.push(pNode->m_pRight);
                }
            }
        }
    }


};  // namespace CORE


