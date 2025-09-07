#include "error.hpp"
#include "../Common/common.hpp"

static const char* ERROR_STRINGS[ERROR_COUNT] = {
    stringify(ERROR_SUCCESS),
    stringify(ERROR_RESOURCE_NOT_FOUND),
    stringify(ERROR_RESOURCE_TOO_BIG),
    stringify(ERROR_NULL_PARAMETER),
    stringify(ERROR_INVALID_PARAMETER),
};

const char* error_str(Error error_code) {
    RUNTIME_ASSERT((error_code >= 0) && (error_code < ERROR_COUNT));

    return ERROR_STRINGS[error_code];
}