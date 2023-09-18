#include "messageitem.h"


MessageItem::MessageItem(GotifyModel::Message * message)
    : QStandardItem()
{
    setData(message->id, MessageRole::Id);
    setData(message->appId, MessageRole::AppId);
    setData(message->priority, MessageRole::Priority);
    setData(message->title, MessageRole::Title);
    setData(message->date, MessageRole::Date);
    setData(message->message, MessageRole::Message);
    setData(message->markdown, MessageRole::Markdown);
}


int MessageItem::id()
{
    return data(MessageRole::Id).toInt();
}


int MessageItem::appId()
{
    return data(MessageRole::AppId).toInt();
}


int MessageItem::priority()
{
    return data(MessageRole::Priority).toInt();
}


QString MessageItem::title()
{
    return data(MessageRole::Title).toString();
}


QDateTime MessageItem::date()
{
    return data(MessageRole::Date).toDateTime();
}


QString MessageItem::message()
{
    return data(MessageRole::Message).toString();
}


bool MessageItem::markdown()
{
    return data(MessageRole::Markdown).toBool();
}
