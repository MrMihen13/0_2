#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cmath>

class Test {
private:
    std::stringbuf sbuf;
    std::streambuf* oldbuf;
    int n;

    void update_state() {
        this->n += 1;
        this->sbuf = std::stringbuf(std::ios::out);
        this->oldbuf = std::cout.rdbuf(std::addressof(this->sbuf));
    }
public:
    Test() {
        this->update_state();
    };
    void out_stream_to_be(std::string expect) {

        std::cout.rdbuf(this->oldbuf);

        std::string output = this->sbuf.str();

        if (output == expect) {
            std::cout << "Тест " << this->n << " пройден\n";
        }
        else {
            std::cout << "Тест " << this->n << " не пройден.\n" << "Ожидаемый результат: " << expect << "\n" << "Полученный результат: \n" << output << "\n";
        }
        this->update_state();
    }
};


enum class Quadrant { first, second, third, fourth, first_second, second_third, third_fourth, fourth_first, zero };

inline Quadrant get_point_quadrant(std::vector<double> point) {
    double x = point[0];
    double y = point[1];

    if (x > 0 && y > 0) {
        return Quadrant::first;
    }
    if (x > 0 && y < 0) {
        return Quadrant::second;
    }
    if (x < 0 && y < 0) {
        return Quadrant::third;
    }
    if (x > 0 && y < 0) {
        return Quadrant::fourth;
    }
    if (x == 0 && y == 0) {
        return Quadrant::zero;
    }
    if (y == 0 && x > 0) {
        return Quadrant::first_second;
    }
    if (x == 0 && y < 0) {
        return Quadrant::second_third;
    }
    if (y == 0 && x < 0) {
        return Quadrant::third_fourth;
    }
    return Quadrant::fourth_first;
}

std::vector<std::vector<double>> get_intersection_points(std::vector<double> previous, std::vector<double> current) {
    double previous_x = previous[0];
    double previous_y = previous[1];

    double current_x = current[0];
    double current_y = current[1];

    double intersection_x = -previous_y * (current_x - previous_x) / (current_y - previous_y) + previous_x;
    double intersection_y = -previous_x * (current_y - previous_y) / (current_x - previous_x) + previous_y;

    return std::vector<std::vector<double>> { { intersection_x, 0. }, { 0., intersection_y } };
}

double get_side_width(std::vector<double> previous, std::vector<double> current) {
    double previous_x = previous[0];
    double previous_y = previous[1];

    double current_x = current[0];
    double current_y = current[1];

    double dx = pow(current_x - previous_x, 2.);
    double dy = pow(current_y - previous_y, 2.);

    return pow(dx + dy, 1. / 2.);
}

std::vector<double> get_sides_width(std::vector<std::vector<double>> cords) {
    std::vector<double> sides_width;
    int sides = cords.size();
    for (int i = 0; i < sides; i++) {
        std::vector<double> previous_point = i == 0 ? cords[sides - 1] : cords[i - 1];
        sides_width.push_back(get_side_width(previous_point, cords[i]));
    }
    return sides_width;
}

double get_square(std::vector<std::vector<double>> cords) {
    double square_coef = 1. / 2.;
    double positive_sum = cords[cords.size() - 1][0] * cords[0][1];
    double negative_sum = cords[cords.size() - 1][1] * cords[0][0];
    for (int i = 0; i < cords.size() - 1; i++) {
        positive_sum += cords[i][0] * cords[i + 1][1];
        negative_sum += cords[i + 1][0] * cords[i][1];
    }

    return square_coef * std::abs(static_cast<int>(positive_sum - negative_sum));
}

bool is_first(std::vector<double> point) {
    auto quadrant = get_point_quadrant(point);
    return quadrant == Quadrant::first || quadrant == Quadrant::first_second || quadrant == Quadrant::fourth_first;
}

bool is_second(std::vector<double> point) {
    auto quadrant = get_point_quadrant(point);
    return quadrant == Quadrant::second || quadrant == Quadrant::first_second;
}

bool is_third(std::vector<double> point) {
    auto quadrant = get_point_quadrant(point);
    return quadrant == Quadrant::second_third || quadrant == Quadrant::third || quadrant == Quadrant::third_fourth || quadrant == Quadrant::zero;
}

bool is_fouth(std::vector<double> point) {
    auto quadrant = get_point_quadrant(point);
    return quadrant == Quadrant::fourth || quadrant == Quadrant::fourth_first;
}

std::vector<double> get_lines_intersections(std::vector<double> current, std::vector<double> next) {
    auto intersection_points = get_intersection_points(current, next);
    auto x_point = intersection_points[0];
    auto y_point = intersection_points[1];

    bool is_intersection_on_y_axis = std::min(current[0], next[0]) < 0 && std::max(current[0], next[0]) >= 0;
    bool is_intersection_on_x_axis = std::min(current[1], next[1]) < 0 && std::max(current[1], next[1]) >= 0;

    std::vector<std::vector<double>> intersection_map = { x_point, y_point, x_point, y_point };

    return intersection_map[static_cast<int>((is_intersection_on_y_axis + is_intersection_on_x_axis) % intersection_map.size())];
}

int task(std::vector<std::vector<double>> rect)
{
    int sides = 4;
    int dimension = 2;

    std::vector<Quadrant>quadrant_map = { Quadrant::first, Quadrant::second, Quadrant::third, Quadrant::fourth };

    auto required_quadrant = Quadrant::third;

    Quadrant opposite_quadrant = quadrant_map[(static_cast<int>(required_quadrant) + 2) % quadrant_map.size()];
    
    int connection_with_opposite_quadrant = 0;

    std::vector<std::vector<double>> third_quadrant_rect;

    for (int i = 0; i < sides; i++) {
        auto point = rect[i];

        bool is_required = is_third(point);
        if (is_required) {
            third_quadrant_rect.push_back(point);
        }

        int next_index_point = i == sides - 1 ? 0 : i + 1;
        auto next_point = rect[next_index_point];

        bool is_next_point_required = is_third(next_point);
        if (is_next_point_required && is_required) {
            continue;
        }

        connection_with_opposite_quadrant += (get_point_quadrant(point) == opposite_quadrant || get_point_quadrant(next_point) == opposite_quadrant);

        
        auto intersection_point = get_lines_intersections(point, next_point);
        third_quadrant_rect.push_back(intersection_point);
    }

    if (connection_with_opposite_quadrant >= dimension) {
        third_quadrant_rect.push_back({ 0, 0 });
    }

    for (int i = 0; i < third_quadrant_rect.size(); i++) {
        auto point = third_quadrant_rect[i];
        auto x = point[0];
        auto y = point[1];
        if (x != x || y != y) {
            std::cout << "неверная фигура" << std::endl;
            return 1;
        }
    }

    std::vector<double> sides_width = get_sides_width(third_quadrant_rect);
    double perimeter = 0;
    for (int i = 0; i < third_quadrant_rect.size(); i++) {
        perimeter += sides_width[i];
    };
    double sqaure = get_square(third_quadrant_rect);
    std::cout << "Периметр=" << perimeter << " Площадь=" << sqaure << std::endl;
    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    Test test = Test();
    task({ {3,8}, {-8, -8}, {5, -10}, {-1, -7} });
    test.out_stream_to_be("Периметр=31.2624 Площадь=46\n");
    task({ {0,0}, {-4, 0}, {-4, -4}, {0, -4} });
    test.out_stream_to_be("Периметр=16 Площадь=16\n");
    task({ {0,0}, {4, 0}, {4, -4}, {0, 4} });
    test.out_stream_to_be("неверная фигура\n");
}
