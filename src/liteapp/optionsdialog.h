#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class IOption;

class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    void addPage(IOption *opt);    
    explicit OptionsDialog(QWidget *parent = 0);
protected:
    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
    QList<IOption*> options;
public slots:
    virtual void show();
protected slots:
    void changePage(QListWidgetItem*,QListWidgetItem*);
    void closeAndSave();
    void apply();
};

#endif // OPTIONSDIALOG_H
