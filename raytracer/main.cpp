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
		m_iNumWorkers(std::thread::hardware_concurrency() / 2),
		m_iMaxSamplesPerPixel(16),
		m_iMaxTraceDepth(32),
		m_uRandSeed(1)
	{
        resize(m_iWidth, m_iHeight);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(20, Qt::CoarseTimer);
        
        m_pCamera = _pLoader->loadCamera();
        m_pScene = _pLoader->loadScene();
    }
    
 protected:
    virtual void paintEvent(QPaintEvent *) {
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
    
    virtual void timerEvent(QTimerEvent *) {
		auto tp = clock_type::now();

        if (m_pSource == nullptr)
        {
            m_tpInit = tp;
            m_pSource = std::make_unique<Frame>(m_iWidth, m_iHeight,
                                                m_pCamera.get(),
                                                m_pScene.get(),
                                                m_iNumWorkers,
                                                m_iMaxSamplesPerPixel,
                                                m_iMaxTraceDepth,
                                                m_uRandSeed);
        }
        else if (tp - m_tpLastFrame > std::chrono::milliseconds(200)) {
				m_pSource->updateFrameProgress();
				printf("active jobs=%d, progress=%.2f, time_to_finish=%.2fs, total_time=%.2fs, rays_ps=%.2f\n",
					   (int)m_pSource->activeJobs(), m_pSource->progress(), m_pSource->timeToFinish(), m_pSource->timeTotal(), m_pSource->raysPerSecond());
				this->update(this->rect());
				m_tpLastFrame = tp;
		}
        
		if (m_pSource->isFinished() == true) {
			if (m_bFrameDone == false) {
				m_pSource->writeToFile("raytraced.jpeg");
				m_bFrameDone = true;

				auto td = tp - m_tpInit;
				auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();

				std::string title = std::string("Done ") + std::to_string((float)ns / 1e09) + "s";
				setWindowTitle(QString::fromStdString(title));
			}
		}
    }
    
 private:
    std::unique_ptr<Scene>              m_pScene;
    std::unique_ptr<Camera>             m_pCamera;
    std::unique_ptr<Frame>              m_pSource;
    int                                 m_iFrameCount;
    bool                                m_bFrameDone;
	clock_type::time_point              m_tpInit;
	clock_type::time_point              m_tpLastFrame;
	int                                 m_iWidth;
    int                                 m_iHeight;
    int                                 m_iNumWorkers;
    int                                 m_iMaxSamplesPerPixel;
    int                                 m_iMaxTraceDepth;
    uint32_t                            m_uRandSeed;
};


int main(int argc, char *argv[])
{
    auto pLoader = std::make_unique<LoaderDragonScene>();
    int i = 'abcd';

    // start app
    QApplication app(argc, argv);
    MainWindow window(std::move(pLoader));
    window.show();

    return app.exec();
}
