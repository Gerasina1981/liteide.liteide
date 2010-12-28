#include "configdialog.h"
#include "ui_configdialog.h"

#include <QFontDatabase>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    QFontDatabase db;
    const QStringList families = db.families();
    ui->familyComboBox->addItems(families);
}

void ConfigDialog::updatePointSizes()
{
    // Update point sizes
    const int oldSize = fontSize;
    if (ui->sizeComboBox->count()) {
        const QString curSize = ui->sizeComboBox->currentText();
        bool ok = true;
        int oldSize = curSize.toInt(&ok);
        if (!ok)
            oldSize = fontSize;
        ui->sizeComboBox->clear();
    }
    const QList<int> sizeLst = pointSizesForSelectedFont();
    int idx = 0;
    int i = 0;
    for (; i < sizeLst.count(); ++i) {
        if (idx == 0 && sizeLst.at(i) >= oldSize)
            idx = i;
        ui->sizeComboBox->addItem(QString::number(sizeLst.at(i)));
    }
    if (ui->sizeComboBox->count())
        ui->sizeComboBox->setCurrentIndex(idx);
}

QList<int> ConfigDialog::pointSizesForSelectedFont() const
{
    QFontDatabase db;
    const QString familyName = ui->familyComboBox->currentText();
    QList<int> sizeLst = db.pointSizes(familyName);
    if (!sizeLst.isEmpty())
        return sizeLst;

    QStringList styles = db.styles(familyName);
    if (!styles.isEmpty())
        sizeLst = db.pointSizes(familyName, styles.first());
    if (sizeLst.isEmpty())
        sizeLst = QFontDatabase::standardSizes();

    return sizeLst;
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::load()
{
    QFontDatabase db;
    const QStringList families = db.families();
    const int idx = families.indexOf(fontFamily);

    ui->familyComboBox->setCurrentIndex(idx);

    updatePointSizes();
}

void ConfigDialog::save()
{
    fontFamily = ui->familyComboBox->currentText();
    QString curSize = ui->sizeComboBox->currentText();

    if (ui->sizeComboBox->count()) {
        const QString curSize = ui->sizeComboBox->currentText();
        bool ok = true;
        int size = curSize.toInt(&ok);
        if (ok) {
            fontSize = size;
        }
     }
}
