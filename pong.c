#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define BALL "o"
#define GRID "|"
#define PLAYER "|"
#define H_WALL "-"
#define V_WALL "|"

#define FIELD_LENGTH 80
#define FIELD_WIDTH 25
#define SLEEP_TIME 90000

#define LEFT_UP 2
#define RIGHT_UP 1
#define LEFT_DOWN -2
#define RIGHT_DOWN -1

#define BALL_ON_LEFT_PLAYER 4
#define BALL_ON_RIGHT_PLAYER 76
#define BALL_ON_UPPER_BORDER 2
#define BALL_ON_LOWER_BORDER 24
#define BALL_IN_LOWER_LEFT_CORNER -1
#define BALL_IN_UPPER_LEFT_CORNER -2
#define BALL_IN_LOWER_RIGHT_CORNER -3
#define BALL_IN_UPPER_RIGHT_CORNER -4

void main_printer();
void win_printer(int winner);
void score_printer(int left_player_point, int right_player_point, int command);
void printer(int left_player_y_position, int right_player_y_position, int ball_x_position,
             int ball_y_position);

int test_ball_position(int x, int y);
int player_position(int player_y_position);
int end_of_game_test(int left_player_point, int right_player_point);
int initial_window(int left_player_y_position, int right_player_y_position,
                   int ball_x_position, int ball_y_position);

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    if (initial_window(12, 12, 4, 12) == 0) {
        clear();
        endwin();
        return 0;
    }

    main_printer();
    endwin();
    return 0;
}

void main_printer() {
    int left_player_point = 0, right_player_point = 0;
    int left_player_y_position = 12, right_player_y_position = 12;
    int ball_x_position = 4, ball_y_position = 12, ball_direction = RIGHT_UP;
    int print_counter = -1;

    while (1) {
        int scored_a_goal = 0;
        char input = getch();
        /*
         scored_a_goal = 0 - Никто не забил.
         scored_a_goal = 1 - Забил левый игрок.
         scored_a_goal = 2 - Забил правый игрок.
         */

        if (input == 'a' || input == 'A') {
            left_player_y_position--;
        } else if (input == 'z' || input == 'Z') {
            left_player_y_position++;
        } else if (input == 'k' || input == 'K') {
            right_player_y_position--;
        } else if (input == 'm' || input == 'M') {
            right_player_y_position++;
        }
        usleep(SLEEP_TIME);
        /* Проверка на выход за границы игроков. */
        left_player_y_position = player_position(left_player_y_position);
        right_player_y_position = player_position(right_player_y_position);

        switch (ball_direction) {
            case RIGHT_UP:
                ball_x_position++;
                ball_y_position--;

                switch (test_ball_position(ball_x_position, ball_y_position)) {
                    case BALL_ON_RIGHT_PLAYER:
                        /* Попали на игрока. */
                        if (ball_y_position >= right_player_y_position - 1 &&
                            ball_y_position <= right_player_y_position + 1) {
                            /* Попадание в правый верхний угол. (частный случай) */
                            if (ball_y_position == 2) {
                                ball_direction = LEFT_DOWN;
                            } else {
                                ball_direction = LEFT_UP;
                            }
                        } else {
                            left_player_point++;
                            scored_a_goal = 1;
                        }
                        break;
                    case BALL_ON_UPPER_BORDER:
                        ball_direction = RIGHT_DOWN;
                        break;
                }
                break;

            case RIGHT_DOWN:
                ball_x_position++;
                ball_y_position++;

                switch (test_ball_position(ball_x_position, ball_y_position)) {
                    case BALL_ON_RIGHT_PLAYER:
                        /* Попали на игрока. */
                        if (ball_y_position >= right_player_y_position - 1 &&
                            ball_y_position <= right_player_y_position + 1) {
                            /* Попадание в правый нижний угол. (частный случай) */
                            if (ball_y_position == BALL_ON_LOWER_BORDER) {
                                ball_direction = LEFT_UP;
                            } else {
                                ball_direction = LEFT_DOWN;
                            }
                        } else {
                            left_player_point++;
                            scored_a_goal = 1;
                        }
                        break;
                    case BALL_ON_LOWER_BORDER:
                        ball_direction = RIGHT_UP;
                        break;
                }
                break;

            case LEFT_UP:
                ball_x_position--;
                ball_y_position--;

                switch (test_ball_position(ball_x_position, ball_y_position)) {
                    case BALL_ON_LEFT_PLAYER:
                        /* Попали на игрока. */
                        if (ball_y_position >= left_player_y_position - 1 &&
                            ball_y_position <= left_player_y_position + 1) {
                            /* Попадание в левый верхний угол. (частный случай) */
                            if (ball_y_position == 2) {
                                ball_direction = RIGHT_DOWN;
                            } else {
                                ball_direction = RIGHT_UP;
                            }
                        } else {
                            right_player_point++;
                            scored_a_goal = 2;
                        }
                        break;
                    case BALL_ON_UPPER_BORDER:
                        ball_direction = LEFT_DOWN;
                        break;
                }
                break;

            case LEFT_DOWN:
                ball_x_position--;
                ball_y_position++;

                switch (test_ball_position(ball_x_position, ball_y_position)) {
                    case BALL_ON_LEFT_PLAYER:
                        /* Попали на игрока. */
                        if (ball_y_position >= left_player_y_position - 1 &&
                            ball_y_position <= left_player_y_position + 1) {
                            /* Попадание в левый нижний угол. (частный случай) */
                            if (ball_y_position == 24) {
                                ball_direction = RIGHT_UP;
                            } else {
                                ball_direction = RIGHT_DOWN;
                            }
                        } else {
                            right_player_point++;
                            scored_a_goal = 2;
                        }
                        break;
                    case BALL_ON_LOWER_BORDER:
                        ball_direction = LEFT_UP;
                        break;
                }
                break;
        }

        if (scored_a_goal != 0) {
            /* Конец игры. */
            if (end_of_game_test(left_player_point, right_player_point) == 1) {
                if (initial_window(12, 12, 4, 12) == 0) {
                    break;
                } else {
                    return main_printer();
                }
            } else {
                if (scored_a_goal == 2) {
                    ball_x_position = 76;
                    ball_y_position = right_player_y_position;
                } else {
                    ball_x_position = 4;
                    ball_y_position = left_player_y_position;
                }
                score_printer(left_player_point, right_player_point, 1);
                printer(left_player_y_position, right_player_y_position, ball_x_position, ball_y_position);
                print_counter = 1;
            }
        } else {
            /* Активировна надпись "GOAL!" */
            if (print_counter > 0) {
                score_printer(left_player_point, right_player_point, 1);
                printer(left_player_y_position, right_player_y_position, ball_x_position, ball_y_position);
                print_counter++;
                if (print_counter == 10) {
                    print_counter = -1;
                }
            } else {
                score_printer(left_player_point, right_player_point, 0);
                printer(left_player_y_position, right_player_y_position, ball_x_position, ball_y_position);
            }
        }
    }
}

int end_of_game_test(int left_player_point, int right_player_point) {
    if (left_player_point == 21 || right_player_point == 21) {
        /* Конец игры. */
        if (left_player_point == 21) {
            win_printer(0);
        } else {
            win_printer(1);
        }
        return 1;
    }
    return 0;
}

void score_printer(int left_player_point, int right_player_point, int command) {
    clear();

    if (command == 0) {
        printw("                                 %d           %d\n", left_player_point, right_player_point);
    } else {
        printw("                                      GOAL!\n");
    }
    refresh();
}

void printer(int left_player_y_position, int right_player_y_position, int ball_x_position,
             int ball_y_position) {
    /* print_flag - Флаг, который указывает, была ли уже ранее совершена отрисовка. */
    for (int y = 1; y <= 25; y++) {
        for (int x = 1; x <= 80; x++) {
            int print_flag = 0;
            /* Отрисовка горизонтальных стен. */
            if (y == 1 || y == 25) {
                printw("%s", H_WALL);
                print_flag = 1;
            } else {
                /* Отрисовка боковых стен. */
                if (x == 1 || x == 80) {
                    printw("%s", V_WALL);
                    print_flag = 1;
                } else if (x == ball_x_position && y == ball_y_position) {
                    /* Отрисовка мяча. */
                    printw("%s", BALL);
                    print_flag = 1;
                } else if (x == 40) {
                    printw("%s", GRID);
                    print_flag = 1;
                } else {
                    /* Отрисовка игроков. */
                    if ((x == 3 && (y >= left_player_y_position - 1) && (y <= left_player_y_position + 1)) ||
                        (x == 77 && (y >= right_player_y_position - 1) &&
                         (y <= right_player_y_position + 1))) {
                        printw("%s", PLAYER);
                        print_flag = 1;
                    }
                }
            }

            if (print_flag == 0) {
                printw(" ");
            }
            if (x == 80) {
                printw("\n");
            }
        }
    }
    refresh();
}

void win_printer(int winner) {
    clear();
    /* Победа левого игрока. */
    if (winner == 0) {
        printw(
            "\n--------------------------------------------------------------------------------\n|           "
            "                           |                                       |\n|                         "
            "             |                                       |\n|                                      "
            "|                                       |\n|                                      |             "
            "                          |\n|                                      |                           "
            "            |\n|                                      |                                       "
            "|\n|                                      |                                       |\n|          "
            "                            |                                       |\n|                        "
            "              |                                       |\n| |                                    "
            "|                                    |  |\n| | o                           Left player won!     "
            "                       |  |\n| |                                    |                           "
            "         |  |\n|                                      |                                       "
            "|\n|                                      |                                       |\n|          "
            "                            |                                       |\n|                        "
            "              |                                       |\n|                                      "
            "|                                       |\n|                                      |             "
            "                          |\n|                                      |                           "
            "            |\n|                                      |                                       "
            "|\n|                                      |                                       |\n|          "
            "                            |                                       |\n|                        "
            "              |                                       "
            "|\n--------------------------------------------------------------------------------");
    } else {
        /* Победа правого игрока. */
        printw(
            "\n--------------------------------------------------------------------------------\n|           "
            "                           |                                       |\n|                         "
            "             |                                       |\n|                                      "
            "|                                       |\n|                                      |             "
            "                          |\n|                                      |                           "
            "            |\n|                                      |                                       "
            "|\n|                                      |                                       |\n|          "
            "                            |                                       |\n|                        "
            "              |                                       |\n| |                                    "
            "|                                    |  |\n| | o                           Right player won!    "
            "                       |  |\n| |                                    |                           "
            "         |  |\n|                                      |                                       "
            "|\n|                                      |                                       |\n|          "
            "                            |                                       |\n|                        "
            "              |                                       |\n|                                      "
            "|                                       |\n|                                      |             "
            "                          |\n|                                      |                           "
            "            |\n|                                      |                                       "
            "|\n|                                      |                                       |\n|          "
            "                            |                                       |\n|                        "
            "              |                                       "
            "|\n--------------------------------------------------------------------------------");
    }
    refresh();
    sleep(3);
}

int initial_window(int left_player_y_position, int right_player_y_position, int ball_x_position,
                   int ball_y_position) {
    char input;

    clear();

    printw(
        "\n--------------------------------------------------------------------------------\n|               "
        "                       |                                       |\n|                                 "
        "     |                                       |\n|                                      |            "
        "                           |\n|                                      |                              "
        "         |\n|                                      |                                       |\n|     "
        "                                 |                                       |\n|                       "
        "               |                                       |\n|                                      |  "
        "                                     |\n|                               ----------------            "
        "                   |\n| |                            |                |                           | "
        " |\n| | o                          | N - New game.  |                           |  |\n| |           "
        "                 |                |                           |  |\n|                              "
        "| Q - End game.  |                              |\n|                              |                "
        "|                              |\n|                              -----------------                  "
        "             |\n|                                      |                                       |\n| "
        "                                     |                                       |\n|                   "
        "                   |                                       |\n|                                     "
        " |                                       |\n|                                      |                "
        "                       |\n|                                      |                                  "
        "     |\n|                                      |                                       |\n|         "
        "                             |                                       "
        "|\n--------------------------------------------------------------------------------");
    refresh();

    input = getchar();

    if (input == 'n' || input == 'N') {
        clear();
        score_printer(0, 0, 0);
        printer(left_player_y_position, right_player_y_position, ball_x_position, ball_y_position);
        return 1;
    } else if (input == 'Q' || input == 'q') {
        clear();
        return 0;
    } else {
        return initial_window(left_player_y_position, right_player_y_position, ball_x_position,
                              ball_y_position);
    }
}

int player_position(int player_y_position) {
    /* Проверка на то, что игрок не зайдет за границы. */
    if (player_y_position > 23) {
        player_y_position = 23;
    } else if (player_y_position < 3) {
        player_y_position = 3;
    }
    return player_y_position;
}

int test_ball_position(int x, int y) {
    /* Мяч находится вплотную к координате x левого игрока. */
    if (x == BALL_ON_LEFT_PLAYER) {
        return x;
        /* Мяч находится вплотную к координате x правого игрока. */
    } else if (x == BALL_ON_RIGHT_PLAYER) {
        return x;
    }
    /* Мяч у горизонтальной стены. */
    if (y == BALL_ON_UPPER_BORDER) {
        return y;
    } else if (y == BALL_ON_LOWER_BORDER) {
        return y;
    }

    return 0;
}
