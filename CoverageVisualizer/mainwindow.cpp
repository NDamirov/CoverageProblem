#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <fstream>
#include <vector>
#include <string>
#include <iostream>

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
    std::ifstream fin("/Users/namigdamirov/Documents/HSE/CoverageProblem/build-CoverageVisualizer-Qt_6_4_1_for_macOS-Debug/out.txt");
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
    std::cout << "Height: " << block_height << " " << height << std::endl;
//    for (int i = 0; i < height; ++i) {
//        for (int j = 0; j < width; ++j) {
//            int ch = block_height * i;
//            int cw = block_width * i;
//            QGraphicsRectItem *rect = new QGraphicsRectItem(ch, cw, ch + block_height, cw + block_width);
//            if (input[i][j] == '|') {
//                scene->addRect(QRectF(ch, cw, ch + block_height, cw + block_width), QPen());
//                rect->setBrush(Qt::red);
//            } else if (input[i][j] == '-') {
//                rect->setBrush(Qt::green);
//            } else {
//                rect->setBrush(Qt::black);
//            }
//            scene->addItem(rect);
//        }
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

