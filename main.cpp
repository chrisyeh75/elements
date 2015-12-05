/*Christopher Yeh
 * This game works by moving the tile around, match tile horizontally or vertically with three or more to cancel
 * background image from
 * http://images4.fanpop.com/image/photos/19300000/Tha-Last-Airbender-avatar-the-last-airbender-19378955-800-1200.png
 * based on anime Avatar the last airbender and game tetris attack
 * */

#include "game.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Game* g = new Game();
    w.setFixedSize(400,500);
    w.setCentralWidget(g);
    g->activateWindow();
    g->setFocus();

    w.show();
    return a.exec();
}
