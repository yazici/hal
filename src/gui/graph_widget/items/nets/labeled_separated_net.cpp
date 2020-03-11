#include "graph_widget/items/nets/labeled_separated_net.h"

#include "graph_widget/graph_widget_constants.h"

#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>

static const qreal s_baseline = 1;

qreal labeled_separated_net::s_wire_length;
qreal labeled_separated_net::s_text_offset;

QFont labeled_separated_net::s_font;
qreal labeled_separated_net::s_font_height;
qreal labeled_separated_net::s_font_ascend;

void labeled_separated_net::load_settings()
{
    s_wire_length = 20;
    s_text_offset = 2.4;

    s_font = QFont("Iosevka");
    s_font.setPixelSize(12);
    QFontMetricsF fm(s_font);
    s_font_height = fm.height();
    s_font_ascend = fm.ascent();
}

labeled_separated_net::labeled_separated_net(const std::shared_ptr<const net> n, const QString& text) : separated_graphics_net(n),
  m_text(text)
{
    QFontMetricsF fm(s_font);
    m_text_width = fm.width(m_text);
}

void labeled_separated_net::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    if (s_lod < graph_widget_constants::separated_net_min_lod)
        return;

    QColor color = (option->state & QStyle::State_Selected) ? s_selection_color : m_color;
    color.setAlphaF(s_alpha);

    s_pen.setColor(color);
    painter->setPen(s_pen);
    painter->setFont(s_font);

    if (m_fill_icon)
    {
        s_brush.setColor(m_fill_color);
        s_brush.setStyle(m_brush_style);
        painter->setBrush(s_brush); // ???
    }

    for (const QPointF& position : m_input_positions)
    {
        if (m_fill_icon)
            painter->fillRect(QRectF(position.x() - s_wire_length - s_text_offset - m_text_width, position.y() - s_font_height / 2, m_text_width, s_font_height), s_brush);

        QPointF point(position.x() - s_wire_length, position.y());
        painter->drawLine(position, point);
        point.setX(point.x() - s_text_offset - m_text_width);
        point.setY(point.y() + s_baseline + s_font_ascend - s_font_height / 2);
        painter->drawText(point, m_text);
    }

    for (const QPointF& position : m_output_positions)
    {
        if (m_fill_icon)
            painter->fillRect(QRectF(position.x() + s_wire_length + s_text_offset, position.y() - s_font_height / 2, m_text_width, s_font_height), s_brush);

        QPointF point(position.x() + s_wire_length, position.y());
        painter->drawLine(position, point);
        point.setX(point.x() + s_text_offset);
        point.setY(point.y() + s_baseline + s_font_ascend - s_font_height / 2);
        painter->drawText(point, m_text);
    }

    s_brush.setStyle(Qt::NoBrush);
    painter->setBrush(s_brush);

#ifdef HAL_DEBUG_GUI_GRAPH_WIDGET
    s_pen.setColor(Qt::green);
    painter->setPen(s_pen);
    painter->drawPath(m_shape);
#endif
}

void labeled_separated_net::add_input(const QPointF& scene_position)
{
    const QPointF mapped_position = mapFromScene(scene_position);
    m_input_positions.append(mapped_position);

    const qreal half_of_shape_width = s_shape_width / 2;
    const qreal half_of_font_height = s_font_height / 2;

    QPointF point(mapped_position.x() - s_wire_length - half_of_shape_width, mapped_position.y() - half_of_shape_width);

    m_shape.moveTo(point);
    point.setX(point.x() + s_wire_length + s_shape_width);
    m_shape.lineTo(point);
    point.setY(point.y() + s_shape_width);
    m_shape.lineTo(point);
    point.setX(point.x() - s_wire_length - s_shape_width);
    m_shape.lineTo(point);
    m_shape.closeSubpath();

    point.setX(mapped_position.x() - s_wire_length - s_text_offset - m_text_width); // - half_of_shape_width
    point.setY(mapped_position.y() - half_of_font_height); // - half_of_shape_width

    m_shape.moveTo(point);
    point.setX(point.x() + m_text_width); // + s_shape_width
    m_shape.lineTo(point);
    point.setY(point.y() + s_font_height); // + s_shape_width
    m_shape.lineTo(point);
    point.setX(point.x() - m_text_width); // - s_shape_width
    m_shape.lineTo(point);
    m_shape.closeSubpath();
}

void labeled_separated_net::add_output(const QPointF& scene_position)
{
    const QPointF mapped_position = mapFromScene(scene_position);
    m_output_positions.append(mapped_position);

    const qreal half_of_shape_width = s_shape_width / 2;
    const qreal half_of_font_height = s_font_height / 2;

    QPointF point(mapped_position.x() - half_of_shape_width, mapped_position.y() - half_of_shape_width);

    m_shape.moveTo(point);
    point.setX(point.x() + s_wire_length + s_shape_width);
    m_shape.lineTo(point);
    point.setY(point.y() + s_shape_width);
    m_shape.lineTo(point);
    point.setX(point.x() - s_wire_length - s_shape_width);
    m_shape.lineTo(point);
    m_shape.closeSubpath();

    point.setX(mapped_position.x() + s_wire_length + s_text_offset); // - half_of_shape_width
    point.setY(mapped_position.y() - half_of_font_height); // - half_of_shape_width

    m_shape.moveTo(point);
    point.setX(point.x() + m_text_width); // + s_shape_width
    m_shape.lineTo(point);
    point.setY(point.y() + s_font_height); // + s_shape_width
    m_shape.lineTo(point);
    point.setX(point.x() - m_text_width); // - s_shape_width
    m_shape.lineTo(point);
    m_shape.closeSubpath();
}

qreal labeled_separated_net::input_width() const
{
    return s_wire_length + s_text_offset + m_text_width;
}

qreal labeled_separated_net::output_width() const
{
    return s_wire_length + s_text_offset + m_text_width;
}
