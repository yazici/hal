#include "gui/graph_widget/items/nets/graphics_net.h"

#include "netlist/net.h"

#include <QPen>

QPen graphics_net::s_pen;

qreal graphics_net::s_line_width;
qreal graphics_net::s_stroke_width;

void graphics_net::load_settings()
{
    s_line_width = 1.8;

    s_stroke_width = 5;

    s_pen.setWidthF(s_line_width);
    s_pen.setJoinStyle(Qt::MiterJoin);
}

graphics_net::graphics_net(const std::shared_ptr<const net> n) : graphics_item(hal::item_type::net, n->get_id())
{
}

QRectF graphics_net::boundingRect() const
{
    return m_rect;
}

QPainterPath graphics_net::shape() const
{
    return m_shape;
}

void graphics_net::set_visuals(const graphics_net::visuals& v)
{
    setVisible(v.visible);

    m_color = v.color;
    m_line_style = v.style;
    m_fill_icon = v.fill_icon;
}
