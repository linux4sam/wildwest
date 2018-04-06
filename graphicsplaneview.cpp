/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "graphicsplaneview.h"
#include <QApplication>
#include <QPaintEvent>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

GraphicsPlaneView::GraphicsPlaneView(QGraphicsScene *scene)
    : QGraphicsView(scene)
{
    setAttribute(Qt::WA_NoSystemBackground);
}

void GraphicsPlaneView::paintEvent(QPaintEvent * event)
{
    qDebug() << "GraphicsPlaneView::paintEvent " << event->region().boundingRect();
    QGraphicsView::paintEvent(event);
}

bool GraphicsPlaneView::eventFilter(QObject* object, QEvent* event)
{
    qDebug() << "GraphicsPlaneView::eventFilter " << event;

    if (event->type() == QEvent::UpdateRequest) { return true; }
    if (event->type() == QEvent::Paint) { return true; }

    Q_UNUSED(object);

    return false;
}

bool GraphicsPlaneView::event(QEvent *event)
{
    qDebug() << "GraphicsPlaneView::event " << event->type();

    if (event->type() == QEvent::Paint) { return true; }

    return QGraphicsView::event(event);
}

void GraphicsPlaneView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (!event->isAccepted())
        emit clicked();
}

void GraphicsPlaneView::keyPressEvent(QKeyEvent* k)
{
    if(k->key() == 48){
        QApplication::instance()->exit();
    }
}

GraphicsPlaneView::~GraphicsPlaneView()
{}
