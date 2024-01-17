#include <msquic.h>

#include <memory>
#include <stdexcept>

namespace rvn::detail {

template <typename... Args>
QUIC_STATUS NoOpSuccess(Args... args) {
    return QUIC_STATUS_SUCCESS;
}

template <typename... Args>
void NoOpVoid(Args... args) {
    return;
};
template <typename Ctor, typename Dtor>
class unique_handler1 {
    // Pointer to QUIC_HANDLER owned by unique_handler1
    HQUIC handler;

    // non owning callable (function pointor)
    Ctor open;
    Dtor close;

    void destroy() {
        if (handler != NULL) close(handler);
    }

    void reset() {
        destroy();
        handler = NULL;
    }

   protected:
    unique_handler1(Ctor open_, Dtor close_) noexcept
        : handler(NULL) {
        open = open_;
        close = close_;
    }

    ~unique_handler1() noexcept { destroy(); }

    unique_handler1(const unique_handler1 &) = delete;
    unique_handler1 &operator=(const unique_handler1 &) = delete;

    unique_handler1(unique_handler1 &&rhs) {
        reset();
        // take ownership
        handler = rhs.handler;
        open = rhs.open;
        close = rhs.close;

        // RHS releases ownership
        rhs.handler = NULL;
    }

    /*Don't take universal reference as this is C.
      Everything is copied

      If return is QUIC_FAILED then throw exception
    */
    template <typename... Args>
    QUIC_STATUS construct(Args... args) {
        return open(args..., &handler);
    }

    HQUIC get() const { return handler; }
};

// To be used when only one construct function exists
template <typename Open, typename Close, typename Start,
          typename Stop = decltype(&NoOpVoid<HQUIC>)>
class unique_handler2 {
    // Pointer to QUIC_HANDLER owned by unique_handler2
    HQUIC handler;

    // non owning callable (function pointor)
    Open open_func;
    Close close_func;

    Start start_func;
    Stop stop_func;

    void destroy() {
        if (handler != NULL) {
            stop_func(handler);
            close_func(handler);
        };
    }

    void reset() {
        destroy();
        handler = NULL;
    }

   protected:
    unique_handler2(Open open_, Close close_, Start start_,
                    Stop stop_ = &NoOpVoid<HQUIC>) noexcept
        : handler(NULL) {
        open_func = open_;
        close_func = close_;

        start_func = start_;
        stop_func = stop_;
    }

    ~unique_handler2() noexcept { destroy(); }

    unique_handler2(const unique_handler2 &) = delete;
    unique_handler2 &operator=(const unique_handler2 &) = delete;

    unique_handler2(unique_handler2 &&rhs) {
        reset();
        // take ownership
        handler = rhs.handler;
        open_func = rhs.open_func;
        close_func = rhs.close_func;

        start_func = rhs.start_func;
        stop_func = rhs.stop_func;

        // RHS releases ownership
        rhs.handler = NULL;
    }

    /*Don't take universal reference as this is C interface.
      Everything is copied

      If return is QUIC_FAILED then throw exception
    */
    template <typename... Args>
    QUIC_STATUS open_handler(Args... args) {
        return open_func(args..., &handler);
    }

    template <typename... Args>
    QUIC_STATUS start_handler(Args... args) {
        QUIC_STATUS status;
        status = start_func(handler, args...);
        if (QUIC_FAILED(status)) close_func(handler);
        return status;
    }

    HQUIC get() const { return handler; }
};

};  // namespace rvn::detail

namespace rvn {

/*-----------------QUIC_API_TABLE------------------------*/

static inline auto QUIC_API_TABLE_deleter =
    [](const QUIC_API_TABLE *tbl) { MsQuicClose(tbl); };

using QUIC_API_TABLE_uptr_t =
    std::unique_ptr<const QUIC_API_TABLE,
                    decltype(QUIC_API_TABLE_deleter)>;

class unique_QUIC_API_TABLE : public QUIC_API_TABLE_uptr_t {
   public:
    unique_QUIC_API_TABLE(const QUIC_API_TABLE *tbl);
};

/* rvn::make_unique should not be called on
   non specialised template */
template <class T, class... Args>
std::unique_ptr<T> make_unique(Args &&...);

static inline unique_QUIC_API_TABLE make_unique() {
    const QUIC_API_TABLE *tbl;
    MsQuicOpen2(&tbl);
    return unique_QUIC_API_TABLE(tbl);
}

/*----------------MsQuic->RegistrationOpen---------------*/
class unique_registration
    : public detail::unique_handler1<
          decltype(QUIC_API_TABLE::RegistrationOpen),
          decltype(QUIC_API_TABLE::RegistrationClose)> {
   public:
    unique_registration(
        const QUIC_API_TABLE *tbl_,
        const QUIC_REGISTRATION_CONFIG *RegConfig);
};

/*------------MsQuic->ListenerOpen and Start-------------*/
class unique_listener
    : public detail::unique_handler2<
          decltype(QUIC_API_TABLE::ListenerOpen),
          decltype(QUIC_API_TABLE::ListenerClose),
          decltype(QUIC_API_TABLE::ListenerStart),
          decltype(QUIC_API_TABLE::ListenerStop)> {
    struct ListenerOpenParams {
        HQUIC registration;
        QUIC_LISTENER_CALLBACK_HANDLER listenerCb;
        void *context = NULL;
    };
    struct ListenerStartParams {
        const QUIC_BUFFER *const AlpnBuffers;
        uint32_t AlpnBufferCount = 1;
        const QUIC_ADDR *LocalAddress;
    };

   public:
    unique_listener(const QUIC_API_TABLE *tbl_,
                    ListenerOpenParams openParams,
                    ListenerStartParams startParams);
};

/*-------------MsQuic->Config open and load--------------*/
class unique_configuration
    : public detail::unique_handler2<
          decltype(QUIC_API_TABLE::ConfigurationOpen),
          decltype(QUIC_API_TABLE::ConfigurationClose),
          decltype(QUIC_API_TABLE::ConfigurationLoadCredential)
          /*No need to unload configuration*/> {
    struct ConfigurationOpenParams {
        HQUIC registration;
        const QUIC_BUFFER *const AlpnBuffers;
        uint32_t AlpnBufferCount = 1;
        const QUIC_SETTINGS *Settings;
        uint32_t SettingsSize;
        void *Context;
    };

    struct ConfigurationStartParams {
        const QUIC_CREDENTIAL_CONFIG *CredConfig;
    };

   public:
    unique_configuration(const QUIC_API_TABLE *tbl_,
                         ConfigurationOpenParams openParams,
                         ConfigurationStartParams startParams);
};

};  // namespace rvn