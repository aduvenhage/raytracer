#pragma once

#include "scene.h"
#include "camera.h"

#include <memory>
#include <string>


namespace BASE
{
    // Factory responsible for creating the correct scene and camera
    class Loader
    {
     public:
        virtual ~Loader() = default;
        virtual std::string &name() const = 0;
        virtual std::string &description() const = 0;
        virtual std::unique_ptr<Scene> loadScene() const = 0;
        virtual std::unique_ptr<Camera> loadCamera() const = 0;
    };

};  // namespace BASE

