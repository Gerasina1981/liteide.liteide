#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QList>

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
    int zoomSize;
private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
