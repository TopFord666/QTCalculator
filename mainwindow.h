#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstack.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    QString operand;

    QString opcode;

    QStack<QString> operands;

    QStack<QString> opcodes;

    QString calculate(bool *ok=NULL);
private slots:
    void btnNumClicked();

    void btnBinaryOperatorClick();

    void btnUnaryOperatorClick();

    void on_btnPoint_clicked();

    void on_btnDel_clicked();

    void on_btnClearAll_clicked();

    void on_btnEquals_clicked();

    void on_btnClear_clicked();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
