#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    btnNums = {
        {Qt::Key_0, ui->btnNum0},
        {Qt::Key_1, ui->btnNum1},
        {Qt::Key_2, ui->btnNum2},
        {Qt::Key_3, ui->btnNum3},
        {Qt::Key_4, ui->btnNum4},
        {Qt::Key_5, ui->btnNum5},
        {Qt::Key_6, ui->btnNum6},
        {Qt::Key_7, ui->btnNum7},
        {Qt::Key_8, ui->btnNum8},
        {Qt::Key_9, ui->btnNum9}
    };

    foreach(auto btn,btnNums)
        connect(btn,SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    // connect(ui->btnNum0,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum1,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum2,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum3,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum4,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum5,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum6,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum7,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum8,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum9,SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    connect(ui->btnMultiplied,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClick()));
    connect(ui->btnDivision,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClick()));
    connect(ui->btnMinus,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClick()));
    connect(ui->btnPlus,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClick()));
    connect(ui->btnInverse,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClick()));
    connect(ui->btnNegation,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClick()));
    connect(ui->btnSquare,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClick()));
    connect(ui->btnPercentage,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClick()));
    connect(ui->btnSqrt,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClick()));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_btnPoint_clicked()
{
    if(!operand.contains("."))
        operand+=qobject_cast<QPushButton*>(sender())->text();
    ui->display->setText(operand);
}


void MainWindow::on_btnDel_clicked()
{
    operand = operand.left(operand.length()-1);
    ui->display->setText(operand);
}


void MainWindow::on_btnClearAll_clicked()
{
    operand.clear();
    operands.clear();
    opcodes.clear();
    ui->display->setText(operand);
}


void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    ui->display->setText("0");
}


void MainWindow::on_btnEquals_clicked()
{
    if (!operand.isEmpty()) {
        operands.push_back(operand);
        operand.clear();
    }
if (operands.size() >= 2 && !opcodes.isEmpty()) {
        QString result = calculate();
        ui->display->setText(result);

        operands.clear();
        opcodes.clear();
        operand = result;
    }
}


void MainWindow::btnNumClicked()
{
    QString digit=qobject_cast<QPushButton*>(sender())->text();
    if(digit=="0"&&operand=="0")
        digit="";
    if(operand=="0"&&digit!="0")
        operand=digit;
    else
        operand += digit;
    ui->display->setText(operand);
}

void MainWindow::btnUnaryOperatorClick()
{
    if (!operand.isEmpty()) {
        double rs =operand.toDouble();
        operand="";
        QString op = qobject_cast<QPushButton*>(sender())->text();
        if(op=="%")
        {
            rs /= 100.0;
        }else if(op=="1/x"){
            if(rs!=0)
                rs = 1.0/rs;
            else{
                ui->display->setText("除数不能为0");
                operand.clear();
                return;
            }
        }else if(op=="x²"){
            rs *=rs;
        }else if(op=="²√x"){
            rs=sqrt(rs);
        }else if(op=="+/-")
        {
            rs = -rs;
        }
        operand = QString::number(rs);
        ui->display->setText(operand);
    }
}

void MainWindow::btnBinaryOperatorClick()
{
    opcode =qobject_cast<QPushButton*>(sender())->text();
    if(!operand.isEmpty())
    {
        operands.push_back(operand);
        operand.clear();
    }
    opcodes.push_back(opcode);

    //计算前日志打印
    ui->statusbar->showMessage(QString("calculation is in progress : operands is %1,opcodes is %2")
                                   .arg(operands.size())
                                   .arg(opcodes.size()));

    if (operands.size() >= 2 && !opcodes.isEmpty()) {
        QString result = calculate();
        ui->display->setText(result);
        operands.push_back(result);
    }else{
        ui->statusbar->showMessage(QString("operands is %1,opcodes is %2").arg(operands.size()).arg(opcodes.size()));
    }
}



QString MainWindow::calculate(bool *ok)
{
    //安全判断
    if (operands.size() < 2 || opcodes.isEmpty()) {
        ui->statusbar->showMessage("计算出错");
        return operand.isEmpty() ? "0" : operand;
    }
    double result=0;
    QString op =opcodes.front();
    opcodes.pop_front();
    double n1 = operands.front().toDouble();
    operands.pop_front();
    double n2 = operands.front().toDouble();
    operands.pop_front();
    if (op == "+") {
        result = n1 + n2;
    } else if (op == "-") {
        result = n1 - n2;
    } else if (op == "×") {
        result = n1 * n2;
    } else if (op == "÷") {
        if (n2 == 0) {
            ui->display->setText("除数不能为0");
            return "除数不能为0";
        }
        result = n1 / n2;
    }
    // 计算后日志打印
    ui->statusbar->showMessage(
        QString("After Compute: operands=%1, opcodes=%2, result=%3")
            .arg(operands.size())
            .arg(opcodes.size())
            .arg(result)
        );
    return QString::number(result);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    foreach(auto btnKey,btnNums.keys())
    {
        if(event->key()==btnKey)
            btnNums[btnKey]->animateClick();
    }
}





