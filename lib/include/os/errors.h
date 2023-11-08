typedef enum {
    os_NoError = 0,
    os_InvalidArgument,
    os_InvalidCapability,
    os_IllegalOperation,
    os_RangeError,
    os_AlignmentError,
    os_FailedLookup,
    os_TruncatedMessage,
    os_DeleteFirst,
    os_RevokeFirst,
    os_NotEnoughMemory,

    /* This should always be the last item in the list
     * so it gives a count of the number of errors in the
     * enum.
     */
    os_NumErrors
} OsError;