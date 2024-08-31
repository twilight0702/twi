#ifndef LOGIN_H
#define LOGIN_H

#include <QSqlDatabase>
#include <QWidget>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

    void setDatabase();


    bool checkEdit();

    //之后所有要用到数据库的地方，统一用这个地址
    static QString fileName;

private slots:
    void on_showCode_clicked();

    void on_loginButton_clicked();

    void on_name_textChanged(const QString &arg1);

    void on_code_textChanged(const QString &arg1);

    void on_selectDatabase_clicked();

private:
    Ui::login *ui;

    QString editName;
    QString editCode;

    static QSqlDatabase db;
};

#endif // LOGIN_H
