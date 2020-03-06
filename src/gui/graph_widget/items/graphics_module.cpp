#include "gui/graph_widget/items/graphics_module.h"

#include "netlist/module.h"
#include "netlist/net.h"

#include "gui/graph_widget/graphics_scene.h"

graphics_module::graphics_module(const std::shared_ptr<const module> m) : graphics_node(hal::item_type::module, m->get_id(), QString::fromStdString(m->get_name()))
{
    assert(m);

    for (const std::shared_ptr<net>& n : m->get_input_nets())
    {
        QVector<std::string> pin_types;

        for (const endpoint& e : n->get_destinations())
        {
            if (e.gate)
                if (m->contains_gate(e.gate, true) && !pin_types.contains(e.pin_type))
                {
                    pin_types.append(e.pin_type);
                    m_input_pins.append(module_pin{n->get_id(), QString::fromStdString(e.pin_type)});
                }
        }
    }

    for (const std::shared_ptr<net>& n : m->get_output_nets())
    {
        endpoint e = n->get_source();

        if (e.get_gate())
            m_output_pins.append(module_pin{n->get_id(), QString::fromStdString(e.get_pin())});
    }
}

void graphics_module::set_visuals(const graphics_node::visuals& v)
{
    setVisible(v.visible);

    m_color = v.main_color;
}
