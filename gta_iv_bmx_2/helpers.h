#pragma once

// Макрос для преобразования выражения в строку
#define STR(s) #s

// Внутренний макрос, который объединяет токены.
#define CONCAT_INNER(a, b) a ## b

// Внешний макрос, который сначала раскрывает a и b, а затем передает их во внутренний макрос.
#define CONCAT(a, b) CONCAT_INNER(a, b)

// Макрос для заполнения ненужных элементов структуры
#define PADDING(size) char CONCAT(padding_, __LINE__)[size]

// ToDo: fix it and use instead of PADDING
#define PADDING_TO_OFFSET(offset, _class, fromMember) \
    char CONCAT(padding_, __LINE__)[(offset) - (offsetof(_class, fromMember) + sizeof(fromMember))]

#define STATIC_ASSERT_EXPR(expr) static_assert(expr, "Failed: " STR(expr))
