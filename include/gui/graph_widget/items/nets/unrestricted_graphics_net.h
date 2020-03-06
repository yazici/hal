//  MIT License
//
//  Copyright (c) 2019 Ruhr-University Bochum, Germany, Chair for Embedded Security. All Rights reserved.
//  Copyright (c) 2019 Marc Fyrbiak, Sebastian Wallat, Max Hoffmann ("ORIGINAL AUTHORS"). All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#ifndef UNRESTRICTED_GRAPHICS_NET_H
#define UNRESTRICTED_GRAPHICS_NET_H

#include "gui/graph_widget/items/nets/graphics_net.h"

#include <QLineF>
#include <QVector>

class net;

class unrestricted_graphics_net : public graphics_net
{
public:
    unrestricted_graphics_net(const std::shared_ptr<const net> n);

    virtual void set_visuals(const visuals& v) Q_DECL_OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) Q_DECL_OVERRIDE;

    void line_to(const QPointF& scene_position);
    void line_to_x(const qreal scene_x);
    void line_to_y(const qreal scene_y);
    void move_pen_to(const QPointF& scene_position);

    void finalize();

    QPointF current_scene_position() const;

private:
    QPainterPath m_path;
    QVector<QLineF> m_lines;
};

#endif // UNRESTRICTED_GRAPHICS_NET_H
