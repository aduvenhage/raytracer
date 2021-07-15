#ifndef LIBS_HEADER_LOADER_H
#define LIBS_HEADER_LOADER_H

#include "lnf/scene.h"
#include "lnf/camera.h"

#include <memory>


namespace LNF
{
    // Factory responsible for creating the correct scene and camera
    class Loader
    {
     public:
        virtual ~Loader() = default;
        virtual std::unique_ptr<Scene> loadScene() const = 0;
        virtual std::unique_ptr<Camera> loadCamera() const = 0;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_LOADER_H
