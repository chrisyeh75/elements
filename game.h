#ifndef GAME
#define GAME
#include <QWidget>
#include <QtGui>
#include <QPainter>
#include <QObject>
#include <vector>
#include <utility>
#include <string>
#include <QTime>
#include <QTimer>
#include <QKeyEvent>
#include <cassert>
#include <cmath>
#include <QApplication>
#include <iostream>
#include "mainwindow.h"

bool delete_existed(int y, int x);
void animate_glow(int x, int y, QTime* t, QPixmap& glow, QPainter& painter);


/**
 * @brief The DelSequence class the delete sequence class, class that stores list of tiles to be deleted
 */
class DelSequence{
    public:
        //timer for how long since need to be deleted, we delete after 2 seconds
        QTime t;
        //whether the score has been added, score relevent to this deletion
        bool score_added;
        int score_inc;
        //a vector of pairs to store tiles to be deleted, the pair store x and y position
        std::vector< std::pair<int, int> > seq;

        /**
         * @brief DelSequence::add add x and y position to sequence
         * @param x x position
         * @param y y position
         */
        void add(int x, int y){
            if(find(seq.begin(), seq.end(), std::make_pair(x, y)) == seq.end())
                seq.push_back( std::make_pair(x, y) );
        }

        /**
         * @brief DelSequence::operator [] directly access elements
         * @param a element to be accessed
         * @return a pair for the x and y position
         */

        std::pair<int, int> operator[](int a){
            return seq[a];
        }

        /**
         * @brief DelSequence::operator == whether two sequence is same
         * @param d2 the other sequence to be compared
         * @return whether they are the same
         */
        bool operator==(DelSequence d2){
            return (d2.seq == seq);
        }
};

/**
 * @brief The Game class the main class of game, using mainly QPainter to do all painting
 */
class Game:public QWidget{
    public:
        Game();
        void deleteEmptySpace();
        void keyPressEvent(QKeyEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);

    public slots:
        void grow();

    protected:
        void paintEvent(QPaintEvent *);
    private:
        bool clean(bool add_combo);
        void boardDelete(int y, int x);
        void boardInsert(int y, int x, int insert);
        void paintTiles(QPainter &painter);
        void paintInfo(QPainter &painter);
        void paintBackground(QPainter &painter);
        QPixmap air, earth, fire, lightning, solar, vapor, water, empty, background, right_arrow, left_arrow,
            up_arrow, down_arrow, blue_glow, orange_glow, purple_glow, red_glow;
        int cursor_x, cursor_y, board_x = 0, board_y = 0, mouse_down_board_x, mouse_down_board_y;
        Q_OBJECT;
};





#endif // GAME

