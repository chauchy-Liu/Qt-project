#ifndef DIALOGPEN_H
#define DIALOGPEN_H

#include <QDialog>

namespace Ui {
class dialogPen;
}

class dialogPen : public QDialog
{
    Q_OBJECT

public:
    explicit dialogPen(QWidget *parent = nullptr);
    ~dialogPen();

private:
    Ui::dialogPen *ui;
};

#endif // DIALOGPEN_H
