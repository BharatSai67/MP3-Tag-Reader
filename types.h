#ifndef TYPES_H
#define TYPES_H

/* Status return type */
typedef enum
{
    success,
    failure
} Status;

/* Operation type */
typedef enum
{
    view,
    edit,
    help,
    unsupported
} OperationType;

#endif
