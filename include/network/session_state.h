#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    enum morobox8_session_state
    {
        MOROBOX8_SESSION_CLOSED,
        MOROBOX8_SESSION_CREATING,
        MOROBOX8_SESSION_HOSTING,
        MOROBOX8_SESSION_JOINING,
        MOROBOX8_SESSION_JOINED,
        MOROBOX8_SESSION_LEAVING
    };

#ifdef __cplusplus
}
#endif
