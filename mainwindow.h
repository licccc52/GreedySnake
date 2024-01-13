#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QQueue>
//#include "rankdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit  不能再进行隐式类型转换, (在调用函数的时候 进行自动类型转换)
    explicit MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent* event);


private slots:
    void on_btnButton_clicked();

//    void on_btnPause_toggled(bool checked);

    void on_btnRelnit_clicked();

 //   void on_btnRank_clicked();

    void setHardLevel();

    void timeout();

private:
    Ui::MainWindow *ui;
    void generateFood();
    void initGame();
    void gameOver();
    enum Role{ // 枚举类型的元素是互斥的, 每次只能有一个元素生效
        NOTHING, SNAKEBODY, SNAKEHEAD, WALL, FOOD
    };

    enum Direction{
        UP, DOWN, LEFT, RIGHT
    };

    Role **board;
    QQueue<QPoint> snake;
    int cellNumX, cellNumY;
    int cellLengthX, cellLengthY;
    Direction direct, newDirect;
    int hardLevel = 1;
    QTimer *timer;
    int foodNum = 0;
    QString rankFile;
    //RankDialog* rankDlg;
};
#endif // MAINWINDOW_H
