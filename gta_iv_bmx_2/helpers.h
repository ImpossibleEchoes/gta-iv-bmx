#pragma once

// Макрос для преобразования выражения в строку
#define STR(s) #s
// Макрос, который создает сообщение об ошибке
// Внутренний макрос, который объединяет токены.
#define CONCAT_INNER(a, b) a ## b

// Внешний макрос, который сначала раскрывает a и b,
// а затем передает их во внутренний макрос.
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define PADDING(size) char CONCAT(padding_, __LINE__)[size]

#define PADDING_TO_OFFSET(offset, _class, fromMember) \
    char CONCAT(padding_, __LINE__)[(offset) - (offsetof(_class, fromMember) + sizeof(fromMember))]

//#define PADDING_TO(OFFSET_TO_NEXT, PREV_MEMBER) \
//    int8_t CONCAT(padding_, __LINE__)[(OFFSET_TO_NEXT) - (offsetof(struct, PREV_MEMBER) + sizeof(PREV_MEMBER))]

#define STATIC_ASSERT_EXPR(expr) static_assert(expr, "Failed: " STR(expr))
