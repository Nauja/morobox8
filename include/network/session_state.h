#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    enum moronet8_session_state
    {
        MORONET8_SESSION_CLOSED,
        MORONET8_SESSION_CREATING,
        MORONET8_SESSION_HOSTING,
        MORONET8_SESSION_JOINING,
        MORONET8_SESSION_JOINED,
        MORONET8_SESSION_LEAVING
    };

#ifdef __cplusplus
}
#endif
