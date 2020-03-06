#ifndef GRAPH_SCENE_H
#define GRAPH_SCENE_H

#include "def.h"

#include "gui/gui_globals.h"
#include "gui/graph_widget/shaders/graph_shader.h"
#include "items/utility_items/node_drag_shadow.h"
#include "netlist/gate.h"
#include "netlist/module.h"

#include <QGraphicsScene>
#include <QPair>
#include <QVector>

//class gate_navigation_popup;
class graphics_gate;
class graphics_item;
class graphics_module;
class graphics_net;

namespace graph_widget_constants
{
enum class grid_type;
}

class graphics_scene : public QGraphicsScene
{
    Q_OBJECT

public:

    static void set_lod(const qreal& lod);
    static void set_grid_enabled(const bool& value);
    static void set_grid_clusters_enabled(const bool& value);
    static void set_grid_type(const graph_widget_constants::grid_type& grid_type);

    static void set_grid_base_line_color(const QColor& color);
    static void set_grid_cluster_line_color(const QColor& color);
    static void set_grid_base_dot_color(const QColor& color);
    static void set_grid_cluster_dot_color(const QColor& color);

    static QPointF snap_to_grid(const QPointF& pos) Q_DECL_DEPRECATED;

    graphics_scene(QObject* parent = nullptr);

    void start_drag_shadow(const QPointF& posF, const QSizeF& sizeF, const node_drag_shadow::drag_cue cue);
    void move_drag_shadow(const QPointF& posF, const node_drag_shadow::drag_cue cue);
    void stop_drag_shadow();
    QPointF drop_target();

    void add_item(graphics_item* item);
    void remove_item(graphics_item* item);

    void delete_all_items();

    void connect_all();
    void disconnect_all();

    void update_visuals(const graph_shader::shading& s);

    void move_nets_to_background();

    const graphics_gate* get_gate_item(const u32 id) const;

    #ifdef GUI_DEBUG_GRID
    void debug_set_layouter_grid(const QVector<qreal>& debug_x_lines, const QVector<qreal>& debug_y_lines, qreal debug_default_height, qreal debug_default_width);
    #endif

//    void update_utility_items();

public Q_SLOTS:
    void handle_intern_selection_changed();
    void handle_extern_selection_changed(void* sender);
    void handle_extern_subfocus_changed(void* sender);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void handle_global_setting_changed(void* sender, const QString& key, const QVariant& value);

private:
    struct module_data
    {
        u32 id;
        graphics_module* item;
    };

    struct gate_data
    {
        u32 id;
        graphics_gate* item;
    };

    struct net_data
    {
        u32 id;
        graphics_net* item;
    };

    static qreal s_lod;

    static const qreal s_grid_fade_start;
    static const qreal s_grid_fade_end;

    static bool s_grid_enabled;
    static bool s_grid_clusters_enabled;
    static graph_widget_constants::grid_type s_grid_type;

    static QColor s_grid_base_line_color;
    static QColor s_grid_cluster_line_color;

    static QColor s_grid_base_dot_color;
    static QColor s_grid_cluster_dot_color;

    using QGraphicsScene::addItem;
    using QGraphicsScene::removeItem;
    using QGraphicsScene::clear;

    void drawBackground(QPainter* painter, const QRectF& rect) Q_DECL_OVERRIDE;

    node_drag_shadow* m_drag_shadow_gate;
    
    QVector<module_data> m_module_items;
    QVector<gate_data> m_gate_items;
    QVector<net_data> m_net_items;

    #ifdef GUI_DEBUG_GRID
    void debug_draw_layouter_grid(QPainter* painter, const int x_from, const int x_to, const int y_from, const int y_to);
    QVector<qreal> m_debug_x_lines;
    QVector<qreal> m_debug_y_lines;
    qreal m_debug_default_width;
    qreal m_debug_default_height;
    bool m_debug_grid_enable;
    #endif

//    gate_navigation_popup* m_left_gate_navigation_popup;
//    gate_navigation_popup* m_right_gate_navigation_popup;
};

#endif // GRAPH_SCENE_H
