
#ifndef LIBS_HEADER_BVH_H
#define LIBS_HEADER_BVH_H

#include "constants.h"
#include "vec3.h"
#include "ray.h"

#include <algorithm>


namespace LNF
{
    // sorted insert (and unique) into list
    template <typename node_type>
    void sortedInsert(std::vector<node_type*> &_nodes, node_type *_pNode) {
        auto it = std::lower_bound(_nodes.begin(), _nodes.end(), _pNode);
        if ( (it == _nodes.end()) ||
             (*it != _pNode) )
        {
            _nodes.insert(it, _pNode);
        }
    }
    
    /*
        Bounding volume hyrarchy node.
     */
    template <typename node_type>
    class BvhNode
    {
     public:
        BvhNode()
        {}
        
        BvhNode(const Bounds &_bounds)
            :m_bounds(_bounds)
        {}
        
        // search for hits in bounding volume hyrarchy
        bool intersect(std::vector<node_type*> &_hitNodes, const Ray &_ray) const {
            bool bHit = false;
            if (aaboxIntersectCheck(m_bounds, _ray.m_origin, _ray.m_invDirection) == true) {
            
                if (m_nodes.empty() == false) {
                    for (const auto &pNode : m_nodes) {
                        sortedInsert(_hitNodes, pNode);
                    }

                    bHit = true;
                }
                
                if (m_left != nullptr) {
                    bHit |= m_left->intersect(_hitNodes, _ray);
                }
                
                if (m_right != nullptr) {
                    bHit |= m_right->intersect(_hitNodes, _ray);
                }
            }

            return bHit;
        }
        
        // build bounding volume hyrarchy
        void build(const std::vector<node_type*> &_nodes) {
            if (_nodes.empty() == false) {
                // find bounding volume
                m_bounds = _nodes[0]->bounds();
                
                for (auto &pNode : _nodes) {
                    const auto &nb = pNode->bounds();
                    m_bounds.m_min = perElementMin(m_bounds.m_min, nb.m_min);
                    m_bounds.m_max = perElementMax(m_bounds.m_max, nb.m_max);
                }

                // build nodes recursively
                m_iLevels = buildTree(_nodes, 0);
            }
        }
        
      private:
        // build bounding volume hyrarchy
        int buildTree(const std::vector<node_type*> &_nodes, int _iLevel) {
            // build left and right sets
            std::vector<node_type*> nodesLeft;
            std::vector<node_type*> nodesRight;
            auto boxes = splitBox(m_bounds);
            
            for (auto &pNode : _nodes) {
                const auto &nb = pNode->bounds();
                bool bAllocated = false;
                
                if (aaboxIntersectCheck(nb, boxes.first) == true) {
                    nodesLeft.push_back(pNode);
                    bAllocated = true;
                }
                
                if (aaboxIntersectCheck(nb, boxes.second) == true) {
                    nodesRight.push_back(pNode);
                    bAllocated = true;
                }
                
                if (bAllocated == false) {
                    m_nodes.push_back(pNode);
                }
            }
            
            int levelsLeft = _iLevel;
            if (nodesLeft.empty() == false) {
                if (nodesLeft.size() < _nodes.size())
                {
                    m_left = std::make_unique<BvhNode>(boxes.first);
                    levelsLeft = m_left->buildTree(nodesLeft, _iLevel + 1);
                }
                else {
                    m_nodes.insert(m_nodes.begin(), nodesLeft.begin(), nodesLeft.end());
                }
            }
            
            int levelsRight = _iLevel;
            if (nodesRight.empty() == false) {
                if (nodesRight.size() < _nodes.size())
                {
                    m_right = std::make_unique<BvhNode>(boxes.second);
                    levelsRight = m_right->buildTree(nodesRight, _iLevel + 1);
                }
                else {
                    m_nodes.insert(m_nodes.begin(), nodesRight.begin(), nodesRight.end());
                }
            }
            
            return std::max(levelsLeft, levelsRight);
        }
        
     private:
        Bounds                                   m_bounds;
        std::unique_ptr<BvhNode>                 m_left;
        std::unique_ptr<BvhNode>                 m_right;
        std::vector<node_type*>                  m_nodes;        // leaf nodes
        int                                      m_iLevels;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_BVH_H

