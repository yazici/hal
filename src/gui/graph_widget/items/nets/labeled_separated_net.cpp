#include "graph_widget/items/nets/labeled_separated_net.h"

#include "graph_widget/graph_widget_constants.h"

#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>

static const qreal baseline = 1;

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

    s_pen.setColor(QColor(160, 160, 160)); // USE STYLESHEETS
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

    if (m_draw_output)
    {
        painter->drawLine(QPointF(0, 0), QPointF(s_wire_length, 0));
        painter->drawText(QPointF(s_wire_length + s_text_offset, -(s_font_height / 2) + s_font_ascend + baseline), m_text);
    }

    for (const QPointF& position : m_input_wires)
    {
        QPointF to(position.x() - s_wire_length, position.y());
        painter->drawLine(position, to);
        qreal horizontal_offset = s_text_offset + m_text_width;
        qreal vertical_offset = -(s_font_height / 2) + s_font_ascend + baseline;
        painter->drawText(QPointF(to.x() - horizontal_offset, to.y() + vertical_offset), m_text);
    }

#ifdef HAL_DEBUG_GUI_GRAPHICS
    s_pen.setColor(Qt::green);
    painter->setPen(s_pen);
    painter->drawPath(m_shape);
#endif
}

void labeled_separated_net::add_output()
{
    if (m_draw_output)
        return;

    m_draw_output = true;

    // 1 SUBPATH

    // 2 SUBPATHS
    m_shape.moveTo(QPointF(-s_stroke_width, -s_stroke_width));
    m_shape.lineTo(QPointF(s_wire_length + s_stroke_width, -s_stroke_width));
    m_shape.lineTo(QPointF(s_wire_length + s_stroke_width, s_stroke_width));
    m_shape.lineTo(QPointF(-s_stroke_width, s_stroke_width));
    m_shape.closeSubpath();

    // TBC...

//    m_shape.moveTo(QPointF(0, -s_stroke_width / 2));
//    m_shape.lineTo(QPointF(m_shape.currentPosition().x() + s_wire_length + s_text_offset, m_shape.currentPosition().y()));
//    m_shape.lineTo(QPointF(m_shape.currentPosition().x(), m_shape.currentPosition().y() - QFontMetricsF(s_font).height() / 2 + s_stroke_width / 2));
//    m_shape.lineTo(QPointF(m_shape.currentPosition().x() + m_text_width, m_shape.currentPosition().y()));
//    m_shape.lineTo(QPointF(m_shape.currentPosition().x(), m_shape.currentPosition().y() + QFontMetricsF(s_font).height()));
//    m_shape.lineTo(QPointF(m_shape.currentPosition().x() - m_text_width, m_shape.currentPosition().y()));
//    m_shape.lineTo(QPointF(m_shape.currentPosition().x(), m_shape.currentPosition().y() - QFontMetricsF(s_font).height() / 2 + s_stroke_width / 2));
//    m_shape.lineTo(QPointF(m_shape.currentPosition().x() - s_wire_length - s_text_offset, m_shape.currentPosition().y()));
//    m_shape.closeSubpath();
}

void labeled_separated_net::add_input(const QPointF& scene_position)
{
    QPointF mapped_position = mapFromScene(scene_position);
    m_input_wires.append(mapped_position);

    m_shape.moveTo(QPointF(mapped_position.x(), mapped_position.y() - s_stroke_width / 2));
    m_shape.lineTo(QPointF(m_shape.currentPosition().x() - s_wire_length - s_text_offset, m_shape.currentPosition().y()));
    m_shape.lineTo(QPointF(m_shape.currentPosition().x(), m_shape.currentPosition().y() - QFontMetricsF(s_font).height() / 2 + s_stroke_width / 2));
    m_shape.lineTo(QPointF(m_shape.currentPosition().x() - m_text_width, m_shape.currentPosition().y()));
    m_shape.lineTo(QPointF(m_shape.currentPosition().x(), m_shape.currentPosition().y() + QFontMetricsF(s_font).height()));
    m_shape.lineTo(QPointF(m_shape.currentPosition().x() + m_text_width, m_shape.currentPosition().y()));
    m_shape.lineTo(QPointF(m_shape.currentPosition().x(), m_shape.currentPosition().y() - QFontMetricsF(s_font).height() / 2 + s_stroke_width / 2));
    m_shape.lineTo(QPointF(m_shape.currentPosition().x() + s_wire_length + s_text_offset, m_shape.currentPosition().y()));
    m_shape.closeSubpath();
}

void labeled_separated_net::finalize()
{
    m_rect = m_shape.boundingRect();
    m_rect.adjust(-1, -1, 1, 1);
}

qreal labeled_separated_net::input_width() const
{
    return s_wire_length + s_text_offset + m_text_width;
}

qreal labeled_separated_net::output_width() const
{
    return s_wire_length + s_text_offset + m_text_width;
}
