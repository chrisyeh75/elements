#include "game.h"


//the board is 12 by 6, extra line on y coordinate to prevent overflowing when doing calculations
int state[13][6];

//level, score, score increment shown to user, coordinate of mouse down, and combo
int level, score, score_sinc, pt_mousedown_x, pt_mousedown_y, combo = 1;

//whether or not we are inserting tiles while tryig to remove tiles
bool delete_inserting;

//delete insert from, delete insert to (actual), delete insert to (drawn)
int difrom_x, difrom_y, dito_x, dito_y, disto_y;

//whether we are drawing combo animation, whether we have started timer for combo animation
bool drawCombo, started = false;

//vector of delete sequence
std::vector<DelSequence> del;

//keeping track of game state to perform different operations, current game state for switching in and out of tutorial
std::string gameState = "pregame", curgameState;

//universal timer
QTime t;

//timer for combo animation
QTime* combo_t;

//coordinates for where to show combo and score
int combo_show_x, combo_show_y;

/**
 * @brief Game::Game game constructor
 * game constructor,
 * load all pictures, start important timers, set rendering engine
 */
Game::Game()
{
    qsrand(QTime::currentTime().msec());

    for(int i = 0; i<9; ++i){
        for(int j = 0; j<6; ++j){
            state[i][j] = qrand()%5+1;
        }
    }
    if (! air.load(":/images/air.png") ) std::cout << "failed to load air" << std::endl;
    if (! earth.load(":/images/earth.png")) std::cout << "failed to load earth" << std::endl;
    if (! fire.load(":/images/fire.png")) std::cout << "failed to load fire" << std::endl;
    if (! lightning.load(":/images/lightning.png")) std::cout << "failed to load lightning" << std::endl;
    if (! solar.load(":/images/solar.png")) std::cout << "failed to load solar" << std::endl;
    if (! vapor.load(":/images/vapor.png")) std::cout << "failed to load vapor" << std::endl;
    if (! water.load(":/images/water.png")) std::cout << "failed to load water" << std::endl;
    if (! empty.load(":/images/empty.png")) std::cout << "failed to load empty" << std::endl;
    if (! background.load(":/images/background.png")) std::cout << "failed to load background" << std::endl;
    if (! left_arrow.load(":/images/left_arrow.png")) std::cout << "failed to load left arrow" << std::endl;
    if (! right_arrow.load(":/images/right_arrow.png")) std::cout << "failed to load right arrow" << std::endl;
    if (! down_arrow.load(":/images/down_arrow.png")) std::cout << "failed to load down arrow" << std::endl;
    if (! up_arrow.load(":/images/up_arrow.png")) std::cout << "failed to load up arrow" << std::endl;
    if (! blue_glow.load(":/images/blue_glow.png")) std::cout << "failed to load blue glow" << std::endl;
    if (! orange_glow.load(":/images/orange_glow.png")) std::cout << "failed to load orange glow" << std::endl;
    if (! purple_glow.load(":/images/purple_glow.png")) std::cout << "failed to load purple glow" << std::endl;
    if (! red_glow.load(":/images/red_glow.png")) std::cout << "failed to load red glow" << std::endl;

    //start universal timer
    t.start();
    level = 1;

    //start timer for rendering
    QTimer* timer = new QTimer();
    timer->start(0);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));

    //connect board growing signal
    QTimer* growth = new QTimer();
    growth->start(11000-1000*level);
    connect(growth, SIGNAL(timeout()), this, SLOT(grow()));

    //needed for mousemove to work
    setMouseTracking(true);
}


/**
 * @brief Game::grow increment board
 * incriment board after certain time, increment as well all variables in play to keep game working
 */
void Game::grow(){
    for(int i = 0; i< 6; ++i)
        if(state[11][i] != 0) gameState = "gameover";

    if(gameState != "gameover" && gameState != "pregame" && gameState != "tutorial"){
        for(int i = 0; i<6; ++i)
            boardInsert(0, i, qrand()%5+1);

        if(!del.empty()){
            for(int i = 0; i<del.size(); ++i){
                for(int j = 0; j<del[i].seq.size(); ++j){
                    ++del[i].seq[j].first;
                }
            }
        }
        if(delete_inserting){
            ++dito_x; ++dito_y;
            ++difrom_x; ++difrom_y;
        }
        ++mouse_down_board_y;}
}

/**
 * @brief paintWelcome paint beginning conditions
 * @param painter is painter reference used
 */
void paintWelcome(QPainter &painter){
    painter.setFont(QFont("Fantasy", 20));
    painter.setPen(QColor(0,0,0,128));
    painter.drawText(70, 200+(t.elapsed()%350)/70, "Press SPACE to begin");
}

/**
 * @brief Game::paintBackground paint background component
 * @param painter is painter used
 */
void Game::paintBackground(QPainter &painter){
    painter.drawPixmap(0,0,400,500, background);
    painter.setFont(QFont("Helvetica", 15));
    painter.setPen(QColor(83,39,215));
    if(level != 10)
        painter.drawText(330, 305, QString::number(level));
    else
        painter.drawText(330, 305, "max");
    painter.drawText(330 - 5 * ((int)log10(score)), 365, QString::number(score));

    QPen pen;
    pen.setWidth(4);
    pen.setColor(QColor(0,128,128,128));
    painter.setPen(pen);
    painter.drawRect(35,24,224,426);
}

/**
 * @brief Game::paintTiles main painting events
 * loop through depending on which game state we are on to do various different things
 *
 * @param painter is painter used
 */
void Game::paintTiles(QPainter &painter){
    QPixmap temp;

    //paint in the tiles
    for(int i = 0; i<12; ++i){
        for(int j = 0; j<6; ++j){
            switch(state[i][j]){
                case 0: continue;
                case 1: temp = earth; break;
                case 2: temp = fire; break;
                case 3: temp = water; break;
                case 4: temp = vapor; break;
                case 5: temp = air; break;
            }

            painter.drawPixmap(40+j*36,410-i*35,35,35,temp);
        }
    }

    //deleting things that need to be deleted with flickering animation
    if(!del.empty()){
        for(size_t i = 0; i<del.size(); ++i){
            if(del[i].t.elapsed() > 2000){
                for(int j = 0; j<del[i].seq.size(); ++j){
                    boardDelete(del[i][j].first, del[i][j].second);
                }
                del.erase(del.begin()+i);
                while(!clean(true)) clean(true);
                break;
            }
            else if(del[i].t.elapsed() > 1000 && del[i].score_added == false){
                score += del[i].score_inc;
                del[i].score_added = true;
            }
            else if(del[i].t.elapsed() % 300 > 150){
                assert(!del[i].seq.empty());
                for(int j = 0; j<del[i].seq.size(); ++j)
                    painter.drawPixmap(40+del[i][j].second*36,410-del[i][j].first*35,35,35,empty);
            }
        }
    } else {
        combo = 1;
    }


    //enlarge tiles of mouseover
    if(gameState == "ingame" || gameState == "mouse_down"){

      deleteEmptySpace();
      clean(false);


      QPixmap temp;

      switch(state[board_y][board_x]){
          case 0: break;
          case 1: temp = earth; break;
          case 2: temp = fire; break;
          case 3: temp = water; break;
          case 4: temp = vapor; break;
          case 5: temp = air; break;
      }
        painter.drawPixmap(40+board_x*36-4, 410-board_y*35-4, 43, 43, temp);
    }


    //paint gameover state
    else if(gameState == "gameover"){
        painter.setFont(QFont("Fantasy", 20));
        painter.setPen(QColor(31, 123, 169));
        painter.drawText(70, 200+(t.elapsed()%350)/70, "GAME OVER");
    }

    //paint tutorial state
    else if(gameState == "tutorial"){
        painter.fillRect(QRect(80, 110,200,260), QBrush(QColor(186, 221, 255)));
        painter.setFont(QFont("Fantasy", 10));
        painter.drawText(80,130, " - click and drag tiles to");
        painter.drawText(80,150, "   clear at least 3-connected tiles");
        painter.drawText(80,190, " - tiles will grow toward the");
        painter.drawText(80,210, "   top, when reach the top");
        painter.drawText(80,230, "   countdown of 3 seconds will");
        painter.drawText(80,250, "   start, if not cleared within");
        painter.drawText(80,270, "   time, gameover");

    }

    //draw combo and score added animation
    if(drawCombo){
        if(!started) {
            combo_t = new QTime();
            combo_t->start();
            started = true;

            if(score < 100) level = 1;
            else if(score >= 100 && score < 400) level = 2;
            else if(score >= 400 && score < 1000) level = 3;
            else if(score >= 1000 && score < 3000) level = 4;
            else if(score >= 3000 && score < 7000) level = 5;
            else if(score >= 7000 && score < 14000) level = 6;
            else if(score >= 14000 && score < 30000) level = 7;
            else if(score >= 30000 && score < 50000) level = 8;
            else if(score >= 50000 && score < 100000) level = 9;
            else if(score >= 100000) level = 10;

        }
        else if (combo_t->elapsed() > 1000){
            drawCombo = false;
            combo_t = new QTime();
            started = false;
        }


        if(combo != 1){
            painter.setFont(QFont("Fantasy", 20));
            painter.setPen(QColor(89, 111, 253, 255-(combo_t->elapsed()/10)));
            painter.drawText(combo_show_x, combo_show_y - (combo_t->elapsed()/100), "combo " + QString::number(combo));
        }

        painter.setFont(QFont("Fantasy", 10));
        painter.setPen(QColor(69, 79, 229, 255-(combo_t->elapsed()/10)));
        painter.drawText(combo_show_x + 20, combo_show_y - (combo_t->elapsed()/100) + 20, "+" + QString::number(score_sinc));


        if(combo_t->elapsed() < 1000){

            if(combo == 1)
            painter.drawPixmap((int)(combo_show_x + (double)(320-combo_show_x)*(pow((double)combo_t->elapsed()/1000, 3))),
                               (int)(combo_show_y + (double)(355-combo_show_y)*(pow((double)combo_t->elapsed()/1000, 3))),
                               25,25,blue_glow);

            if(combo == 2)
            painter.drawPixmap((int)(combo_show_x + (double)(320-combo_show_x)*(pow((double)combo_t->elapsed()/1000, 3))),
                               (int)(combo_show_y + (double)(355-combo_show_y)*(pow((double)combo_t->elapsed()/1000, 3))),
                               25,25,orange_glow);

            if(combo == 3)
            painter.drawPixmap((int)(combo_show_x + (double)(320-combo_show_x)*(pow((double)combo_t->elapsed()/1000, 3))),
                               (int)(combo_show_y + (double)(355-combo_show_y)*(pow((double)combo_t->elapsed()/1000, 3))),
                               25,25,red_glow);

            if(combo >= 4)
            painter.drawPixmap((int)(combo_show_x + (double)(320-combo_show_x)*(pow((double)combo_t->elapsed()/1000, 3))),
                                   (int)(combo_show_y + (double)(355-combo_show_y)*(pow((double)combo_t->elapsed()/1000, 3))),
                                   25,25,purple_glow);

        }

    }

    //if trying to insert tile (conceptually before the tile drop)
    if(delete_inserting){


        if(difrom_x < dito_x && difrom_y == disto_y)
            painter.drawPixmap(difrom_x*36+58, (11-difrom_y)*35+43, 40, 20, right_arrow);

        else if(difrom_x > dito_x && difrom_y == disto_y)
            painter.drawPixmap(dito_x*36+58, (11-disto_y)*35+43, 40, 20, left_arrow);

        else if(difrom_x == dito_x && difrom_y > disto_y)
            painter.drawPixmap(difrom_x*36+58, (11-difrom_y)*35+43, 20, 40, down_arrow);

        else if(difrom_x == dito_x && difrom_y < disto_y)
            painter.drawPixmap(dito_x*36+58, (11-disto_y)*35+43, 20, 40, up_arrow);

        if(!delete_existed(dito_y, dito_x)) {
                boardInsert(dito_y, dito_x, state[difrom_y][difrom_x]);
                boardDelete(difrom_y, difrom_x);
                while(!clean(true)) clean(true);
                delete_inserting = false;
        }
    }



}



/**
 * @brief Game::deleteEmptySpace fix any floating tiles
 * recursive function that fix floating tiles
 *
 */
void Game::deleteEmptySpace(){

    if(gameState == "mouse_down") return;

    for(int i = 0; i<11; ++i){
        for(int j = 0; j<6; ++j){
            if(state[i][j] == 0 && state[i+1][j] != 0 && !delete_existed(i+1, j)){
                state[i][j] = state[i+1][j];
                state[i+1][j] = 0;
                deleteEmptySpace();
            }
        }
    }
}


/**
 * @brief delete_existed check whether certain tile is in delete sequence
 * @param y board y position
 * @param x board x position
 * @return whether it is in delete sequence
 */
bool delete_existed(int y, int x){
    if(y > 11) return false;
    if(x > 5) return false;
    if(del.empty()) return false;
    for(int k = 0; k<del.size(); ++k){
        for(int l = 0; l<del[k].seq.size(); ++l){
            if(del[k][l].first == y && del[k][l].second == x)
                return true;
        }
    }
    return false;
}


/**
 * @brief Game::clean clean up board by eliminating three, four, five-in-a-rows
 * @param add_combo whether or not deleting will add to combo
 * @return whether or not there was thing to delete
 */
bool Game::clean(bool add_combo){
    deleteEmptySpace();

    DelSequence tdel;
    tdel.score_added = false;
    bool breaking = false;
    for(int i = 0; i<10; ++i){
        for(int j = 0; j<6; ++j){
            if(state[i][j] == 0) continue;
            if(delete_existed(i,j)) continue;

            if(state[i][j] == state[i+1][j] && state[i][j] == state[i+2][j]){
                int same = state[i][j];
                if(i<9 && state[i+3][j] == same){
                    tdel.add(i+3, j);
                    if(i<8 && state[i+4][j] == same) tdel.add(i+4, j);
                }
                tdel.add(i+2, j);
                tdel.add(i+1, j);
                tdel.add(i, j);
                breaking = true;
                break;
            }
        }
        if(breaking == true){
            breaking = false;
            break;
        }
    }

    for(int i = 0; i<12; ++i){
        for(int j = 0; j<4; ++j){
            if(state[i][j] == 0) continue;
            if(delete_existed(i,j)) continue;

            if(state[i][j] == state[i][j+1] && state[i][j] == state[i][j+2]){
                int same = state[i][j];
                if(j<3 && state[i][j+3] == same){
                    tdel.add(i, j+3);
                    if(j<2 && state[i][j+4] == same) tdel.add(i, j+4);
                }
                tdel.add(i, j+2);
                tdel.add(i, j+1);
                tdel.add(i, j);
                breaking = true;
                break;
            }
        }
        if(breaking == true){
            breaking = false;
            break;
        }
    }

    if(!tdel.seq.empty()){

        if(add_combo){
            ++combo;
        }

        bool existed = false;
        for(int i = 0; i<del.size(); ++i) {
            if(del[i] == tdel)
                 existed = true;
        }
        if(!existed){
            tdel.t.start();

            score_sinc = (int)(tdel.seq.size() * tdel.seq.size() * sqrt(level));
            if(combo > 1){
                score_sinc *= combo * combo;
            }
            tdel.score_inc = score_sinc;


            del.push_back(tdel);

        }

        combo_show_x = tdel.seq[0].second*36+40;
        combo_show_y = (11-tdel.seq[0].first)*35+25;
        drawCombo = true;
        return false;
    }
    return true;

}

/**
 * @brief Game::keyPressEvent key press event, pressing space will start game
 * @param event event handler
 */
void Game::keyPressEvent(QKeyEvent* event){
    if(event->key() == Qt::Key_Space && gameState == "pregame"){
        gameState = "ingame";
        while(!clean(false)) clean(false);
    }
}

/**
 * @brief Game::mousePressEvent mouse press event, clicking will change to mouse down game state
 * @param event event handler
 */
void Game::mousePressEvent(QMouseEvent *event){
    if(gameState == "ingame"){
        gameState = "mouse_down";

        pt_mousedown_x = event->x();
        pt_mousedown_y = event->y();
        mouse_down_board_x = board_x;
        mouse_down_board_y = board_y;
    }
}

/**
 * @brief Game::boardDelete delete tile at position x, y
 * board start with 0,0 to 11,5
 * @param y y position
 * @param x x position
 */
void Game::boardDelete(int y, int x){
    int i = y;
    if(y == 12){
        state[y][x] = 0;
        return;
    }
    while(true){
        state[i][x] = state[i+1][x];
        if(state[i][x] == 0)
          break;
        ++i;
    }
}

/**
 * @brief Game::boardInsert insert tile at position x, y
 * @param y y position
 * @param x x position
 * @param insert the tile to insert
 */
void Game::boardInsert(int y, int x, int insert){
    int temp, i = y;
    while(true){
        temp = state[i][x];
        state[i][x] = insert;
        insert = temp;
        ++i;
        if(i==12) return;
    }
}

/**
 * @brief Game::mouseReleaseEvent releasing mouse return to in game state
 * @param event event handler
 */
void Game::mouseReleaseEvent(QMouseEvent *event){
    if(gameState == "mouse_down"){
        delete_inserting = false;
        gameState = "ingame";
    }
}

/**
 * @brief Game::mouseMoveEvent changing mouse position while pressed will swap tiles swiped across
 * @param event event handler
 */
void Game::mouseMoveEvent(QMouseEvent *event){
    cursor_x = event->x();
    cursor_y = event->y();

    //if curser is in board
    if(cursor_x>=40 && cursor_x<=255 && cursor_y>=25 && cursor_y<=445){
        int x = cursor_x, y = cursor_y;
        x -= 40; y-=25;

        //if mouse is currently down
        if(gameState == "mouse_down" && (board_x != x/36 || board_y != 11-y/35)){
            int prev_x = board_x, prev_y = board_y;
            board_x = x/36; board_y = 11-y/35;

            //if the tile going into is deleting cannot swap
            if(!delete_existed(board_y, board_x) && !delete_existed(prev_y, prev_x)){
                    std::swap(state[board_y][board_x], state[prev_y][prev_x]);
                    while(!clean(false)) clean(false);
                    delete_inserting = false;
            } else {
                //this is for inserting tiles into space that are deleting, dito_y is
                //delete inserting y position (actual) while disto_y is the shown one for arrow drawing
                delete_inserting = true;
                difrom_x = prev_x, difrom_y = prev_y;
                dito_x = board_x; dito_y = board_y; disto_y = board_y;
                if(delete_existed(board_y-1, board_x)) {
                        dito_y = board_y-1;
                   if(delete_existed(board_y-2, board_x)) {
                       dito_y = board_y-2;
                        if(delete_existed(board_y-3, board_x)){
                            dito_y = board_y-3;
                            if(delete_existed(board_y-4, board_x)){
                                dito_y = board_y-4;
                            }
                      }
                   }
                }
                if(!delete_existed(board_y, board_x))
                    delete_inserting = false;
            }
        } else {
            board_x = x/36; board_y = 11-y/35;}




    } else {
        if(gameState == "mouse_down") {
            if(pt_mousedown_y-cursor_y > 35){
                grow();
                pt_mousedown_y -= 35;
            }
        }
    }

    //if go into tutorial area paint tutorial and pause game
    if(gameState != "tutorial")
        curgameState = gameState;
    if(cursor_x>=300 && cursor_x<=360 && cursor_y>=112 && cursor_y<=135){
        gameState = "tutorial";
    }else
        gameState = curgameState;
}

/**
 * @brief Game::paintInfo paint cursor and board information on top left
 * @param painter is painter used
 */
void Game::paintInfo(QPainter &painter){
    painter.setFont(QFont("Helvetica", 10));
    painter.setPen(QColor(0,0,0));
    painter.drawText(10,10,"Curosr x:" + QString::number(cursor_x) + " y: " + QString::number(cursor_y));
    painter.drawText(10,25,"Board x:" + QString::number(board_x) + " y: " + QString::number(board_y));
}

/**
 * @brief Game::paintEvent paint event that should be the main painting function
 * but really, we just create the painter here and send it to where the main painting events occur, paintTiles
 */
void Game::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    paintBackground(painter);
    paintTiles(painter);


    if(gameState == "pregame")
        paintWelcome(painter);

    paintInfo(painter);
}
