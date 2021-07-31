#include "core/viewport.h"
#include "base/camera.h"
#include "base/scene.h"
#include "detail/example_scenes.h"
#include "systems/frame.h"

#include <chrono>
#include <memory>
#include <thread>
#include <algorithm>
#include <iostream>
#include <string>


using namespace CORE;
using namespace BASE;
using namespace DETAIL;
using namespace SYSTEMS;


const int width = 1024;
const int height = 768;
const int numWorkers = std::max(std::thread::hardware_concurrency() * 2, 2u);
const int maxSamplesPerPixel = 64;
const int maxTraceDepth = 64;
const float colorTollerance = 0.0f;
const uint32_t randSeed = 1;

using clock_type = std::chrono::high_resolution_clock;


int runFrame(const std::shared_ptr<Loader> &_pLoader)
{
    auto pViewport = std::make_unique<Viewport>(width, height);
    auto pCamera = _pLoader->loadCamera();
    auto pScene = _pLoader->loadScene();
    auto tpInit = clock_type::now();
    auto pSource = std::make_unique<Frame>(pViewport.get(),
                                           pCamera.get(),
                                           pScene.get(),
                                           numWorkers,
                                           maxSamplesPerPixel,
                                           maxTraceDepth,
                                           colorTollerance,
                                           randSeed);

    printf("Starting with scene ...");
    while (pSource->isFinished() == false) {
        if (pSource->updateFrameProgress() == true) {
            printf("update: active jobs=%d, progress=%.2f, time_to_finish=%.2fs, total_time=%.2fs, rays_ps=%.2f\n",
                    (int)pSource->activeJobs(), pSource->progress(), pSource->timeToFinish(), pSource->timeTotal(), pSource->raysPerSecond());
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    pSource->writeJpegFile("raytraced.jpeg", 100);
    
    auto td = clock_type::now() - tpInit;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();
    printf("Done %.2fs\n", (float)ns/1e09);
    
    return 0;
}


int main(int argc, char *argv[])
{
    auto pLoader = std::make_shared<LoaderScene2>();
    return runFrame(pLoader);
}
