#ifndef DETAIL_SIMPLE_SCENE_H
#define DETAIL_SIMPLE_SCENE_H

#include "core/bvh.h"
#include "core/color.h"
#include "core/queue.h"
#include "base/intersect.h"
#include "base/primitive.h"
#include "base/resource.h"

#include <vector>
#include <memory>
#include <cassert>




namespace DETAIL
{

    // simple scene with a linear search for object hits
    class SimpleScene   : public BASE::Scene
    {
     public:
        SimpleScene(const CORE::Color &_background)
            :m_backgroundColor(_background)
        {}
            
        /*
           Checks for an intersect with a scene object.
           Could be accessed by multiple worker threads concurrently.
         */
        virtual bool hit(BASE::Intersect &_hit) const override {
            for (const auto &pObj : m_objects) {
                if (BASE::Intersect nh(_hit); (pObj->hit(nh) == true) && ( (_hit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
                {
                    _hit = nh;
                }
            }
            
            return _hit;
        }
        
        /*
         Checks for the background color (miss handler).
         Could be accessed by multiple worker threads concurrently.
         */
        virtual CORE::Color backgroundColor() const override {
            return m_backgroundColor;
        }

        /*
         Add a new resource (material, primitive, instance) to the scene.
         May not be safe to call while worker threads are calling 'hit'/
        */
        virtual BASE::Resource *addResource(std::unique_ptr<BASE::Resource> &&_pResource) override {
            m_resources.push_back(std::move(_pResource));
            return m_resources.back().get();
        }

        /*
         Add a new primitive instance to the scene.
         May not be safe to call while worker threads are calling 'hit'/
         */
        virtual BASE::PrimitiveInstance *addPrimitiveInstance(std::unique_ptr<BASE::PrimitiveInstance> &&_pInstance) override {
            m_objects.push_back(std::move(_pInstance));
            return m_objects.back().get();
        }
        
     protected:
        CORE::Color                                            m_backgroundColor;
        std::vector<std::unique_ptr<BASE::Resource>>           m_resources;
        std::vector<std::unique_ptr<BASE::PrimitiveInstance>>  m_objects;
    };


    // simple scene using a BVH for optimising hits
    class SimpleSceneBvh   : public SimpleScene
    {
     public:
        SimpleSceneBvh(const CORE::Color &_background)
            :SimpleScene(_background)
        {}
            
        // Checks for an intersect with a scene object (could be accessed by multiple worker threads concurrently).
        virtual bool hit(BASE::Intersect &_hit) const override {
            return checkBvhHitI(_hit);
            //return checkBvhHitR(_hit, m_root);
        }

        // Build acceleration structures
        void build() {
            std::vector<const BASE::PrimitiveInstance*> rawObjects(m_objects.size(), nullptr);
            for (size_t i = 0; i < m_objects.size(); i++) {
                rawObjects[i] = m_objects[i].get();
            }

            m_root = CORE::buildBvhRoot<2>(rawObjects, 16);
        }

     private:
        // Search for best hit through BVHs (iterative)
        bool checkBvhHitI(BASE::Intersect &_hit) const
        {
            thread_local static CORE::Stack<CORE::BvhNode<BASE::PrimitiveInstance>*> nodes;
            assert(m_root != nullptr);  // check that we built the BVH tree
            nodes.push(m_root.get());
                    
            while (nodes.empty() == false) {
                // get last node
                auto pNode = nodes.pop();
                
                // check node hits (finding closest hit)
                for (const auto &pObj : pNode->m_primitives) {
                    if (BASE::Intersect nh(_hit); (pObj->hit(nh) == true) && ( (_hit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
                    {
                        _hit = nh;
                    }
                }
                
                // go down tree
                if ( (pNode->m_right != nullptr) &&
                     (pNode->m_right->intersect(_hit.m_viewRay) == true) ) {
                    nodes.push(pNode->m_right.get());
                }

                if ( (pNode->m_left != nullptr) &&
                     (pNode->m_left->intersect(_hit.m_viewRay) == true) ) {
                    nodes.push(pNode->m_left.get());
                }
            }
            
            return _hit;
        }

        // Search for best hit through BVHs (recursive)
        bool checkBvhHitR(BASE::Intersect &_hit, const std::unique_ptr<CORE::BvhNode<BASE::PrimitiveInstance>> &_pNode) const
        {
            if (_pNode->empty() == false) {
                for (const auto &pObj : _pNode->m_primitives) {
                    if (BASE::Intersect nh(_hit); (pObj->hit(nh) == true) && ( (_hit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
                    {
                        _hit = nh;
                    }
                }
            }

            if ( (_pNode->m_left != nullptr) &&
                 (_pNode->m_left->intersect(_hit.m_viewRay) == true) ) {
                checkBvhHitR(_hit, _pNode->m_left);
            }
            
            if ( (_pNode->m_right != nullptr) &&
                 (_pNode->m_right->intersect(_hit.m_viewRay) == true) ) {
                checkBvhHitR(_hit, _pNode->m_right);
            }
            
            return _hit;
        }
        
     private:
        std::unique_ptr<CORE::BvhNode<BASE::PrimitiveInstance>>      m_root;
    };


};      // namespace DETAIL


#endif  // #ifndef DETAIL_SIMPLE_SCENE_H
