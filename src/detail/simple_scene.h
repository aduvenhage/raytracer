#pragma once

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
            return checkBvhHit(_hit);
        }

        // Build acceleration structures
        void build() {
            std::vector<const BASE::PrimitiveInstance*> rawObjects(m_objects.size(), nullptr);
            for (size_t i = 0; i < m_objects.size(); i++) {
                rawObjects[i] = m_objects[i].get();
            }

            m_pBvhRoot = CORE::buildBvhRoot<2>(rawObjects, 32,
                                               [](){
                                                return new CORE::BvhNode<BASE::PrimitiveInstance>();
                                               });
        }

     private:
        // Search for best hit through BVHs (iterative)
        bool checkBvhHit(BASE::Intersect &_hit) const
        {
            CORE::checkBvhHit(m_pBvhRoot, _hit.m_viewRay,
                              [&](const CORE::BvhNode<BASE::PrimitiveInstance> *_pNode, const CORE::Ray _ray){
                                if (_pNode->empty() == false) {
                                    for (const auto &pObj : _pNode->m_primitives) {
                                        if (BASE::Intersect nh(_hit); (pObj->hit(nh) == true) && ( (_hit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
                                        {
                                            _hit = nh;
                                        }
                                    }
                                }
                              });

            return _hit;
        }

     private:
        CORE::BvhNode<BASE::PrimitiveInstance>      *m_pBvhRoot;
    };


};      // namespace DETAIL

