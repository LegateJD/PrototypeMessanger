#ifndef MESSAGELABEL_H
#define MESSAGELABEL_H

#include <QObject>
#include <QLabel>

class MessageLabel : public QLabel {

public:
    MessageLabel(int messageId);
    int messageId;
};

#endif // MESSAGELABEL_H
