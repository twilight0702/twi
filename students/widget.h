#ifndef WIDGET_H
#define WIDGET_H

#include <QSqlTableModel>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //打开数据库
    void openDatabase();

    //展示全部信息
    void showStudentsData();
    void showCourseDataOfAStudent();

    void keyPressEvent(QKeyEvent *event);
    void on_tableView_clicked(const QModelIndex &index);
    void showCoursesForStudent();
    void on_tableView_4_clicked(const QModelIndex &index);
    void showCourses(const QString &studentID);

    void showSortingMenu(int logicalIndex);
    void showSortingMenu_3(int logicalIndex);

    void searchStudent();
    void searchCourse();

    QString studentID;
    QString studentName;

private slots:

    //第一页
    void on_add_clicked();

    void on_delete_2_clicked();

    void on_deleteData_clicked();

    void on_showAllStudents_clicked();

    void on_check_clicked();

    //第二页

    void on_add_3_clicked();

    void on_showAllStudents_2_clicked();

    void on_delete_4_clicked();

    void on_check_2_clicked();

    void on_save_2_clicked();



    void on_showAllStudents_3_clicked();

    void on_return_2_clicked();

    void on_return_3_clicked();




    void on_showAllStudents_4_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Widget *ui;
    QSqlTableModel *model;
    QSqlTableModel *model_2;
    QSqlQueryModel *model_3;
    QSqlTableModel* model_4;
//    QString studentID;
//    QString studentName;
    QString courseID;
    QString courseName;

};
#endif // WIDGET_H
