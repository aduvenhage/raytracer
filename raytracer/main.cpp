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

#include <QtWidgets>



using namespace CORE;
using namespace BASE;
using namespace DETAIL;
using namespace SYSTEMS;


class MainWindow : public QMainWindow
{
 protected:
    using clock_type = std::chrono::high_resolution_clock;
    
 public:
    MainWindow(const std::unique_ptr<Loader> &&_pLoader)
        :QMainWindow(),
         m_iFrameCount(0),
         m_bFrameDone(false),
         m_iWidth(1024),
         m_iHeight(768),
         m_iNumWorkers(std::max(std::thread::hardware_concurrency() * 2, 2u)),
         m_iMaxSamplesPerPixel(1000),
         m_iMaxTraceDepth(32),
         m_fColorTollerance(0.0f),
         m_uRandSeed(1)
    {
        resize(m_iWidth, m_iHeight);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(200, Qt::PreciseTimer);
        
        m_pCamera = _pLoader->loadCamera();
        m_pScene = _pLoader->loadScene();
    }
    
 protected:
    virtual void paintEvent(QPaintEvent *_event) {
        QPainter painter(this);
        QImage image(m_iWidth, m_iHeight, QImage::Format_RGB888);

        if (m_pSource != nullptr) {
            std::memcpy(image.bits(),
                        m_pSource->image().data(),
                        m_pSource->image().size());
        }
        else {
            image.fill(Qt::black);
        }
        
        painter.drawImage(0, 0, image);
        m_iFrameCount++;
    }
    
    virtual void timerEvent(QTimerEvent *_event) {
        if (m_pSource == nullptr)
        {
            m_tpInit = clock_type::now();
            m_pSource = std::make_unique<Frame>(m_iWidth, m_iHeight,
                                                m_pCamera.get(),
                                                m_pScene.get(),
                                                m_iNumWorkers,
                                                m_iMaxSamplesPerPixel,
                                                m_iMaxTraceDepth,
                                                m_fColorTollerance,
                                                m_uRandSeed);
        }
        else {
            if (m_pSource->updateFrameProgress() == true) {
                printf("active jobs=%d, progress=%.2f, time_to_finish=%.2fs, total_time=%.2fs, rays_ps=%.2f\n",
                        (int)m_pSource->activeJobs(), m_pSource->progress(), m_pSource->timeToFinish(), m_pSource->timeTotal(), m_pSource->raysPerSecond());
            }

            if (m_pSource->isFinished() == true) {
                if (m_bFrameDone == false) {
                    m_pSource->writeJpegFile("raytraced.jpeg", 100);
                    m_bFrameDone = true;
                    
                    auto td = clock_type::now() - m_tpInit;
                    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();
                    
                    std::string title = std::string("Done ") + std::to_string((float)ns/1e09) + "s";
                    setWindowTitle(QString::fromStdString(title));
                }
            }
        }
        
        this->update(this->rect());
    }
    
 private:
    std::unique_ptr<Scene>              m_pScene;
    std::unique_ptr<Camera>             m_pCamera;
    std::unique_ptr<Frame>              m_pSource;
    int                                 m_iFrameCount;
    bool                                m_bFrameDone;
    clock_type::time_point              m_tpInit;
    int                                 m_iWidth;
    int                                 m_iHeight;
    int                                 m_iNumWorkers;
    int                                 m_iMaxSamplesPerPixel;
    int                                 m_iMaxTraceDepth;
    float                               m_fColorTollerance;
    uint32_t                            m_uRandSeed;
};


int main(int argc, char *argv[])
{
    auto pLoader = std::make_unique<LoaderFractalBox>();
    
    // start app
    QApplication app(argc, argv);
    MainWindow window(std::move(pLoader));
    window.show();

    return app.exec();
}
