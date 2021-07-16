#ifndef BASE_LOADER_H
#define BASE_LOADER_H

#include "scene.h"
#include "camera.h"

#include <memory>


namespace BASE
{
    // Factory responsible for creating the correct scene and camera
    class Loader
    {
     public:
        virtual ~Loader() = default;
        virtual std::unique_ptr<Scene> loadScene() const = 0;
        virtual std::unique_ptr<Camera> loadCamera() const = 0;
    };

};  // namespace BASE


#endif  // #ifndef BASE_LOADER_H
