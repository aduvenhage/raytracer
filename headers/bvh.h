
#ifndef LIBS_HEADER_BVH_H
#define LIBS_HEADER_BVH_H

#include "constants.h"
#include "vec3.h"
#include "ray.h"

#include <algorithm>


namespace LNF
{
    // sorted insert (and unique) into list
    void sortedInsert(std::vector<Node*> &_nodes, Node *_pNode) {
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
    class BvhNode
    {
     public:
        BvhNode()
        {}
        
        BvhNode(const Bounds &_bounds)
            :m_bounds(_bounds)
        {}
        
        // check for possible bounding volume hyrarchy
        bool intersect(std::vector<Node*> &_hitNodes, const Ray &_ray) const {
            bool bHit = false;
            if (aaboxIntersectCheck(m_bounds, _ray.m_origin, _ray.m_invDirection) == true) {
            
                if (m_nodes.empty() == false) {
                    for (const auto &pNode : m_nodes) {
                        sortedInsert(_hitNodes, pNode.get());
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
        void build(const std::vector<std::shared_ptr<Node>> &_nodes) {
            if (_nodes.empty() == false) {
                // find bounding volume
                m_bounds = _nodes[0]->bounds();
                
                for (auto &pNode : _nodes) {
                    const auto &nb = pNode->bounds();
                    m_bounds.m_min = perElementMin(m_bounds.m_min, nb.m_min);
                    m_bounds.m_max = perElementMax(m_bounds.m_max, nb.m_max);
                }

                // build nodes recursively
                buildTree(_nodes);
            }
        }
        
      private:
        // build bounding volume hyrarchy
        void buildTree(const std::vector<std::shared_ptr<Node>> &_nodes) {
            // build left and right sets
            std::vector<std::shared_ptr<Node>> nodesLeft;
            std::vector<std::shared_ptr<Node>> nodesRight;
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
            
            if ( (nodesLeft.size() > 4) &&
                 (boxes.first.volume() > 100) )
            {
                m_left = std::make_unique<BvhNode>(boxes.first);
                m_left->buildTree(nodesLeft);
            }
            else {
                m_nodes.insert(m_nodes.begin(), nodesLeft.begin(), nodesLeft.end());
            }
            
            if ( (nodesRight.size() > 4) &&
                 (boxes.second.volume() > 100) )
            {
                m_right = std::make_unique<BvhNode>(boxes.second);
                m_right->buildTree(nodesRight);
            }
            else {
                m_nodes.insert(m_nodes.begin(), nodesRight.begin(), nodesRight.end());
            }
        }
        
     private:
        Bounds                              m_bounds;
        std::unique_ptr<BvhNode>            m_left;
        std::unique_ptr<BvhNode>            m_right;
        std::vector<std::shared_ptr<Node>>  m_nodes;        // leaf nodes
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_BVH_H

