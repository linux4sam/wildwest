/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef GRAPHICSLAYERITEM_H
#define GRAPHICSLAYERITEM_H

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QEvent>
#include "planemanager.h"
#include "graphicsplaneitem.h"

/**
 * @brief The GraphicsLayerItem class
 *
 * A GraphicsPlaneItem that is meant for handling a panning layer of a graphics scene.
 *
 * This expects the image to be 2X the width of the scene, and it will scroll the layer.
 */
class GraphicsLayerItem : public GraphicsPlaneItem
{
public:

    GraphicsLayerItem(struct plane_data* plane, const QPixmap& image, int width, int height, int speed)
        : GraphicsPlaneItem(plane, image.rect()),
          m_image(image),
          m_speed(speed),
          m_plane(plane),
          m_width(width),
          m_height(height),
          m_x(0)
    {
        if (!plane)
            qFatal("invalid plane pointer");

        if (speed < 0)
            m_x = m_image.width()/2;

        plane_set_pan_size(m_plane, width, height);
        plane_set_pan_pos(m_plane, m_x, 0);
        plane_apply(m_plane);
    }

    inline int width() const
    {
        return m_width;
    }

    virtual void reverse()
    {
        m_speed *= -1;
    }

    virtual void advance(int step) override
    {
        if (!step)
            return;

        m_x += m_speed;

        if (m_x >= m_image.width()/2)
            m_x = 0;
        else if (m_x < 0)
            m_x = m_image.width()/2;

        plane_set_pan_pos(m_plane, m_x, 0);
        plane_apply(m_plane);
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        qDebug() << "GraphicsLayerItem::paint";

        draw(m_plane, m_image.toImage());

        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

    virtual ~GraphicsLayerItem()
    {}

protected:
    QPixmap m_image;
    int m_speed;
    struct plane_data* m_plane;
    int m_width;
    int m_height;
    int m_x;
};

#endif // GRAPHICSLAYERITEM_H
