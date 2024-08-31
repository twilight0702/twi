#ifndef COURSES_H
#define COURSES_H

#include <QWidget>
#include <QSqlRecord>
#include <QSqlQueryModel>

namespace Ui {
class courses;
}

class courses : public QWidget
{
    Q_OBJECT

public:
    explicit courses(QWidget *parent = nullptr);
    ~courses();

    void showCourses();
    void showNumberOfStudents();
    void set_student_id(const QString &id);

signals:
    void dataIsChange();

private slots:
    void on_pushButton_clicked();

    void on_refresh_clicked();

private:
    Ui::courses *ui;
    QSqlQueryModel* model;

    QString student_id;
};

#endif // COURSES_H
