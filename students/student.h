#ifndef STUDENT_H
#define STUDENT_H

#include <QWidget>
#include <courses.h>

#include <QSqlQueryModel>
#include <QSqlTableModel>

namespace Ui {
class student;
}

class student : public QWidget
{
    Q_OBJECT

public:
    explicit student(QWidget *parent = nullptr);
    ~student();

    void setID_and_Name(QString id,QString name);
    void showCourse();

    void setCoursesInstance(courses c);




private slots:
    void on_pushButton_clicked();

    void on_deleteCourse_clicked();

    void on_select_clicked();

    void on_refresh_clicked();

private:
    Ui::student *ui;

    QString id;
    QString name;

    QSqlTableModel* model_1;
    QSqlTableModel* model_2;

    QSqlQueryModel* model1;
    QSqlQueryModel* model2;

    courses* coursesInstance;



};

#endif // STUDENT_H
