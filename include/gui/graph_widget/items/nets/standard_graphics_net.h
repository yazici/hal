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

#ifndef STANDARD_GRAPHICS_NET_H
#define STANDARD_GRAPHICS_NET_H

#include "gui/graph_widget/items/nets/graphics_net.h"

#include <QLineF>
#include <QVector>

class net;

class standard_graphics_net : public graphics_net
{
public:
    struct h_line
    {
        qreal small_x;
        qreal big_x;
        qreal y;
    };

    struct v_line
    {
        qreal x;
        qreal small_y;
        qreal big_y;
    };

    struct lines
    {
        qreal src_x;
        qreal src_y;

        QVector<h_line> h_lines;
        QVector<v_line> v_lines;

//        void remove_zero_length_lines();
//        void fix_order();
//        void move(qreal x, qreal y);
    };

    static void load_settings();
    static void update_alpha();

    //standard_graphics_net(const std::shared_ptr<const net> n, const lines& l);
    standard_graphics_net(const std::shared_ptr<const net> n, lines& l, bool draw_arrow);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    static qreal s_alpha;
    static qreal s_radius;
    static QBrush s_brush;
    static QPainterPath s_arrow;

    QVector<QLineF> m_lines;
    QVector<QPointF> m_splits;

    bool m_draw_arrow;
};

#endif // STANDARD_GRAPHICS_NET_H
