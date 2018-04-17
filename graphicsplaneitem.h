/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef GRAPHICSPLANEITEM_H
#define GRAPHICSPLANEITEM_H

#include <QGraphicsItem>
#include "planemanager.h"

/**
 * @brief The GraphicsPlaneItem class
 *
 *
 */
class GraphicsPlaneItem : public QObject, public QGraphicsItem
{
public:

    GraphicsPlaneItem(struct plane_data* plane, const QRectF& bounding);

    virtual QRectF boundingRect() const override
    {
        return m_bounding;
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

    virtual ~GraphicsPlaneItem()
    {}

protected:

    virtual void moveEvent(const QPointF& point);

    /**
     * @brief draw
     *
     * A special drawing function that draws an image directly to a plane.
     *
     * @param plane
     * @param image
     * @param horizontal
     * @param vertical
     */
    void draw(struct plane_data* plane, QImage image, bool horizontal = false, bool vertical = false, bool scale = true);

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QRectF m_bounding;
    struct plane_data* m_plane;
};

#endif // GRAPHICSPLANEITEM_H
