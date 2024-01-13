#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QPainter>
#include<QTimer>
#include<QTime>
#include <QKeyEvent>
#include <QMessageBox>

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置初始参数
    cellNumX = cellNumY = 100;  //格子数
    cellLengthX = cellLengthY = 10; // 格子长和宽
    rankFile = "score.txt";
    //rankDlg = new RankDialog(this , rankFile);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(ui->radioEasy, SIGNAL(clicked()), this, SLOT(setHardLevel()));
    connect(ui->radioMid, SIGNAL(clicked()), this, SLOT(setHardLevel()));
    connect(ui->radioHard, SIGNAL(clicked()), this, SLOT(setHardLevel()));
    resize(200 + 10 + cellNumX * cellLengthX, 20 + cellNumY * cellLengthY);//设置窗口大小
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);// 禁止最大化按钮
    setFixedSize(this -> width(), this->height()); // 禁止拖动窗口大小

    //申请棋盘储存空间: 动态二维数组 cellNumx * cellNumy
    //每个点都有五种状态 蛇头 , 蛇身子 , 墙, 食物 ,nothing
    board = new Role* [cellNumX];  // board 是一个指向 Role数组 的指针
    for(int i = 0; i < cellNumX; i++)
        board[i] = new Role[cellNumY]; // 数组中的每个元素都是 Role的数组
    initGame();
}

void MainWindow::initGame(){
    foodNum = 0;
    ui -> lcdNumber -> display(foodNum);
    direct = newDirect = RIGHT; // 蛇当前前进的方向, 下一次前进的方向初始值
    ui -> radioEasy -> setChecked(true);
    ui -> btnButton -> setEnabled(true); // 开始按钮
    ui -> btnPause -> setDisabled(true);
    ui -> btnRelnit -> setDisabled(true);
    //设置棋盘为空的地方
    for(int i = 0; i < cellNumX - 1; i++){
        for(int j = 0; j < cellNumY - 1; j++){
            board[i][j] = NOTHING;
        }
    }
    //设置棋盘最外面一圈是 墙
    for(int i = 0; i < cellNumX; i++){
        board[i][0] = WALL;
        board[i][cellNumY-1] = WALL;
        board[0][i] = WALL;
        board[cellNumX - 1][i] = WALL;
    }
    snake.clear();
    snake.enqueue(QPoint(1, cellNumX/2));//蛇尾
    snake.enqueue(QPoint(2, cellNumX/2));//蛇身
    snake.enqueue(QPoint(3, cellNumX/2));//蛇头 , 舌头在队列尾巴
    board[1][cellNumX/2] = SNAKEBODY;
    board[2][cellNumX/2] = SNAKEBODY;
    board[3][cellNumX/2] = SNAKEHEAD;

    generateFood();
    update();
}
void MainWindow::generateFood(){
    qsrand(QTime::currentTime().msec());
    do{
        int x = 1 + qrand() % (cellNumX - 1);
        int y = 1 + qrand() % (cellNumY - 1);
        if(board[x][y] == NOTHING){
            board[x][y] = FOOD;
            break;
        }
    }while(true);
}

void MainWindow::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QBrush brush(Qt::SolidPattern);
    for(int i = 0; i < cellNumX; i++){
        for(int j = 0; j < cellNumY; j++){
            switch(board[i][j]){
            case NOTHING:
                brush.setColor(QColor(255, 255, 200));
                break;
            case SNAKEBODY:
                brush.setColor(Qt::blue);
                break;
            case WALL:
                brush.setColor(Qt::black);
                break;
            case SNAKEHEAD:
                brush.setColor(Qt::darkBlue);
                break;
            case FOOD:
                brush.setColor(Qt::red);
                break;
            }
            painter.setBrush(brush); // 整个界面全部用小方块画出来
            painter.drawRect(150 + i * cellLengthX, 20 + j * cellLengthY, cellLengthX, cellLengthY);
        }

        ui -> lcdNumber -> display(foodNum);
    }

}

void MainWindow::setHardLevel(){
    if(ui-> radioEasy -> isChecked())
        hardLevel = 4;
    else if(ui -> radioMid -> isChecked())
        hardLevel = 6;
    else hardLevel = 8;

    timer -> setInterval(1000/hardLevel);
}
MainWindow::~MainWindow()
{
    delete ui;
    for(int i = 0; i < cellNumX; i++){
        delete board[i];
    }
    delete board;
}


void MainWindow::on_btnButton_clicked()
{
    setHardLevel();
    timer -> start(1000/hardLevel);
    ui -> btnButton -> setDisabled(true);
    ui -> btnPause -> setEnabled(true);
    ui -> btnRelnit -> setEnabled(true);

}

//void MainWindow::on_btnPause_toggled(bool checked)
//{

//}

void MainWindow::on_btnRelnit_clicked()
{
    timer -> stop();
    initGame();
}

//void MainWindow::on_btnRank_clicked()
//{

//}

void MainWindow::timeout(){
    QPoint delPoint;
    QPoint snakeHead = snake.last();
    int newCellx = snakeHead.x();
    int newCelly = snakeHead.y();
    // 先判断方向是不是可以进行操作
    // 再判断要装上的是不是可以去的地方, 如果不能去, 直接死
    //如果是可以去的地方, 直接在 snake 的队列中 入队
    switch(direct){
    case UP:
        if(newDirect != DOWN) direct = newDirect;
        break;
    case DOWN:
        if(newDirect != UP) direct = newDirect;
        break;
    case LEFT:
        if(newDirect != RIGHT) direct = newDirect;
        break;
    case RIGHT:
        if(newDirect != LEFT) direct = newDirect;
        break;
    }

    switch(direct){
    case UP:
        newCelly -= 1;
        break;
    case DOWN:
        newCelly += 1;
        break;
    case LEFT:
        newCellx -= 1;
        break;
    case RIGHT:
        newCellx += 1;
        break;
    }

    switch(board[newCellx][newCelly]){
    case WALL:
    case SNAKEBODY:
    case SNAKEHEAD:
        gameOver();
        break;
    case NOTHING:
        // 队列, 队尾元素, 入队, 队头元素 ,出队
        snake.enqueue(QPoint(newCellx, newCelly));
        delPoint = snake.dequeue();
        board[snakeHead.x()][snakeHead.y()] = SNAKEBODY;
        board[newCellx][newCelly] = SNAKEHEAD;
        board[delPoint.x()][delPoint.y()] = NOTHING;
        break;
    case FOOD:
        snake.enqueue(QPoint(newCellx, newCelly));
        board[snakeHead.x()][snakeHead.y()] = SNAKEBODY;
        board[newCellx][newCelly] = SNAKEHEAD;
        foodNum++;
        generateFood();
        break;
    }
    this -> update();

}

void MainWindow::keyPressEvent(QKeyEvent *event){
    switch(event -> key()){
    case Qt::Key_Down:
        newDirect = DOWN;
        break;
    case Qt::Key_Up:
        newDirect = UP;
        break;
    case Qt::Key_Left:
        newDirect = LEFT;
        break;
    case Qt::Key_Right:
        newDirect = RIGHT;
        break;
    }
}

void MainWindow::gameOver(){
    timer -> stop();

    QMessageBox gameOverMsgBox;
    gameOverMsgBox.setText(" GG ");

    QFont font;
    font.setPointSize(24);
    gameOverMsgBox.setFont(font);
    gameOverMsgBox.exec();
    initGame();
}
