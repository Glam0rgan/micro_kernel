typedef enum {
    OsNoError = 0,
    OsInvalidArgument,
    OsInvalidCapability,
    OsIllegalOperation,
    OsRangeError,
    OsAlignmentError,
    OsFailedLookup,
    OsTruncatedMessage,
    OsDeleteFirst,
    OsRevokeFirst,
    OsNotEnoughMemory,

    /* This should always be the last item in the list
     * so it gives a count of the number of errors in the
     * enum.
     */
    OsNumErrors
} OsError;