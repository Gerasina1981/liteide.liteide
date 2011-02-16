#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QList>
#include <QIcon>
#include "../../api/ioption.h"

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    void save();
    void load();
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
    void updatePointSizes();
    QList<int> pointSizesForSelectedFont() const;
public:
    bool autoIndent;
    bool autoBlock;
    int fontSize;
    QString fontFamily;
private:
    Ui::ConfigDialog *ui;
};

class SyntaxEditOption : public IOption
{
public:
    SyntaxEditOption() {
        dlg = new ConfigDialog;
    }

    virtual QWidget *widget()
    {
        return dlg;
    }

    virtual QIcon icon()
    {
        return QIcon();
    }

    virtual QString text()
    {
        return "SyntaxEdit";
    }

    virtual void load()
    {
        dlg->load();
    }

    virtual void save()
    {
        dlg->save();
    }
    ConfigDialog *dlg;
};

#endif // CONFIGDIALOG_H
