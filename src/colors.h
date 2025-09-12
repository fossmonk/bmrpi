#ifndef CSS_COLORS_H
#define CSS_COLORS_H

/**
 * @file css_colors.h
 * @brief This header file defines common CSS color names as hexadecimal integer values.
 *
 * Each color is defined using a preprocessor macro to make it easily accessible
 * in C/C++ projects that need to work with color values. The format is a 
 * 6-digit hexadecimal number, where the first two digits represent Red, the
 * next two represent Green, and the last two represent Blue (RRGGBB).
 */

// Basic Colors
#define CSS_BLACK        (0xFF000000)
#define CSS_SILVER       (0xFFC0C0C0)
#define CSS_GRAY         (0xFF808080)
#define CSS_WHITE        (0xFFFFFFFF)
#define CSS_MAROON       (0xFF800000)
#define CSS_RED          (0xFFFF0000)
#define CSS_PURPLE       (0xFF800080)
#define CSS_FUCHSIA      (0xFFFF00FF)
#define CSS_GREEN        (0xFF008000)
#define CSS_LIME         (0xFF00FF00)
#define CSS_OLIVE        (0xFF808000)
#define CSS_YELLOW       (0xFFFFFF00)
#define CSS_NAVY         (0xFF000080)
#define CSS_BLUE         (0xFF0000FF)
#define CSS_TEAL         (0xFF008080)
#define CSS_AQUA         (0xFF00FFFF)

// Extended Colors
#define CSS_ORANGE       (0xFFFFA500)
#define CSS_GOLD         (0xFFFFD700)
#define CSS_INDIGO       (0xFF4B0082)
#define CSS_VIOLET       (0xFFEE82EE)
#define CSS_TURQUOISE    (0xFF40E0D0)
#define CSS_CYAN         (0xFF00FFFF)
#define CSS_SALMON       (0xFFFA8072)
#define CSS_CORAL        (0xFFFF7F50)
#define CSS_TOMATO       (0xFFFF6347)
#define CSS_ORCHID       (0xFFDA70D6)
#define CSS_HOTPINK      (0xFFFF69B4)
#define CSS_CRIMSON      (0xFFDC143C)
#define CSS_CHOCOLATE    (0xFFD2691E)
#define CSS_SIENNA       (0xFFA0522D)
#define CSS_PEACHPUFF    (0xFFFFDAB9)
#define CSS_LAVENDER     (0xFFE6E6FA)

#define NUM_COLORS       (32)

#endif // CSS_COLORS_H