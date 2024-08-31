#ifndef TIP_H
#define TIP_H


#include <QWidget>

namespace Ui {
class tip;
}

class tip : public QWidget
{
    Q_OBJECT

public:
    explicit tip(QWidget *parent = nullptr);
    ~tip();

    void setText(QString text);


private:
    Ui::tip *ui;
};

#endif // TIP_H
