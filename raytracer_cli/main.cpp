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
#include <map>


using namespace CORE;
using namespace BASE;
using namespace DETAIL;
using namespace SYSTEMS;


/*
 TODO: include these fields on the scene loader:
 - width
 - height
 - max samples per pixel
 - max trace depth
 - rand seed
*/

const int width = 1600;
const int height = 1200;
const int numWorkers = std::max(std::thread::hardware_concurrency() * 2, 2u);
const int maxSamplesPerPixel = 64;
const int maxTraceDepth = 64;
const float colorTollerance = 0.0f;
const uint32_t randSeed = 1;

using clock_type = std::chrono::high_resolution_clock;


int runFrame(const std::shared_ptr<Loader> &_pLoader, const std::string &_strOutputPath)
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

    printf("Starting with scene ...\n");
    while (pSource->isFinished() == false) {
        if (pSource->updateFrameProgress() == true) {
            printf("update: active jobs=%d, progress=%.2f, time_to_finish=%.2fs, total_time=%.2fs, rays_ps=%.2f\n",
                    (int)pSource->activeJobs(), pSource->progress(), pSource->timeToFinish(), pSource->timeTotal(), pSource->raysPerSecond());
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    pSource->writeJpegFile(_strOutputPath, 100);
    
    auto td = clock_type::now() - tpInit;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();
    printf("Done %.2fs\n", (float)ns/1e09);
    
    return 0;
}


std::shared_ptr<Loader> findScenarioLoader(const std::string &_strLoaderName) {
    static std::map<std::string, std::shared_ptr<Loader>> loaders = {
        {"scene0", std::make_shared<LoaderScene0>()},
        {"scene1", std::make_shared<LoaderScene1>()},
        {"scene2", std::make_shared<LoaderScene2>()},
        {"scene3", std::make_shared<LoaderScene3>()}
    };

    auto it = loaders.find(_strLoaderName);
    if (it != loaders.end()) {
        printf("Scene %s loaded.\n", _strLoaderName.c_str());
        return it->second;
    }
    else {
        return nullptr;
    }
}


int main(int argc, char *argv[])
{
    // parse command line
    std::string scenario;
    std::string output;
    
    if (argc == 3) {
        scenario = argv[1];
        output = argv[2];
    }
    else if (argc == 2){
        scenario = argv[1];
        output = "raytraced.jpeg";
    }
    
    printf("Running frame '%s' saving to '%s'\n", scenario.c_str(), output.c_str());
    
    // load and run frame
    auto pLoader = findScenarioLoader(scenario);
    if (pLoader != nullptr) {
        return runFrame(pLoader, output);
    }
    else {
        printf("Could not run frame: No scenario loader found!\n");
        return 1;
    }
}
