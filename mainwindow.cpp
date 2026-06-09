#include "mainwindow.h"
#include <QIntValidator>
#include <QFile>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimpleTimer)
    , PlayReleased{":/resources/play_released.png"}
    , PlayPressed{":/resources/play_pressed.png"}
    , PauseReleased{":/resources/pause_released.png"}
    , PausePressed{":/resources/pause_pressed.png"}
    , RestartReleased{":/resources/restart_released.png"}
    , RestartPressed{":/resources/restart_pressed.png"}
    , State{EState::MainMenu}
    , bIsRest{false}
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("://resources/logo.png"));
    ui->centralwidget->setStyleSheet(
        "#centralwidget {"
        "background-image: url(:/resources/background.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "}"
        );

    QPixmap p(":/resources/background.png");
    qDebug() << p.isNull();

    ui->CountdownDisplay->hide();
    ui->CountdownLabel1->hide();
    ui->CurrentCycleDisplay->hide();
    ui->CountdownLabel2->hide();

    //Sound Effect

    SoundEffect = new QSoundEffect();
    SoundEffect->setSource(QUrl("qrc:/resources/chime01.wav"));

    //Play Button

    ui->Button01->setStyleSheet(R"(QPushButton {background-color: transparent; border: none;})");

    ui->Button01->setIcon(PlayReleased);
    ui->Button01->setIconSize(QSize(55,55));

    connect(ui->Button01, &QPushButton::pressed, this,  &MainWindow::OnPlayButtonPressed);
    connect(ui->Button01, &QPushButton::released, this, &MainWindow::OnPlayButtonReleased);

    //Restart Button

    ui->Restart->setStyleSheet(R"(QPushButton {background-color: transparent; border: none;})");

    ui->Restart->setIcon(RestartReleased);
    ui->Restart->setIconSize(QSize(35,35));

    connect(ui->Restart, &QPushButton::pressed, this,  &MainWindow::OnRestartButtonPressed);
    connect(ui->Restart, &QPushButton::released, this, &MainWindow::OnRestartButtonReleased);

    //Typing Fields

    SetDefaultLineEditStyle(ui->Cycles);
    SetDefaultLineEditValidator(ui->Cycles,"([1-9]|[1-9][0-9])");

    SetDefaultLineEditStyle(ui->WorkHours);
    SetDefaultLineEditValidator(ui->WorkHours,"(00|0[1-9]|1[0-6])");

    SetDefaultLineEditStyle(ui->WorkMinutes);
    SetDefaultLineEditValidator(ui->WorkMinutes,"(00|[0-5][0-9])");

    SetDefaultLineEditStyle(ui->WorkSeconds);
    SetDefaultLineEditValidator(ui->WorkSeconds,"(00|[0-5][0-9])");

    SetDefaultLineEditStyle(ui->RestHours);
    SetDefaultLineEditValidator(ui->RestHours,"(00|0[1-9]|1[0-6])");

    SetDefaultLineEditStyle(ui->RestMinutes);
    SetDefaultLineEditValidator(ui->RestMinutes,"(00|[0-5][0-9])");

    SetDefaultLineEditStyle(ui->RestSeconds);
     SetDefaultLineEditValidator(ui->RestSeconds,"(00|[0-5][0-9])");

    SetDefaultLabelStyle(ui->Label1);
    SetDefaultLabelStyle(ui->Label2);
    SetDefaultLabelStyle(ui->Label3);
    SetDefaultLabelStyle(ui->Label4);
    SetDefaultLabelStyle(ui->Label5);
    SetDefaultLabelStyle(ui->Label6);
    SetDefaultLabelStyle(ui->Label7);


    ui->CountdownLabel1->setStyleSheet(R"(
QLabel {
    border-radius: 8px;
    padding: 4px;
    background-color: #1b1b1b;
}
)");

    ui->CountdownLabel2->setStyleSheet(R"(
QLabel {
    border-radius: 8px;
    padding: 4px;
    background-color: #1b1b1b;
}
)");


    ui->Cycles->setText(QString("8"));
    ui->WorkHours->setText(QString("01"));
    ui->WorkMinutes->setText(QString("00"));
    ui->WorkSeconds->setText(QString("00"));
    ui->RestHours->setText(QString("00"));
    ui->RestMinutes->setText(QString("15"));
    ui->RestSeconds->setText(QString("00"));

    //Timer

    Timer = new QTimer(this);
    connect(Timer, &QTimer::timeout, this, &MainWindow::UpdateCountdown);
}

MainWindow::~MainWindow()
{
    delete Timer;
    delete SoundEffect;
    delete ui;
}

void MainWindow::UpdateCountdown()
{
    Countdown--;

    UpdateCountdownDisplay();


    if(Countdown <= std::chrono::seconds::zero())
    {
       SoundEffect->play();

        if(bIsRest)
       {
           CurrentCycle--;
           ui->CurrentCycleDisplay->setText(QString::number(CurrentCycle));

           if(CurrentCycle == 0)
            {
               Restart();
           }else
           {
               ui->Cycles->setText(QString::number(ui->Cycles->text().toInt()-1));
           }

            bIsRest = false;
            ResetCountdown(bIsRest);
       }
        else
        {
            bIsRest = true;
            ResetCountdown(bIsRest);
        }

    }


    //restart timer after Rest

}

void MainWindow::OnPlayButtonPressed()
{
    if(State == EState::Play )
    {
        ui->Button01->setIcon(PausePressed);
    }
    else
    {
        ui->Button01->setIcon(PlayPressed);
    }
}

void MainWindow::OnPlayButtonReleased()
{
    if(State == EState::MainMenu)
    {
        State = EState::Play;
        ui->Button01->setIcon(PauseReleased);
        ResetCountdown(bIsRest);

        Timer->start(1000);

        CurrentCycle = ui->Cycles->text().toInt();
        UpdateCountdownDisplay();
        ui->CurrentCycleDisplay->setText(QString::number(CurrentCycle));
        ToggleMainMenu(false);
    }
    else if(State == EState::Play)
    {
        State = EState::Pause;
        ui->Button01->setIcon(PlayReleased);

        Timer->stop();
    }else
    {
        State = EState::Play;
        ui->Button01->setIcon(PauseReleased);
        Timer->start();
    }
}

void MainWindow::OnRestartButtonPressed()
{
    ui->Restart->setIcon(RestartPressed);
}

void MainWindow::OnRestartButtonReleased()
{
    ui->Restart->setIcon(RestartReleased);

    Restart();
}

void MainWindow::Restart()
{
    Timer->stop();
    State = EState::MainMenu;
    CurrentCycle = 1;
    Countdown = std::chrono::seconds::zero();
    bIsRest = false;
    ui->Button01->setIcon(PlayReleased);
    this->setWindowTitle("Simple Timer");


    ToggleMainMenu(true);
}

void MainWindow::UpdateCountdownDisplay()
{
    std::chrono::minutes Minutes = std::chrono::duration_cast<std::chrono::minutes>(Countdown);
    std::chrono::seconds Seconds = Countdown - Minutes;

    QString NewTime = QString("%1:%2")
            .arg(Minutes.count(), 2, 10, QChar('0'))
            .arg(Seconds.count(), 2, 10, QChar('0'));

    ui->CountdownDisplay->setText(NewTime);

    if(Minutes >= std::chrono::minutes(100))
    {

        ui->CountdownLabel2->setFixedSize(300, 81);
        ui->CountdownLabel2->move(15,176);

    }
    else
    {
        ui->CountdownLabel2->setFixedSize(251, 81);
        ui->CountdownLabel2->move(40,176);
    }

    this->setWindowTitle(NewTime);
}

void MainWindow::ResetCountdown(bool IsRest)
{
    if(bIsRest)
    {
        Countdown = std::chrono::hours(ui->RestHours->text().toInt())
                        + std::chrono::minutes(ui->RestMinutes->text().toInt())
                        + std::chrono::seconds(ui->RestSeconds->text().toInt());
    }else
    {
        Countdown = std::chrono::hours(ui->WorkHours->text().toInt())
                        + std::chrono::minutes(ui->WorkMinutes->text().toInt())
                        + std::chrono::seconds(ui->WorkSeconds->text().toInt());
    }
}

void MainWindow::ToggleMainMenu(bool Show)
{
    if(Show)
    {
        ui->Cycles->show();
        ui->WorkHours->show();
        ui->WorkMinutes->show();
        ui->WorkSeconds->show();
        ui->RestHours->show();
        ui->RestMinutes->show();
        ui->RestSeconds->show();
        ui->Label1->show();
        ui->Label2->show();
        ui->Label3->show();
        ui->Label4->show();
        ui->Label5->show();
        ui->Label6->show();
        ui->Label7->show();

        ui->CountdownDisplay->hide();
        ui->CountdownLabel1->hide();
        ui->CurrentCycleDisplay->hide();
        ui->CountdownLabel2->hide();
    }else
    {
        ui->Cycles->hide();
        ui->WorkHours->hide();
        ui->WorkMinutes->hide();
        ui->WorkSeconds->hide();
        ui->RestHours->hide();
        ui->RestMinutes->hide();
        ui->RestSeconds->hide();
        ui->Label1->hide();
        ui->Label2->hide();
        ui->Label3->hide();
        ui->Label4->hide();
        ui->Label5->hide();
        ui->Label6->hide();
        ui->Label7->hide();

        ui->CountdownDisplay->show();
        ui->CountdownLabel1->show();
        ui->CurrentCycleDisplay->show();
        ui->CountdownLabel2->show();
    }
}

void MainWindow::SetDefaultLineEditStyle(QLineEdit *LineEdit)
{
    LineEdit->setStyleSheet(R"(
QLineEdit {
    color: white;
    border-radius: 8px;
    padding: 4px;
    background-color: #1b1b1b;

}
)");
}

void MainWindow::SetDefaultLabelStyle(QLabel *Label)
{
    Label->setStyleSheet(R"(
QLabel {
    color: white;
}
)");
}

void MainWindow::SetDefaultLineEditValidator(QLineEdit *LineEdit, QString String)
{
    LineEdit->setValidator(
        new QRegularExpressionValidator(
            QRegularExpression(String),
            LineEdit
            )
        );
}
