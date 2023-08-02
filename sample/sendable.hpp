#ifndef INCLUDE_57684e5d0153682189e540037a7832c80c6c6245
#define INCLUDE_57684e5d0153682189e540037a7832c80c6c6245

#include <functional>
#include <memory>
#include <string>

namespace jduck::gen_trait::sample {
namespace detail {
struct sendable_base {
  struct vtable {
    using _gen_trait_send_vtbl_t = bool (*)(void *, std::string const &);
    _gen_trait_send_vtbl_t send;
    void (*_gen_trait_destroy)(void *);
  };
  template <typename _GEN_TRAIT_TMPL_Impl> struct vtable_impl {
    static bool send(void *_gen_trait_impl, std::string const &msg) {
      return static_cast<_GEN_TRAIT_TMPL_Impl *>(_gen_trait_impl)->send(msg);
    }
    static void _gen_trait_destroy(void *_gen_trait_impl) {
      delete static_cast<_GEN_TRAIT_TMPL_Impl *>(_gen_trait_impl);
    }
  };
  template <typename _GEN_TRAIT_TMPL_Impl>
  constexpr static vtable vtable_for{
      vtable_impl<_GEN_TRAIT_TMPL_Impl>::send,
      vtable_impl<_GEN_TRAIT_TMPL_Impl>::_gen_trait_destroy,
  };
  template <typename _GEN_TRAIT_TMPL_Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<sendable_base, std::decay_t<_GEN_TRAIT_TMPL_Impl>>;
  template <typename _GEN_TRAIT_TMPL_T,
            template <typename...> typename _GEN_TRAIT_TMPL_TA>
  struct is_specialization_of : std::false_type {};
  template <template <typename...> typename _GEN_TRAIT_TMPL_T,
            typename... _GEN_TRAIT_TMPL_TA>
  struct is_specialization_of<_GEN_TRAIT_TMPL_T<_GEN_TRAIT_TMPL_TA...>,
                              _GEN_TRAIT_TMPL_T> : std::true_type {};
  template <typename _GEN_TRAIT_TMPL_T>
  constexpr static bool not_smartptr =
      !(is_specialization_of<_GEN_TRAIT_TMPL_T, std::shared_ptr>::value ||
        is_specialization_of<_GEN_TRAIT_TMPL_T, std::unique_ptr>::value);
};
} // namespace detail
class sendable_ref : detail::sendable_base {
  using base = detail::sendable_base;
  friend class sendable;
  friend class sendable_shared;
  friend struct std::hash<sendable_ref>;
  void *_gen_trait_impl;
  typename base::vtable::_gen_trait_send_vtbl_t _gen_trait_vtbl;
  sendable_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : _gen_trait_impl(impl), _gen_trait_vtbl(vtbl->send) {}

public:
  sendable_ref() noexcept = default;
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit sendable_ref(_GEN_TRAIT_TMPL_Impl *impl) noexcept
      : sendable_ref(impl, &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit sendable_ref(
      std::unique_ptr<_GEN_TRAIT_TMPL_Impl> const &impl) noexcept
      : sendable_ref(impl.get(),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit sendable_ref(
      std::shared_ptr<_GEN_TRAIT_TMPL_Impl> const &impl) noexcept
      : sendable_ref(impl.get(),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl> &&
                base::template not_smartptr<_GEN_TRAIT_TMPL_Impl>>>
  sendable_ref(_GEN_TRAIT_TMPL_Impl &impl) noexcept
      : sendable_ref(std::addressof(impl),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  void swap(sendable_ref &other) noexcept {
    std::swap(_gen_trait_impl, other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(sendable_ref &lhs, sendable_ref &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(sendable_ref const &lhs,
                         sendable_ref const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  bool send(std::string const &msg) {
    return _gen_trait_vtbl(_gen_trait_impl, msg);
  }
};
class sendable : detail::sendable_base {
  using base = detail::sendable_base;
  friend struct std::hash<sendable>;
  void *_gen_trait_impl;
  typename base::vtable const *_gen_trait_vtbl;

public:
  sendable(sendable const &) = delete;
  sendable &operator=(sendable const &) = delete;
  sendable(sendable &&other) noexcept
      : _gen_trait_impl(other._gen_trait_impl),
        _gen_trait_vtbl(other._gen_trait_vtbl) {
    other._gen_trait_impl = nullptr;
  }
  sendable &operator=(sendable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit sendable(_GEN_TRAIT_TMPL_Impl *impl) noexcept
      : _gen_trait_impl(impl),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit sendable(std::unique_ptr<_GEN_TRAIT_TMPL_Impl> impl) noexcept
      : _gen_trait_impl(impl.release()),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl>>>
  sendable(_GEN_TRAIT_TMPL_Impl &&impl)
      : _gen_trait_impl(new std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>(
            std::forward<_GEN_TRAIT_TMPL_Impl>(impl))),
        _gen_trait_vtbl(&base::template vtable_for<
                        std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>>) {}
  ~sendable() {
    _gen_trait_vtbl->_gen_trait_destroy(_gen_trait_impl);
    _gen_trait_impl = nullptr;
  }
  operator sendable_ref() const { return {_gen_trait_impl, _gen_trait_vtbl}; }
  void swap(sendable &other) noexcept {
    std::swap(_gen_trait_impl, other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(sendable &lhs, sendable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(sendable const &lhs, sendable const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  bool send(std::string const &msg) {
    return _gen_trait_vtbl->send(_gen_trait_impl, msg);
  }
};

} // namespace jduck::gen_trait::sample

namespace std {
template <> struct hash<jduck::gen_trait::sample::sendable_ref> {
  size_t
  operator()(jduck::gen_trait::sample::sendable_ref const &v) const noexcept {
    return std::hash<void *>()(v._gen_trait_impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::sendable> {
  size_t
  operator()(jduck::gen_trait::sample::sendable const &v) const noexcept {
    return std::hash<void *>()(v._gen_trait_impl);
  }
};

} // namespace std
#endif // INCLUDE_57684e5d0153682189e540037a7832c80c6c6245
