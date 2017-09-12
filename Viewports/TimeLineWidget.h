/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_TIME_LINE_WIDGET__
#define __UI_TIME_LINE_WIDGET__

#include <QTime>
#include <QList>
#include <QTimer>
#include <QWidget>
#include <QSlider>
#include <QComboBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <FabricUI/Actions/BaseAction.h>
#include <FabricUI/Util/FEDoubleSpinBox.h>

namespace FabricUI {
namespace TimeLine {

class FrameSlider : public QSlider
{
  Q_OBJECT

  public:
    FrameSlider( 
      QWidget *parent
      );

    virtual ~FrameSlider();

    // [FE-6862]
    // Override the slider positioning to make the slider
    // go directly to the clicked positioning (instead of
    // going there by incremenets)
    virtual void mousePressEvent( 
      QMouseEvent *event
      ); /*override*/
    
    /// Implementation of QWidget.
    virtual void resizeEvent(
      QResizeEvent *event
      );
};

/// \brief an Basic Time slider Widget using QT
class TimeLineWidget : public QWidget
{
  Q_OBJECT

  public:
    TimeLineWidget();

    virtual ~TimeLineWidget();

    // loop mode constants.
    #define LOOP_MODE_PLAY_ONCE 0
    #define LOOP_MODE_LOOP      1
    #define LOOP_MODE_OSCILLATE 2
    #define LOOP_MODE_DEFAULT   LOOP_MODE_LOOP

    /// update the internal time and also emit the signals
    void updateTime(
      int frame, 
      bool onLoadingScene = false
      );

    /// Gets the current time
    int getTime();

    /// Gets the current time
    int getRangeStart();

    /// Gets the current time
    int getRangeEnd();

    /// Sets the time range
    void setTimeRange(
      int start, 
      int end
      );
    
    /// Sets the frame rate (will clamp to combo box options)
    void setFrameRate( 
      float framesPerSecond 
      );
    
    /// Gets the loop mode
    int loopMode() const;

    /// Sets the loop mode
    void setLoopMode(
      int mode
      );

    /// Gets the sim mode
    int simulationMode() const;

    /// Sets the sim mode
    void setSimulationMode(
      int mode
      );

    /// Gets a pointer to the timer
    QTimer *getTimer();

    /// Gets the framerate of the timer
    double getFps() const;

    /// Sets the timer from a QTimer interval.
    void setTimerFromFps(
      double fps
      );

    /// Gets the frame rate (frame per seconds),
    /// currently set in m_frameRateComboBox.
    double getFrameRateFromComboBox();

    /// Gets the framerate. 1000 = max.
    float framerate() const;

    /// Gets the framerate. 1000 = max.
    bool isPlaying() const;

  signals :
    /// Emited when ever the time on the widget changed
    /// Connect this slight to any slots that need to know about the time
    void frameChanged(
      int frame
      );

    /// Emitted whenever playback changes
    void playbackChanged(
      bool isPlaying
      );

    void rangeChanged( 
      int start, 
      int end 
      );

    /// Emmitted when 
    void targetFrameRateChanged( 
      float frameRate 
      );
  
  public slots:
    /// Slider value changed
    void sliderChanged(
      int frame
      );

    /// slider value changed by an increment
    void frameChangedBy(
      int frame
      );

    /// Current Frame value change on the DoubleSpinBox
    void currentFrameChanged();
    
    /// Either the start or end time of the slider changed , this will adjust both regarderless which one changed
    void updateFrameRange();
    
    /// Starts or stops the playback (act as a switch on the playing state)
    void onPlayButtonToggled( 
      bool checked 
      );
    
    /// Starts the playback
    void play();

    /// Stops the playback
    void pause();

    /// Toogles the playback
    void togglePlayback();

    /// Sets the frame to the start of the current time slider range
    void goToStartFrame();
    
    /// Goes one frame back based on the current frame 
    void goToPreviousFrame();
    
    /// Goes one frame forward based on the current frame 
    void goToNextFrame();

    /// Sets the frame to the end of the current time slider range
    void goToEndFrame();

    /// Called each time the timer is triggered ( basicly when playing)
    void timerUpdate();

    /// Called to reload the QSS styles
    void reloadStyles();

  private slots:
    /// Called when the frameRate is changed ( only few fixed option at moment)
    void updateTargetFrameRate(
      int index
      );

    /// Called when the looping scheme changed
    void loopModeChanged(
      int index
      );

    /// Called when the simulation mode changed
    void simModeChanged(
      int index
      );
    
    /// internal time changing function 
    /// if you want to set the time from the outside use updateTime(int)
    void setTime(
      int frame
      );
    
  private:
    /// the timer in charge of the playback. 
    /// this is paused when not active
    double m_fps;
    QTimer *m_timer;
    QTime m_lastFrameTime;
    
    int m_simMode;
    int m_loopMode;
    int m_direction;
    bool m_settingTime;
    int m_lastSteppedFrame;

    /// elements
    FrameSlider *m_frameSlider;
    FabricUI::Util::FEDoubleSpinBox *m_endSpinBox;
    FabricUI::Util::FEDoubleSpinBox *m_startSpinBox;
    FabricUI::Util::FEDoubleSpinBox *m_currentFrameSpinBox;
    QPushButton *m_playButton;
    QPushButton *m_goToEndFrameButton;
    QPushButton *m_goToNextFrameButton;
    QPushButton *m_goToStartFrameButton;
    QPushButton *m_goToPreviousFrameButton;
    QComboBox *m_simModeComBox;
    QComboBox *m_loopModeComBox;
    QComboBox *m_frameRateComboBox;
};

// FE-5724 
class BaseTimeLineAction : public Actions::BaseAction
{
  Q_OBJECT

  public:
    BaseTimeLineAction(
      TimeLineWidget *timeLine,
      QString name, 
      QString text = "", 
      QKeySequence shortcut = QKeySequence(),
      Qt::ShortcutContext context = Qt::ApplicationShortcut,
      bool enable = true)
      : Actions::BaseAction( 
        timeLine
        , name 
        , text 
        , shortcut 
        , context
        , enable)
      , m_timeLine(timeLine)
    {
    }

    virtual ~BaseTimeLineAction()
    {
    }

  protected:
    TimeLineWidget *m_timeLine;
};

class TogglePlaybackAction : public BaseTimeLineAction
{
  Q_OBJECT

  public:
    TogglePlaybackAction(
      TimeLineWidget *timeLine,
      bool enable = true)
      : BaseTimeLineAction( 
        timeLine
        , "TimeLineWidget::TogglePlaybackAction"
        , "Toggle playback" 
        , QKeySequence("Ctrl+Space") 
        , Qt::ApplicationShortcut
        , enable)
    {
    }

    virtual ~TogglePlaybackAction()
    {
    }

  private slots:
    virtual void onTriggered()
    {
      m_timeLine->togglePlayback();
    }
};

class PlayAction : public BaseTimeLineAction
{
  Q_OBJECT

  public:
    PlayAction(
      TimeLineWidget *timeLine,
      bool enable = true)
      : BaseTimeLineAction( 
        timeLine
        , "TimeLineWidget::PlayAction"
        , "Play" 
        , QKeySequence("Ctrl+Up") 
        , Qt::ApplicationShortcut
        , enable)
    {
    }

    virtual ~PlayAction()
    {
    }

  private slots:
    virtual void onTriggered()
    {
      m_timeLine->play();
    }
};

class PauseAction : public BaseTimeLineAction
{
  Q_OBJECT

  public:
    PauseAction(
      TimeLineWidget *timeLine,
      bool enable = true)
      : BaseTimeLineAction( 
        timeLine
        , "TimeLineWidget::PauseAction"
        , "Pause" 
        , QKeySequence("Ctrl+Down") 
        , Qt::ApplicationShortcut
        , enable)
    {
    }

    virtual ~PauseAction()
    {
    }

  private slots:
    virtual void onTriggered()
    {
      m_timeLine->pause();
    }
};

class GoToNextFrameAction : public BaseTimeLineAction
{
  Q_OBJECT

  public:
    GoToNextFrameAction(
      TimeLineWidget *timeLine,
      bool enable = true)
      : BaseTimeLineAction( 
        timeLine
        , "TimeLineWidget::GoToNextFrameAction"
        , "Go to next Frame" 
        , QKeySequence("Ctrl+Right") 
        , Qt::ApplicationShortcut
        , enable)
    {
    }

    virtual ~GoToNextFrameAction()
    {
    }
    
  private slots:
    virtual void onTriggered()
    {
      m_timeLine->goToNextFrame();
    }
};

class GoToPreviousFrameAction : public BaseTimeLineAction
{
  Q_OBJECT

  public:
    GoToPreviousFrameAction(
      TimeLineWidget *timeLine,
      bool enable = true)
      : BaseTimeLineAction( 
        timeLine
        , "TimeLineWidget::GoToPreviousFrameAction"
        , "Go to previous Frame" 
        , QKeySequence("Ctrl+Left") 
        , Qt::ApplicationShortcut
        , enable)
    {
    }

    virtual ~GoToPreviousFrameAction()
    {
    }
    
  private slots:
    virtual void onTriggered()
    {
      m_timeLine->goToPreviousFrame();
    }
};

class GoToEndFrameAction : public BaseTimeLineAction
{
  Q_OBJECT

  public:
    GoToEndFrameAction(
      TimeLineWidget *timeLine,
      bool enable = true)
      : BaseTimeLineAction( 
        timeLine
        , "TimeLineWidget::GoToEndFrameAction"
        , "Go to end Frame" 
        , QKeySequence("Ctrl+End") 
        , Qt::ApplicationShortcut
        , enable)
    {
    }

    virtual ~GoToEndFrameAction()
    {
    }

  private slots:
    virtual void onTriggered()
    {
      m_timeLine->goToEndFrame();
    }
};

class GoToStartFrameAction : public BaseTimeLineAction
{
  Q_OBJECT

  public:
    GoToStartFrameAction(
      TimeLineWidget *timeLine,
      bool enable = true)
      : BaseTimeLineAction( 
        timeLine
        , "TimeLineWidget::GoToStartFrameAction"
        , "Go to start Frame" 
        , QKeySequence("Ctrl+Home") 
        , Qt::ApplicationShortcut
        , enable)
    {
    }

    virtual ~GoToStartFrameAction()
    {
    }
    
  private slots:
    virtual void onTriggered()
    {
      m_timeLine->goToStartFrame();
    }
};

} // namespace Timeline
} // namespace FabricUI

#endif // __UI_TIME_LINE_WIDGET__
