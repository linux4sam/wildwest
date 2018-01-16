/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "planemanager.h"
#include "graphicslayeritem.h"
#include "graphicsspriteitem.h"
#include "graphicsplaneview.h"
#include "tools.h"

#include <QApplication>
#include <QTimer>
#include <QTimeLine>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QStateMachine>
#include <QSignalTransition>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QMessageBox>
#include <QDesktopWidget>

#include <map>
#include <memory>

/**
 * @brief The AnimationStateMachine class
 *
 * A helper class that treats QTimeLine objects like they are states.
 */
class AnimationStateMachine : public QObject
{
    Q_OBJECT
public:

    void addTimeLine(const QString& name, QTimeLine* timeline)
    {
        m_timelines.insert(std::pair<QString, std::shared_ptr<QTimeLine>>(name, std::shared_ptr<QTimeLine>(timeline)));
        QObject::connect(timeline, &QTimeLine::finished, this, &AnimationStateMachine::transition);
    }

    void addTransition(const QString& from, const QString& to)
    {
        m_transitions[from] = to;
    }

    void activate(const QString& name)
    {
        qDebug() << "AnimationStateMachine::activate " << name;

        if (m_current.length())
            m_timelines[m_current]->stop();

        if (m_timelines.find(name) != m_timelines.end())
        {
            m_current = name;
            m_timelines[m_current]->start();
        }
    }

public slots:

    void transition()
    {
        if (m_transitions.find(m_current) != m_transitions.end())
        {
            activate(m_transitions[m_current]);
        }
    }

    void event(const QString& from, const QString& to)
    {
        if (m_current == from)
        {
            activate(to);
        }
    }

protected:

    QString m_current;
    std::map<QString, std::shared_ptr<QTimeLine> > m_timelines;
    std::map<QString, QString> m_transitions;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PlaneManager planes;
    if (!planes.load("screen.config"))
    {
        QMessageBox::critical(0, "Failed to Setup Planes",
                              "This demo requires a version of Qt that provides access to the DRI file descriptor,"
                              " a valid planes screen.config file, and using the linuxfb backend with the env var "
                              "QT_QPA_FB_DRM set.\n");
        return -1;
    }

    QRect screen = QApplication::desktop()->screenGeometry();

    QGraphicsScene scene;

    /*
     * Create scene items.  Some are standard Qt objects, others are custom ones that
     * translate to hardware plane usage behind the scenes.
     */

    QGraphicsPixmapItem* logo = new QGraphicsPixmapItem(QPixmap(":/media/logo.png"));
    logo->setPos(10, 10);
    scene.addItem(logo);

    GraphicsLayerItem overlay0(planes.get("overlay0"), QPixmap(":/media/overlay0.png"),
                               screen.width(), 330, 2);
    overlay0.setPos(0,70);
    scene.addItem(&overlay0);

    GraphicsLayerItem overlay1(planes.get("overlay1"), QPixmap(":/media/overlay1.png"),
                               screen.width(), 110, 4);
    overlay1.setPos(0,370);
    scene.addItem(&overlay1);

    GraphicsSpriteItem man(planes.get("overlay2"), QPixmap(":/media/man.png"), 88, 151);
    man.addSequence("walking", 24, 0, 68, 150, 8);
    man.addSequence("jumping", 14, 152, 80, 151, 7);
    man.addSequence("firing", 14, 310, 88, 151, 4);
    man.setPos((screen.width() / 2) - (88/2), (screen.height() * 0.90) - man.height());
    scene.addItem(&man);

    QProgressBar* progress = new QProgressBar();
    progress->setOrientation(Qt::Horizontal);
    progress->setRange(0, 100);
    progress->setTextVisible(true);
    progress->setAlignment(Qt::AlignCenter);
    progress->setFormat("CPU: %p%");
    progress->setValue(0);
    QPalette p = progress->palette();
    p.setColor(QPalette::Highlight, Qt::red);
    p.setBrush(QPalette::Background, Qt::transparent);
    progress->setPalette(p);
    progress->setMaximumWidth(200);
    QGraphicsProxyWidget *proxy = scene.addWidget(progress);
    proxy->setPos(screen.width() - 200 - 10, 10);

    /*
     * Setup the view.
     */

    GraphicsPlaneView view(&scene);
    view.setStyleSheet( "QGraphicsView { border-style: none; }" );
    view.setBackgroundBrush(QPixmap(":/media/primary.png"));
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.resize(screen.width(), screen.height());
    view.setSceneRect(0, 0, screen.width(), screen.height());
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff );
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff );
    view.show();

    /*
     * Setup states and animations.
     */

    AnimationStateMachine machine;

    QTimeLine *walking = new QTimeLine(600);
    walking->setUpdateInterval(1000 / 32);
    walking->setLoopCount(0);
    walking->setCurveShape(QTimeLine::LinearCurve);
    walking->setFrameRange(0, man.frameCount("walking")-1);
    QObject::connect(walking, &QTimeLine::frameChanged, &man, &GraphicsSpriteItem::setFrame);
    QObject::connect(walking, SIGNAL(valueChanged(qreal)), &scene, SLOT(advance()));
    QObject::connect(walking, &QTimeLine::stateChanged, [&machine,&man](QTimeLine::State newState){
        if (newState == QTimeLine::Running)
            man.setSequence("walking");
    });
    machine.addTimeLine("walking", walking);

    QTimeLine *jumping = new QTimeLine(600);
    jumping->setUpdateInterval(100);
    jumping->setLoopCount(1);
    jumping->setCurveShape(QTimeLine::LinearCurve);
    jumping->setFrameRange(0, man.frameCount("jumping")-1);
    QObject::connect(jumping, &QTimeLine::frameChanged, &man, &GraphicsSpriteItem::setFrame);
    QObject::connect(jumping, &QTimeLine::stateChanged, [&machine,&man](QTimeLine::State newState){
        if (newState == QTimeLine::Running)
            man.setSequence("jumping");
    });
    machine.addTimeLine("jumping", jumping);
    machine.addTransition("jumping", "walking");

    QTimeLine *firing = new QTimeLine(300);
    firing->setUpdateInterval(1000 / 32);
    firing->setLoopCount(1);
    firing->setCurveShape(QTimeLine::LinearCurve);
    firing->setFrameRange(0, man.frameCount("firing")-1);
    QObject::connect(firing, SIGNAL(frameChanged(int)), &man, SLOT(setFrame(int)));
    QObject::connect(firing, &QTimeLine::stateChanged, [&machine,&man](QTimeLine::State newState){
        if (newState == QTimeLine::Running)
            man.setSequence("firing");
    });
    machine.addTimeLine("firing", firing);
    machine.addTransition("firing", "walking");

    QObject::connect(&man, &GraphicsSpriteItem::clicked, [&machine](){
        machine.event("walking","firing");
    });

    QObject::connect(&view, &GraphicsPlaneView::clicked, [&machine](){
        machine.event("walking","jumping");
    });

    machine.activate("walking");

    /*
     * Update the progress bar independently.
     */

    Tools tools;
    QTimer cpuTimer;
    QObject::connect(&cpuTimer, &QTimer::timeout, [&tools,&progress]() {
        tools.updateCpuUsage();
        progress->setValue(tools.cpu_usage[0]);
    });
    cpuTimer.start(1000);

    return app.exec();
}

#include "main.moc"
