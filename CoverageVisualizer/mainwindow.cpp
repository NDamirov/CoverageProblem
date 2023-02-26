#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);
    connect(ui->Button, &QPushButton::released, this, &MainWindow::Redraw);
}

void MainWindow::Redraw()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "~");
    if (fileName.isEmpty()) {
        return;
    }
    std::ifstream fin(fileName.toStdString());
    std::string s;
    std::vector<std::string> input;
    while (std::getline(fin, s)) {
        if (s.empty()) {
            break;
        }
        input.push_back(s);
    }
    if (input.empty()) {
        return;
    }
    int height = input.size();
    int width = input[0].size();
    ui->graphicsView->clearMask();
    int block_height = ui->graphicsView->height() / height;
    int block_width = ui->graphicsView->width() / width;
    int block_size = std::min(block_height, block_width);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int ch = block_size * i;
            int cw = block_size * j;
            if (input[i][j] == '|') {
                scene->addRect(QRectF(cw, ch, block_size, block_size), QPen(Qt::black), QBrush(Qt::red));
            } else if (input[i][j] == '-') {
                scene->addRect(QRectF(cw, ch, block_size, block_size), QPen(Qt::black), QBrush(Qt::green));
            } else {
                scene->addRect(QRectF(cw, ch, block_size, block_size), QPen(Qt::black), QBrush(Qt::black));
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

