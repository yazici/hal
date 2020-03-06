#include "style/style.h"

#include "core/log.h"

#include "gui/graph_widget/items/graphics_item.h"
#include "gui/graph_widget/items/graphics_net.h"
#include "gui/graph_widget/items/gates/minimal_graphics_gate.h"
#include "gui/graph_widget/items/gates/standard_graphics_gate.h"
#include "gui/graph_widget/items/modules/standard_graphics_module.h"
#include "gui/graph_widget/items/nets/arrow_separated_net.h"
#include "gui/graph_widget/items/nets/circle_separated_net.h"
#include "gui/graph_widget/items/nets/hollow_arrow_separated_net.h"
#include "gui/graph_widget/items/nets/labeled_separated_net.h"
#include "gui/graph_widget/items/nets/separated_graphics_net.h"
#include "gui/graph_widget/items/nets/standard_graphics_net.h"
#include "gui/graph_widget/items/utility_items/node_drag_shadow.h"
#include "gui/gui_globals.h"
#include "gui/svg_icon_engine/svg_icon_engine.h"

#include <QApplication>
#include <QFile>
#include <QHash>
#include <QRegularExpression>
#include <QTextStream>

namespace style
{
    QString default_stylesheet()
    {
        QString stylesheet;
        QFile file(":/style/hal");

        if (!file.open(QFile::ReadOnly))
        {
            log_error("gui", "Unable to open default stylesheet. This error should never be reached");
            return stylesheet;
        }

        stylesheet = QString(file.readAll());
        file.close();
        return stylesheet;
    }

    QString get_stylesheet()
    {
        QString stylesheet;
        {
            QString path = g_settings_manager.get("stylesheet/base").toString();
            QFile file(path);

            if (!file.exists())
            {
                log_error("gui", "Specified stylesheet '{}' does not exist, proceeding with default style", path.toStdString());
                return default_stylesheet();
            }

            if (!file.open(QFile::ReadOnly))
            {
                log_error("gui", "Unable to open specified stylesheet '{}', proceeding with default style", path.toStdString());
                return default_stylesheet();
            }

            stylesheet = QString(file.readAll());
            file.close();
        }

        QString path = g_settings_manager.get("stylesheet/definitions").toString();

        if (path.isEmpty())
            return stylesheet;

        QHash<QString, QString> definitions;
        QRegularExpression key_regex("(?<key>@[a-zA-Z0-9_]+)($|[^a-zA-Z0-9_])");
        key_regex.optimize();
        {
            QFile file(path);

            if (!file.exists())
            {
                log_error("gui", "Specified stylesheet definitions file '{}' does not exist, proceeding with default style", path.toStdString());
                return default_stylesheet();
            }

            if (!file.open(QFile::ReadOnly))
            {
                log_error("gui", "Unable to open specified stylesheet definitions file '{}', proceeding with default style", path.toStdString());
                return default_stylesheet();
            }

            QRegularExpression value_regex("^\\s*(?<value>\\S[^;]*);");
            value_regex.optimize();

            QTextStream in(&file);
            while (!in.atEnd())
            {
                QString line = in.readLine();

                if (line.isEmpty())
                    continue;

                int index          = line.indexOf(':');
                QString key_part   = line.left(index);
                QString value_part = line.remove(0, index + 1);

                key_part.replace(',', ' ');
                QString value = value_regex.match(value_part).captured("value");

                QRegularExpressionMatchIterator it = key_regex.globalMatch(key_part);
                while (it.hasNext())
                {
                    QRegularExpressionMatch match = it.next();
                    definitions.insert(match.captured("key"), value);
                }
            }
            file.close();
        }

        QRegularExpressionMatchIterator it = key_regex.globalMatch(stylesheet);
        int offset                         = 0;

        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();

            QString key   = match.captured("key");
            QString value = definitions.value(key);

            int index  = match.capturedStart() + offset;
            int length = key.length();

            stylesheet.remove(index, length);
            stylesheet.insert(index, value);

            offset = offset - key.length() + value.length();
        }

        return stylesheet;
    }

    QIcon get_styled_svg_icon(const QString& from_to_colors, const QString& svg_path)
    {
        QString svg_data;
        QFile file(svg_path);

        if (file.open(QFile::ReadOnly))
            svg_data = QString(file.readAll());
        else
        {
            //log_error("error");
            return QIcon();
        }

        if (svg_data.isEmpty())
        {
            //log_error("error");
            return QIcon();
        }

        if (!from_to_colors.isEmpty())
        {
            QRegExp color_regex("#[0-9a-f]{6}", Qt::CaseInsensitive);
            QRegExp all_to_color_regex("\\s*all\\s*->\\s*#[0-9a-f]{6}\\s*", Qt::CaseInsensitive);
            QRegExp color_to_color_regex("\\s*(#[0-9a-f]{6}\\s*->\\s*#[0-9a-f]{6}\\s*,\\s*)*#[0-9a-f]{6}\\s*->\\s*#[0-9a-f]{6}\\s*", Qt::CaseInsensitive);
            if (all_to_color_regex.exactMatch(from_to_colors))
            {
                color_regex.indexIn(from_to_colors);
                QString color = color_regex.cap(0);
                svg_data.replace(color_regex, color.toUtf8());
            }
            else if (color_to_color_regex.exactMatch(from_to_colors))
            {
                QString copy = from_to_colors;
                copy         = copy.simplified();
                copy.replace(" ", "");

                QStringList list = copy.split(",");

                for (QString string : list)
                {
                    QString from_color = string.left(7);
                    QString to_color   = string.right(7);

                    QRegExp regex(from_color);
                    svg_data.replace(regex, to_color.toUtf8());
                }
            }
            else
            {
                //print error
                return QIcon();
            }
        }

        return QIcon(new svg_icon_engine(svg_data.toStdString()));
    }

    void debug_update()
    {
        graphics_item::load_settings();

        standard_graphics_module::load_settings();

        //graphics_gate::load_settings();
        standard_graphics_gate::load_settings();
        minimal_graphics_gate::load_settings();

        graphics_net::load_settings();
        standard_graphics_net::load_settings();
        separated_graphics_net::load_settings();
        arrow_separated_net::load_settings();
        circle_separated_net::load_settings();
        hollow_arrow_separated_net::load_settings();
        labeled_separated_net::load_settings();
        node_drag_shadow::load_settings();
    }

} // namespace style
