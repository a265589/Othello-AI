#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

#define SEARCH_DEPTH 5
#define H_BASE 10000000

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int get_next_player(int player) {
    return 3 - player;
}

class Point {
public:
    int x, y;
   Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};


int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

const std::array<Point, 8> directions{ {
       Point(-1, -1), Point(-1, 0), Point(-1, 1),
       Point(0, -1), /*{0, 0}, */Point(0, 1),
       Point(1, -1), Point(1, 0), Point(1, 1)
   } };



class option
{
public:
    int alpha, beta;
     std::array<std::array<int, SIZE>, SIZE> board;
     std::array<int, 3> disc_count;

    option(std::array<std::array<int, SIZE>, SIZE> &board_)
        :board(board_),alpha(-H_BASE),beta(H_BASE)
    {
        disc_count.fill(0);

        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                disc_count[board[i][j]]++;
            }

        }



    }

    ~option()
    {

    }


    bool is_spot_valid(Point center, int cur_player) {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }

    int get_disc(Point p) {
        return board[p.x][p.y];
    }

    bool is_disc_at(Point p, int disc) {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }

    bool is_spot_on_board(Point p) {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }

      void flip_discs(Point center,int cur_player) {

         for (Point dir : directions) {

             // Move along the direction while testing.

             Point p = center + dir;

             if (!is_disc_at(p, get_next_player(cur_player)))

                 continue;

             std::vector<Point> discs({ p });

             p = p + dir;

             while (is_spot_on_board(p) && get_disc(p) != EMPTY) {

                 if (is_disc_at(p, cur_player)) {

                     for (Point s : discs) {

                         set_disc(s, cur_player);

                     }

                      disc_count[cur_player] += discs.size();

                     disc_count[get_next_player(cur_player)] -= discs.size();

                     break;

                 }

                 discs.push_back(p);

                 p = p + dir;

             }

         }

     }

    void set_disc(Point p, int disc) {

        board[p.x][p.y] = disc;
    }

    int evaluate()
    {
        return  disc_count[player] * 10 - disc_count[get_next_player(player)] * 10;
    }

};



int minimax(option now, int depth, bool MAXIMIZE, int cur_player)
{

    std::vector<Point> next_possible_step;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
            if (now.board[i][j] != EMPTY)
                continue;
            if (now.is_spot_valid(p, cur_player))
                next_possible_step.push_back(p);
        }
    }


    for (auto p : next_possible_step)
    {
        option next = now;

        next.flip_discs(p, cur_player);

        if (depth == 0)
        {
            return minimax(next, depth + 1, !MAXIMIZE, get_next_player(cur_player));
        }
        else if (depth == SEARCH_DEPTH)
        {

         
            return now.evaluate();
           
        }
        else if (MAXIMIZE == true)
        {

            int H_max = -H_BASE;

            int H = minimax(next, depth + 1, !MAXIMIZE, get_next_player(cur_player));

            if (H > H_max)
            {
                H_max = H;
            }

            return H_max;
        }
        else if (MAXIMIZE == false)
        {

            int H_min = H_BASE;


            int H = minimax(next, depth + 1, !MAXIMIZE, get_next_player(cur_player));

            if (H < H_min)
            {
                H_min = H;
            }

            return H_min;

        }
    }
}





int alpha_beta_pruning(option& now, int depth, bool MAXIMIZE, int cur_player)
{


    std::vector<Point> next_possible_step;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
            if (now.board[i][j] != EMPTY)
                continue;
            if (now.is_spot_valid(p, cur_player))
                next_possible_step.push_back(p);
        }
    }


    for (auto p : next_possible_step)
    {
        option next = now;

        if (now.alpha >= now.beta)
        {
            break;
        }


        next.flip_discs(p, cur_player);

        if (depth == 0)
        {
            return alpha_beta_pruning(next, depth + 1, !MAXIMIZE, get_next_player(cur_player));
        }
        else if (depth == SEARCH_DEPTH)
        {
            return now.evaluate();
        }
        else if (MAXIMIZE == true)
        {

            int H_max = -H_BASE;

            int H = alpha_beta_pruning(next, depth + 1, !MAXIMIZE, get_next_player(cur_player));

            if (H > H_max)
            {
                 now.alpha = H_max = H;
            }

            return H_max;
        }
        else if (MAXIMIZE == false)
        {

            int H_min = H_BASE;


            int H = alpha_beta_pruning(next, depth + 1, !MAXIMIZE, get_next_player(cur_player));

            if (H < H_min)
            {
               now.beta  = H_min = H;
            }

            return H_min;

        }
    }
}



void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({ x, y });
    }
}



void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    //srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    //int index = (rand() % n_valid_spots);
    int max_H = -H_BASE;
    int next_step_id = 0;

   

    for (int i = 0; i < n_valid_spots; i++)
    {
        Point p = next_valid_spots[i];

        option start (board);

        start.flip_discs(p, player);

        start.alpha = max_H;

        int H = minimax(start, 0, false, get_next_player(player));

        if ( H > max_H)
        {
             max_H = H;
             next_step_id = i;

        }

    }

    Point p = next_valid_spots[next_step_id];

   
   
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;

    fout.flush();
}


int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}









