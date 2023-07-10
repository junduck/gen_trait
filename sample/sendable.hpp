#ifndef INCLUDE_d39e1723a1a81dc416317f337b3fa33dfda1a8ce
#define INCLUDE_d39e1723a1a81dc416317f337b3fa33dfda1a8ce

#include <functional>
#include <memory>
#include <string>

namespace jduck::gen_trait::sample {
namespace detail {
struct sendable_base {
  struct vtable {
    using send_t = bool (*)(void *, std::string const &);
    send_t send;
    void (*_gen_trait_destroy)(void *);
  };
  template <typename Impl> struct vtable_impl {
    static bool send(void *impl, std::string const &msg) {
      return static_cast<Impl *>(impl)->send(msg);
    }
    static void _gen_trait_destroy(void *impl) {
      delete static_cast<Impl *>(impl);
    }
  };
  template <typename Impl>
  constexpr static vtable vtable_for{
      vtable_impl<Impl>::send,
      vtable_impl<Impl>::_gen_trait_destroy,
  };
  template <typename Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<sendable_base, std::decay_t<Impl>>;
  template <typename T, template <typename...> typename TA>
  struct is_specialization_of : std::false_type {};
  template <template <typename...> typename T, typename... TA>
  struct is_specialization_of<T<TA...>, T> : std::true_type {};
  template <typename T>
  constexpr static bool not_smartptr =
      !(is_specialization_of<T, std::shared_ptr>::value ||
        is_specialization_of<T, std::unique_ptr>::value);
};
} // namespace detail
class sendable_ref : detail::sendable_base {
  using base = detail::sendable_base;
  friend class sendable;
  friend class sendable_shared;
  friend struct std::hash<sendable_ref>;
  void *impl;
  typename base::vtable::send_t vtbl;
  sendable_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : impl(impl), vtbl(vtbl->send) {}

public:
  sendable_ref() noexcept = default;
  template <typename Impl>
  explicit sendable_ref(Impl *impl) noexcept
      : sendable_ref(impl, &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit sendable_ref(std::unique_ptr<Impl> const &impl) noexcept
      : sendable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit sendable_ref(std::shared_ptr<Impl> const &impl) noexcept
      : sendable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl> &&
                                        base::template not_smartptr<Impl>>>
  sendable_ref(Impl &impl) noexcept
      : sendable_ref(std::addressof(impl), &base::template vtable_for<Impl>) {}
  void swap(sendable_ref &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(sendable_ref &lhs, sendable_ref &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(sendable_ref const &lhs,
                         sendable_ref const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  bool send(std::string const &msg) { return vtbl(impl, msg); }
};
class sendable : detail::sendable_base {
  using base = detail::sendable_base;
  friend struct std::hash<sendable>;
  void *impl;
  typename base::vtable const *vtbl;

public:
  sendable(sendable const &) = delete;
  sendable &operator=(sendable const &) = delete;
  sendable(sendable &&other) noexcept : impl(other.impl), vtbl(other.vtbl) {
    other.impl = nullptr;
  }
  sendable &operator=(sendable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename Impl>
  explicit sendable(Impl *impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit sendable(std::unique_ptr<Impl> impl) noexcept
      : impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  sendable(Impl &&impl)
      : impl(new std::remove_reference_t<Impl>(std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  ~sendable() {
    vtbl->_gen_trait_destroy(impl);
    impl = nullptr;
  }
  operator sendable_ref() const { return {impl, vtbl}; }
  void swap(sendable &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(sendable &lhs, sendable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(sendable const &lhs, sendable const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  bool send(std::string const &msg) { return vtbl->send(impl, msg); }
};

} // namespace jduck::gen_trait::sample

namespace std {
template <> struct hash<jduck::gen_trait::sample::sendable_ref> {
  size_t
  operator()(jduck::gen_trait::sample::sendable_ref const &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::sendable> {
  size_t
  operator()(jduck::gen_trait::sample::sendable const &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};

} // namespace std
#endif // INCLUDE_d39e1723a1a81dc416317f337b3fa33dfda1a8ce
