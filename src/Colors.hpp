#ifndef COLORS_HPP
# define COLORS_HPP
# include <sstream>
# include <string>

# define RESET "\001\e[0m\002"
# define BLACK "\001\e[30m\002"
# define RED "\001\e[31m\002"
# define GREEN "\001\e[32m\002"
# define YELLOW "\001\e[33m\002"
# define BLUE "\001\e[34m\002"
# define PURPLE "\001\e[35m\002"
# define CYAN "\001\e[36m\002"
# define WHITE "\001\e[37m\002"
# define DEFAULT "\001\e[39m\002"

# define BLACK_BOLD "\001\e[1;30m\002"
# define RED_BOLD "\001\e[1;31m\002"
# define GREEN_BOLD "\001\e[1;32m\002"
# define YELLOW_BOLD "\001\e[1;33m\002"
# define BLUE_BOLD "\001\e[1;34m\002"
# define PURPLE_BOLD "\001\e[1;35m\002"
# define CYAN_BOLD "\001\e[1;36m\002"
# define WHITE_BOLD "\001\e[1;37m\002"
# define DEFAULT_BOLD "\001\e[1;39m\002"

# define BG_BLACK "\001\e[40m\002"
# define BG_RED "\001\e[41m\002"
# define BG_GREEN "\001\e[42m\002"
# define BG_YELLOW "\001\e[43m\002"
# define BG_BLUE "\001\e[44m\002"
# define BG_PURPLE "\001\e[45m\002"
# define BG_CYAN "\001\e[46m\002"
# define BG_WHITE "\001\e[47m\002"
# define BG_DEFAULT "\001\e[49m\002"

# define BG_BLACK_BOLD "\001\e[1;40m\002"
# define BG_RED_BOLD "\001\e[1;41m\002"
# define BG_GREEN_BOLD "\001\e[1;42m\002"
# define BG_YELLOW_BOLD "\001\e[1;43m\002"
# define BG_BLUE_BOLD "\001\e[1;44m\002"
# define BG_PURPLE_BOLD "\001\e[1;45m\002"
# define BG_CYAN_BOLD "\001\e[1;46m\002"
# define BG_WHITE_BOLD "\001\e[1;47m\002"
# define BG_DEFAULT_BOLD "\001\e[1;49m\002"

template <typename T>
std::string color(T const &t, std::string const color) {
    std::ostringstream oss;
    oss << t;
    return (color + oss.str() + RESET);
}

#endif