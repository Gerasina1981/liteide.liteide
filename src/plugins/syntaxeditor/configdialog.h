#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QList>
#include <QIcon>
#include "../../api/iapp.h"
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
    explicit ConfigDialog(IApplication *app, QWidget *parent = 0);
    ~ConfigDialog();
    void updatePointSizes();
    QList<int> pointSizesForSelectedFont() const;
public:
    bool autoIndent;
    bool autoBlock;
    bool autoWord;
    int fontSize;
    QString fontFamily;
private:
    Ui::ConfigDialog *ui;
    IApplication *liteApp;
};

class SyntaxEditOption : public QObject, public IOption
{
    Q_OBJECT
public:
    SyntaxEditOption(IApplication *app) {
        dlg = new ConfigDialog(app);
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
        emit reloadConfig();
    }
    ConfigDialog *dlg;
signals:
    void reloadConfig();
};

#endif // CONFIGDIALOG_H
