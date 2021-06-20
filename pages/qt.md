
# Realtime updates with Qt
Using Qt 5 to build a simple app that just renders the contents of the raytracing image buffer while workers are still building the images.

## Application stucture
We need to create a main window class by inheriting from QMainWindow. We also create a timer with which we will update the view and draw the raytraced frame.  In general the raytracer takes anything from a few seconds to hours to draw a single frame and our application should also show us the raytracer progress by drawing the frame on each timer update, even before it is done rendering.

```C++
class MainWindow : public QMainWindow
{
 public:
    MainWindow(const SimpleScene *_pScene)
        :QMainWindow(),
         m_iWidth(1024),
         m_iHeight(768),
    {
        resize(m_iWidth, m_iHeight);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(200, Qt::PreciseTimer);
        
        ...
    }
    
 protected:
    virtual void paintEvent(QPaintEvent *_event) {
        QPainter painter(this);
        QImage image(m_iWidth, m_iHeight, QImage::Format_RGB888);

        ...
        
        painter.drawImage(0, 0, image);
    }
    
    virtual void timerEvent(QTimerEvent *_event) {
        ...
        
        this->update(this->rect());
    }
    
 private:
    int              m_iWidth;
    int              m_iHeight;
};

```


## Notes on using CMake

