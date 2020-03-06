#include "graph_widget/items/nets/separated_graphics_net.h"

#include "netlist/net.h"

#include "graph_widget/graph_widget_constants.h"

qreal separated_graphics_net::s_alpha;

void separated_graphics_net::update_alpha()
{
    if (s_lod >= graph_widget_constants::net_fade_in_lod && s_lod <= graph_widget_constants::net_fade_out_lod)
        s_alpha = (s_lod - graph_widget_constants::net_fade_in_lod) / (graph_widget_constants::net_fade_out_lod - graph_widget_constants::net_fade_in_lod);
    else
        s_alpha = 1;
}

separated_graphics_net::separated_graphics_net(const std::shared_ptr<const net> n) : graphics_net(n)
{
}

void separated_graphics_net::finalize()
{
    // RECT INTENTIONALLY SET SLIGHTLY TOO BIG
    m_rect = m_shape.boundingRect();
    m_rect.adjust(-s_line_width, -s_line_width, s_line_width, s_line_width);
}
