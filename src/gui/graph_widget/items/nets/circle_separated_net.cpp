#include "graph_widget/items/nets/circle_separated_net.h"

#include "graph_widget/graph_widget_constants.h"

#include <assert.h>

#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>

qreal circle_separated_net::s_wire_length;
qreal circle_separated_net::s_circle_offset;
qreal circle_separated_net::s_radius;

QBrush circle_separated_net::s_brush;

void circle_separated_net::load_settings()
{
    s_wire_length   = 26;
    s_circle_offset = 0;
    s_radius        = 3;

    s_brush.setStyle(Qt::SolidPattern);
    s_pen.setColor(QColor(160, 160, 160)); // USE STYLESHEETS
}

circle_separated_net::circle_separated_net(const std::shared_ptr<const net> n) : separated_graphics_net(n)
{
}

void circle_separated_net::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    if (s_lod < graph_widget_constants::separated_net_min_lod)
        return;

    QColor color = (option->state & QStyle::State_Selected) ? s_selection_color : m_color;
    color.setAlphaF(s_alpha);

    s_pen.setColor(color);
    painter->setPen(s_pen);

    if (m_fill_icon)
    {
        s_brush.setColor(color);
        painter->setBrush(s_brush);
    }

    const bool original_antialiasing = painter->renderHints() & QPainter::Antialiasing;

    for (const QPointF& position : m_input_positions)
    {
        QPointF to(position.x() - s_wire_length, position.y());
        painter->drawLine(position, to);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawEllipse(QPointF(to.x() - s_circle_offset, to.y()), s_radius, s_radius);
        painter->setRenderHint(QPainter::Antialiasing, original_antialiasing);
    }

    for (const QPointF& position : m_output_positions)
    {
        QPointF to(position.x() + s_wire_length, position.y());
        painter->drawLine(position, to);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawEllipse(QPointF(to.x() + s_wire_length + s_circle_offset, to.y()), s_radius, s_radius);
        painter->setRenderHint(QPainter::Antialiasing, original_antialiasing);
    }

    painter->setBrush(QBrush());

#ifdef HAL_DEBUG_GUI_GRAPH_WIDGET
    bool original_cosmetic = s_pen.isCosmetic();
    s_pen.setCosmetic(true);
    s_pen.setColor(Qt::green);
    painter->setPen(s_pen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPath(m_shape);
    painter->setRenderHint(QPainter::Antialiasing, original_antialiasing);
    s_pen.setCosmetic(original_cosmetic);
#endif
}

void circle_separated_net::add_input(const QPointF& scene_position)
{
    // REWRITE
    QPointF mapped_position = mapFromScene(scene_position);
    m_input_positions.append(mapped_position);

    const qreal x = mapped_position.x() + s_shape_width;
    const qreal y = mapped_position.y();

    m_shape.moveTo(QPointF(x, y - s_shape_width));
    m_shape.lineTo(QPointF(x - s_wire_length - s_shape_width * 2, y - s_shape_width));
    m_shape.lineTo(QPointF(x - s_wire_length - s_shape_width * 2, y + s_shape_width));
    m_shape.lineTo(QPointF(x, y + s_shape_width));
    m_shape.closeSubpath();

    const QPointF point(mapped_position.x() - s_wire_length - s_circle_offset, mapped_position.y());
    const qreal radius = s_radius + s_shape_width;

    m_shape.addEllipse(point, radius, radius);

    // MAYBE BOUNDING RECT IS BETTER HERE
}

void circle_separated_net::add_output(const QPointF& scene_position)
{
    // REWRITE
    QPointF mapped_position = mapFromScene(scene_position);
    m_input_positions.append(mapped_position);

    m_shape.moveTo(QPointF(-s_shape_width, -s_shape_width));
    m_shape.lineTo(QPointF(s_wire_length + s_shape_width, -s_shape_width));
    m_shape.lineTo(QPointF(s_wire_length + s_shape_width, s_shape_width));
    m_shape.lineTo(QPointF(-s_shape_width, s_shape_width));
    m_shape.closeSubpath();

    const QPointF point(s_wire_length + s_circle_offset, 0);
    const qreal radius = s_radius + s_shape_width;

    m_shape.addEllipse(point, radius, radius);

    // MAYBE BOUNDING RECT IS BETTER HERE
}

qreal circle_separated_net::input_width() const
{
    return s_wire_length + s_circle_offset + s_radius + s_line_width / 2;
}

qreal circle_separated_net::output_width() const
{
    return s_wire_length + s_circle_offset + s_radius + s_line_width / 2;
}
