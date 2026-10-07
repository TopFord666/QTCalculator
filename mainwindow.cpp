#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化
    isNewInput = true;

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
    if (isNewInput) {
        operand = "0";
        isNewInput = false;
    }

    if(!operand.contains("."))
        operand+=qobject_cast<QPushButton*>(sender())->text();
    ui->display->setText(operand);
}


void MainWindow::on_btnDel_clicked()
{
    // 操作数为空时不处理，避免 left(-1) 异常
    if (operand.isEmpty()) {
        return;
    }
    operand = operand.left(operand.length()-1);
    // 删空后显示0
    if (operand.isEmpty()) {
        ui->display->setText("0");
        isNewInput = true;
        return;
    }
    ui->display->setText(operand);
}


void MainWindow::on_btnClearAll_clicked()
{
    operand.clear();
    operands.clear();
    opcodes.clear();
    isNewInput = true;
    ui->display->setText(operand);
}


void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    isNewInput = true;
    ui->display->setText("0");
}


void MainWindow::on_btnEquals_clicked()
{
    // 将当前输入的数字压入操作数栈
    if (!operand.isEmpty()) {
        operands.push_back(operand);
        operand.clear();
    }

    // 循环计算栈中所有剩余运算（左到右依次处理）
    while (operands.size() >= 2 && !opcodes.isEmpty()) {
        QString result = calculate();
        ui->display->setText(result);

        // 除零时已经清空所有状态，直接返回
        if (result == "除数不能为0") {
            operand.clear();
            isNewInput = true;
            return;
        }

        // 计算结果插入到栈首，作为下一次运算的左操作数
        // 使用 prepend 确保非交换运算（减、除）的顺序正确
        operands.prepend(result);
    }

    // 最终结果保存到 operand，清空操作数栈
    if (!operands.isEmpty()) {
        operand = operands.front();
        operands.clear();
    }
    isNewInput = true;
}


void MainWindow::btnNumClicked()
{
    //符号重复判断
    if (isNewInput) {
        operand.clear();
        isNewInput = false;
    }

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
                //清空
                operand.clear();
                operands.clear();
                opcodes.clear();
                isNewInput = true;
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
        // 一元运算结果是最终值，下次输入数字应重新开始
        isNewInput = true;
    }
}

void MainWindow::btnBinaryOperatorClick()
{
    opcode = qobject_cast<QPushButton*>(sender())->text();

    // 没有任何操作数和当前输入时，忽略运算符点击
    if (operands.isEmpty() && operand.isEmpty()) {
        return;
    }

    // 连续按运算符：替换最后一个运算符而非追加
    if (isNewInput && !opcodes.isEmpty()) {
        opcodes.pop();
        opcodes.push_back(opcode);
        ui->statusbar->showMessage(QString("替换运算符: %1").arg(opcode));
        return;
    }

    // 将当前输入的数字压入操作数栈
    if (!operand.isEmpty()) {
        operands.push_back(operand);
        operand.clear();
    }

    // 左到右模式：栈中已有两个操作数和一个运算符时，先计算前一步
    if (operands.size() >= 2 && !opcodes.isEmpty()) {
        QString result = calculate();
        if (result == "除数不能为0") {
            return;
        }
        ui->display->setText(result);
        // 计算结果作为下一个左操作数，继续参与后续运算
        operands.push_back(result);
    }

    // 压入新运算符
    opcodes.push_back(opcode);
    isNewInput = true;

    ui->statusbar->showMessage(QString("运算符入栈: operands=%1, opcodes=%2")
                                   .arg(operands.size())
                                   .arg(opcodes.size()));
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
            //清空
            operands.clear();
            opcodes.clear();
            operand.clear();
            isNewInput = true;
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
    // 数字键映射
    foreach (auto btnKey, btnNums.keys()) {
        if (event->key() == btnKey) {
            btnNums[btnKey]->animateClick();
            return;
        }
    }

    // 运算符键盘映射
    switch (event->key()) {
    case Qt::Key_Plus:
        ui->btnPlus->animateClick();
        break;
    case Qt::Key_Minus:
        ui->btnMinus->animateClick();
        break;
    case Qt::Key_Asterisk:
        ui->btnMultiplied->animateClick();
        break;
    case Qt::Key_Slash:
        ui->btnDivision->animateClick();
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        ui->btnEquals->animateClick();
        break;
    case Qt::Key_Backspace:
        ui->btnDel->animateClick();
        break;
    case Qt::Key_Period:
        ui->btnPoint->animateClick();
        break;
    case Qt::Key_Escape:
        ui->btnClearAll->animateClick();
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}