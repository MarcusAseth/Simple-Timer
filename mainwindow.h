#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLineEdit>
#include <QLabel>
#include <QSoundEffect>
#include <chrono>



QT_BEGIN_NAMESPACE
namespace Ui {
class SimpleTimer;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    enum class EState{
        MainMenu,
        Play,
        Pause
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   void UpdateCountdown();

private:

   void OnPlayButtonPressed();
   void OnPlayButtonReleased();
   void OnRestartButtonPressed();
   void OnRestartButtonReleased();

   void UpdateCountdownDisplay();
   void ResetCountdown(bool IsRest);
   void ToggleMainMenu(bool Show);
   void Restart();

    void SetDefaultLineEditStyle(QLineEdit* LineEdit);
    void SetDefaultLabelStyle(QLabel* Label);
    void SetDefaultLineEditValidator(QLineEdit* LineEdit, QString String);
    void FixDisplayedValue(QLineEdit* LineEdit);

private:
    Ui::SimpleTimer *ui;
    QTimer* Timer;
    QSoundEffect* SoundEffect;

    QIcon PlayReleased;
     QIcon PlayPressed;
    QIcon PauseReleased;
    QIcon PausePressed;
    QIcon RestartReleased;
    QIcon RestartPressed;

    EState State;
    unsigned CurrentCycle;
    std::chrono::seconds Countdown;

    bool bIsRest;
};
#endif // MAINWINDOW_H
