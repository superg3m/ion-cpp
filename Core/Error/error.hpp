#pragma once

enum Error {
    ERROR_SUCCESS = 0,
    ERROR_RESOURCE_NOT_FOUND,
    ERROR_RESOURCE_TOO_BIG,
    ERROR_NULL_PARAMETER,
    ERROR_INVALID_PARAMETER,
    ERROR_COUNT
};

/**
 * @brief returns a string literal of the error code
 * 
 * @param error_code 
 * @return const char*
 */
const char* error_str(Error error_code);