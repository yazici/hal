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

#ifndef ARROW_SEPARATED_NET_H
#define ARROW_SEPARATED_NET_H

#include "graph_widget/items/nets/separated_graphics_net.h"

class arrow_separated_net : public separated_graphics_net
{
public:
    static void load_settings();

    arrow_separated_net(const std::shared_ptr<const net> n);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void add_input(const QPointF& scene_position) override;
    void add_output(const QPointF& scene_position) override;

    qreal input_width() const override;
    qreal output_width() const override;

private:
    static qreal s_wire_length;
    static qreal s_input_arrow_offset;
    static qreal s_output_arrow_offset;

    // FIND BETTER NAMES
    static qreal s_arrow_length;
    static qreal s_arrow_height;
    static qreal s_arrow_left_outward_x_shift;
    static qreal s_arrow_right_outward_x_shift;

    static qreal s_input_offset;

    static qreal s_input_width;
    static qreal s_output_width;

    static QPainterPath s_arrow;

    static QBrush s_brush;
};

#endif // ARROW_SEPARATED_NET_H
