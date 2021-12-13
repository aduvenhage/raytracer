
# Realtime updates with Qt
Using Qt to build a simple app that just renders the contents of the raytracing image buffer while workers are still building the images.

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

On each paint event we can manipulate the Qt image buffer directly.
```C++
   
   // copy from source to Qt
   uchar *pDstData = image.bits();
   uchar *pSrcData = m_pSourceRGB888
   std::memcpy(pDstData, pSrcData, width * height * 3);
   
   // or, just fill image
   image.fill(Qt::black);

```

In this case the raytracer should output with the same width, height and color format (RGB888) as the Qt buffer.
