/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef GRAPHICSSPRITEITEM_H
#define GRAPHICSSPRITEITEM_H

#include "planemanager.h"
#include "graphicsplaneitem.h"

#include <QObject>
#include <QGraphicsItem>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <string>
#include <map>

/**
 * @brief The GraphicsSpriteItem class
 *
 * An enhanced GraphicsPlaneItem that specifically handles sprite sheets
 * and animating any number of sequences in those sprite sheets using the
 * hardware plane pan functionality.
 */
class GraphicsSpriteItem : public GraphicsPlaneItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(int frame READ currentFrame WRITE setFrame)

public:

    GraphicsSpriteItem(struct plane_data* plane, const QPixmap &image, int width, int height)
        : GraphicsPlaneItem(plane, QRectF(0, 0, width, height)),
          m_image(image),
          m_frame(0),
          m_flipHorizontal(false),
          m_flipVertical(false)
    {}

    struct sequence
    {
        int m_x;
        int m_y;
        int m_width;
        int m_height;
        int m_count;
    };

    virtual void addSequence(const std::string& name, int x, int y, int width, int height, int count)
    {
        m_sequences[name] = {x,y,width,height,count};
        if (m_sequences.size() == 1)
        {
            m_sequence = name;

            plane_set_pan_pos(m_plane, x, y);
            plane_set_pan_size(m_plane, width, height);
            plane_apply(m_plane);
        }
    }

    virtual void setSequence(const std::string& name)
    {
        if (m_sequence != name)
        {
            m_sequence = name;
            m_frame = 0;
        }
    }

    inline int currentFrame() const
    {
        return m_frame;
    }

    inline int frameCount()
    {
        return m_sequences[m_sequence].m_count;
    }

    inline int frameCount(const std::string& name)
    {
        return m_sequences[name].m_count;
    }

    inline int width()
    {
        return m_sequences[m_sequence].m_width;
    }

    inline int width(const std::string& name)
    {
        return m_sequences[name].m_width;
    }

    inline int height()
    {
        return m_sequences[m_sequence].m_height;
    }

    inline int height(const std::string& name)
    {
        return m_sequences[name].m_height;
    }

    inline void toggleFlipHorizontal()
    {
        m_flipHorizontal = !m_flipHorizontal;
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        qDebug() << "Sprite::paint";

        draw(m_plane, m_image.toImage(), m_flipHorizontal, m_flipVertical);

        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        emit clicked();
        event->ignore();
    }

    virtual ~GraphicsSpriteItem()
    {}

signals:
    void clicked();
    void doubleclicked();

public slots:

    virtual void setFrame(int frame)
    {
        m_frame = frame;

        plane_set_pan_pos(m_plane,
                          m_sequences[m_sequence].m_x + (m_frame * m_sequences[m_sequence].m_width),
                          m_sequences[m_sequence].m_y);
        plane_set_pan_size(m_plane,
                           m_sequences[m_sequence].m_width,
                           m_sequences[m_sequence].m_height);
        plane_apply(m_plane);
    }

protected:

    QPixmap m_image;
    int m_speed;
    int m_frame;
    bool m_flipHorizontal;
    bool m_flipVertical;
    std::map<std::string, sequence> m_sequences;
    std::string m_sequence;
};

#endif // GRAPHICSSPRITEITEM_H
