#ifndef TEXTEDITOUTPUT_H
#define TEXTEDITOUTPUT_H

#include "../api/ioutput.h"

#include <QTextEdit>

class TextEditOutput : public IOutput
{
    Q_OBJECT
public:
    TextEditOutput(QObject *parent) : IOutput(parent)
    {
        edit = new QTextEdit;
        edit->setReadOnly(true);
    }
    virtual QWidget *widget()
    {
        return edit;
    }
    virtual void clear()
    {
        edit->clear();
    }
public slots:
    virtual void outputText(const QString &text)
    {
        edit->append(text);
    }
private:
    QTextEdit *edit;
};


#endif // TEXTEDITOUTPUT_H
