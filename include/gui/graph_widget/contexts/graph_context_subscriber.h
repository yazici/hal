#ifndef GRAPH_CONTEXT_SUBSCRIBER_H
#define GRAPH_CONTEXT_SUBSCRIBER_H

#include <QString>

class graph_context_subscriber
{
public:
    virtual ~graph_context_subscriber() = default;

    virtual void handle_scene_available() = 0;
    virtual void handle_scene_unavailable() = 0;
    virtual void handle_context_about_to_be_deleted() = 0;

    virtual void handle_status_update(const int percent) = 0;
    virtual void handle_status_update(const QString& message) = 0;
};

#endif // GRAPH_CONTEXT_SUBSCRIBER_H
