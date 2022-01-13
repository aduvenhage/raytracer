
#pragma once

#include "core/constants.h"
#include "core/vec3.h"
#include "core/ray.h"
#include "core/queue.h"

#include <algorithm>
#include <vector>


namespace BASE
{
    /*
     Bounding volume hyrarchy nodes
     */
    template <typename primitive_type>
    struct BvhNode
    {
        MANAGE_MEMORY('BVHN');
        BvhNode() noexcept = default;
        
        float intersect(const CORE::Ray &_ray) const {
            if (auto i = aaboxIntersect(m_bounds, _ray); i.intersect() == true) {
                return i.m_tmin >= 0 ? i.m_tmin : 0;
            }
            
            return -1;
        }

        CORE::Bounds m_bounds;
        BvhNode *m_pLeft = nullptr;
        BvhNode *m_pRight = nullptr;
        const primitive_type *m_pPrimitive = nullptr;
    };

    
    /*
        Split primitive items into two bins (left & right)
     */
    template <typename primitive_type>
    std::pair<CORE::Bounds, CORE::Bounds> splitPrimitives(
                std::vector<const primitive_type*> &_left, std::vector<const primitive_type*> &_right,
                const std::vector<const primitive_type*> &_primitives,
                const CORE::Bounds &_splitBounds)
    {
        std::pair<CORE::Bounds, CORE::Bounds> boxes(_splitBounds, _splitBounds);
        if (_splitBounds.area() < 2) {
            CORE::splitItems(_left, _right, _primitives);
        }
        else {
            // split bounds and try to put items in them
            for (;;) {
                boxes = splitBox(_left.empty() == false ? boxes.first : boxes.second);
                CORE::splitItemsByBounds(_left, _right, _primitives, boxes.first, boxes.second, [](const primitive_type *_pItem){
                    return _pItem->bounds();
                });

                if ( (_left.size() != 0) && (_right.size() != 0) )
                {
                    break; // successfull split
                }
                
                if (boxes.first.area() < 2) {
                    CORE::splitItems(_left, _right, _primitives);
                    break;
                }
            }
        }
        
        return boxes;
    }


    /*
     Build BVH tree recursively
     */
    template <typename primitive_type, typename create_func>
    BvhNode<primitive_type> *buildBvhNode(const std::vector<const primitive_type*> &_primitives,
                                          const CORE::Bounds &_splitBounds,
                                          const create_func &_create)
    {
        // create node
        auto pNode = _create();
        if (_primitives.size() == 1) {
            pNode->m_pPrimitive = _primitives[0];
            pNode->m_bounds = pNode->m_pPrimitive->bounds();
        }
        else {
            std::vector<CORE::Bounds> boundsList;
            std::vector<const primitive_type*> left;
            std::vector<const primitive_type*> right;

            // split in left and right
            auto boxes = splitPrimitives(left, right, _primitives, _splitBounds);

            // left node: go down the tree
            if (left.size() > 0) {
                pNode->m_pLeft = buildBvhNode(left, boxes.first, _create);
                boundsList.push_back(pNode->m_pLeft->m_bounds);
            }
            
            // right node: go down the tree
            if (right.size() > 0) {
                pNode->m_pRight = buildBvhNode(right, boxes.second, _create);
                boundsList.push_back(pNode->m_pRight->m_bounds);
            }

            // update with actual bounds of primitives of node
            pNode->m_bounds = combineBoxes(boundsList);
        }

        return pNode;
    }


    /*
     Build BVH tree root
     */
    template <typename primitive_type, typename create_func>
    BvhNode<primitive_type> *buildBvhRoot(const std::vector<const primitive_type*> &_srcNodes,
                                          const create_func &_create)
    {
        CORE::Bounds bounds = CORE::findBounds(_srcNodes, [](const primitive_type *_pItem){
            return _pItem->bounds();
        });

        return buildBvhNode(_srcNodes, bounds, _create);
    }


    /* Search for best hit through BVH */
    template <typename primitive_type, typename hit_func>
    uint32_t checkBvhHit(const BvhNode<primitive_type> *_pRoot, const CORE::Ray &_ray, const hit_func &_hit)
    {
        CORE::Stack<BvhNode<primitive_type>*> nodes(32);
        uint32_t boxHits = 0;
        
        // start with root
        nodes.push(_pRoot->m_pLeft);
        nodes.push(_pRoot->m_pRight);

        // process nodes
        while (nodes.empty() == false) {
            const auto &pNode = nodes.pop();
            if ( (pNode != nullptr) &&
                 (pNode->intersect(_ray) >= 0) )
            {
                boxHits++;
                
                if (pNode->m_pPrimitive != nullptr) {
                    _hit(pNode->m_pPrimitive, _ray);
                }
                else {
                    if (pNode->m_pLeft != nullptr){
                        nodes.push(pNode->m_pLeft);
                    }
                        
                    if (pNode->m_pRight != nullptr){
                        nodes.push(pNode->m_pRight);
                    }
                }
            }
        }
        
        return boxHits;
    }


};  // namespace CORE


