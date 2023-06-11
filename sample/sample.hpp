#ifndef INCLUDE_ead89cfa4467da2c883e8ea0ab1a743a0e2afd8f
#define INCLUDE_ead89cfa4467da2c883e8ea0ab1a743a0e2afd8f

#include <memory>
#include <tuple>

namespace jduck::gen_trait::sample {
namespace detail {
template <size_t I, typename... Args> struct pack_and_get_base {
  struct vtable {
    std::tuple<Args...> (*pack)(void *, Args...);
    int (*get)(void *) noexcept;
    void (*destroy)(void *);
  };
  template <typename Impl> struct vtable_impl {
    static std::tuple<Args...> pack(void *impl, Args... args) {
      return static_cast<Impl *>(impl)->pack(std::move(args)...);
    }
    static int get(void *impl) noexcept {
      return static_cast<Impl *>(impl)->get();
    }
    static void destroy(void *impl) { delete static_cast<Impl *>(impl); }
  };
  template <typename Impl>
  constexpr static vtable vtable_for{
      vtable_impl<Impl>::pack,
      vtable_impl<Impl>::get,
      vtable_impl<Impl>::destroy,
  };
  template <typename Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<pack_and_get_base, std::decay_t<Impl>>;
};
struct sendable_base {
  struct vtable {
    void (*send)(void *, int const &);
    void (*destroy)(void *);
  };
  template <typename Impl> struct vtable_impl {
    static void send(void *impl, int const &x) {
      return static_cast<Impl *>(impl)->send(x);
    }
    static void destroy(void *impl) { delete static_cast<Impl *>(impl); }
  };
  template <typename Impl>
  constexpr static vtable vtable_for{
      vtable_impl<Impl>::send,
      vtable_impl<Impl>::destroy,
  };
  template <typename Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<sendable_base, std::decay_t<Impl>>;
};
} // namespace detail
template <size_t I, typename... Args>
class pack_and_get_ref : detail::pack_and_get_base<I, Args...> {
  using base = detail::pack_and_get_base<I, Args...>;
  template <size_t, typename...> friend class pack_and_get;
  template <size_t, typename...> friend class pack_and_get_shared;
  friend struct std::hash<pack_and_get_ref>;
  void *impl;
  typename base::vtable const *vtbl;
  pack_and_get_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : impl(impl), vtbl(vtbl) {}

public:
  pack_and_get_ref() noexcept = default;
  template <typename Impl>
  explicit pack_and_get_ref(Impl *impl) noexcept
      : pack_and_get_ref(impl, &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit pack_and_get_ref(std::unique_ptr<Impl> impl) noexcept
      : pack_and_get_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit pack_and_get_ref(std::shared_ptr<Impl> impl) noexcept
      : pack_and_get_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  pack_and_get_ref(Impl const &impl) noexcept
      : pack_and_get_ref(std::addressof(const_cast<Impl &>(impl)),
                         &base::template vtable_for<Impl>) {}
  void swap(pack_and_get_ref &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(pack_and_get_ref &lhs, pack_and_get_ref &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(pack_and_get_ref const &lhs,
                         pack_and_get_ref const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }
  explicit operator bool() const noexcept { return impl != nullptr; }

  std::tuple<Args...> pack(Args... args) {
    return vtbl->pack(impl, std::move(args)...);
  }
  int get() const noexcept { return vtbl->get(impl); }
};
class sendable_ref : detail::sendable_base {
  using base = detail::sendable_base;
  friend class sendable;
  friend class sendable_shared;
  friend struct std::hash<sendable_ref>;
  void *impl;
  typename base::vtable const *vtbl;
  sendable_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : impl(impl), vtbl(vtbl) {}

public:
  sendable_ref() noexcept = default;
  template <typename Impl>
  explicit sendable_ref(Impl *impl) noexcept
      : sendable_ref(impl, &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit sendable_ref(std::unique_ptr<Impl> impl) noexcept
      : sendable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit sendable_ref(std::shared_ptr<Impl> impl) noexcept
      : sendable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  sendable_ref(Impl const &impl) noexcept
      : sendable_ref(std::addressof(const_cast<Impl &>(impl)),
                     &base::template vtable_for<Impl>) {}
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
  explicit operator bool() const noexcept { return impl != nullptr; }

  void send(int const &x) { return vtbl->send(impl, x); }
};
template <size_t I, typename... Args>
class pack_and_get : detail::pack_and_get_base<I, Args...> {
  using base = detail::pack_and_get_base<I, Args...>;
  friend struct std::hash<pack_and_get>;
  void *impl;
  typename base::vtable const *vtbl;

public:
  pack_and_get(pack_and_get const &) = delete;
  pack_and_get &operator=(pack_and_get const &) = delete;
  pack_and_get(pack_and_get &&other) noexcept
      : impl(other.impl), vtbl(other.vtbl) {
    other.impl = nullptr;
  }
  pack_and_get &operator=(pack_and_get &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename Impl>
  explicit pack_and_get(Impl *impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit pack_and_get(std::unique_ptr<Impl> impl) noexcept
      : impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  pack_and_get(Impl &&impl)
      : impl(new Impl(std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  ~pack_and_get() {
    vtbl->destroy(impl);
    impl = nullptr;
  }
  operator pack_and_get_ref<I, Args...>() const { return {impl, vtbl}; }
  void swap(pack_and_get &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(pack_and_get &lhs, pack_and_get &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(pack_and_get const &lhs,
                         pack_and_get const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  std::tuple<Args...> pack(Args... args) {
    return vtbl->pack(impl, std::move(args)...);
  }
  int get() const noexcept { return vtbl->get(impl); }
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
      : impl(new Impl(std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  ~sendable() {
    vtbl->destroy(impl);
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

  void send(int const &x) { return vtbl->send(impl, x); }
};
template <size_t I, typename... Args>
class pack_and_get_shared : detail::pack_and_get_base<I, Args...> {
  using base = detail::pack_and_get_base<I, Args...>;
  friend struct std::hash<pack_and_get_shared>;
  std::shared_ptr<void> impl;
  typename base::vtable const *vtbl;

public:
  template <typename Impl>
  explicit pack_and_get_shared(Impl *impl)
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit pack_and_get_shared(std::shared_ptr<Impl> impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  pack_and_get_shared(Impl &&impl)
      : impl(std::make_shared<std::remove_reference_t<Impl>>(
            std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  operator pack_and_get_ref<I, Args...>() const { return {impl.get(), vtbl}; }
  void swap(pack_and_get_shared &other) noexcept {
    impl.swap(other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(pack_and_get_shared &lhs,
                   pack_and_get_shared &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(pack_and_get_shared const &lhs,
                         pack_and_get_shared const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  std::tuple<Args...> pack(Args... args) {
    return vtbl->pack(impl.get(), std::move(args)...);
  }
  int get() const noexcept { return vtbl->get(impl.get()); }
};
class sendable_shared : detail::sendable_base {
  using base = detail::sendable_base;
  friend struct std::hash<sendable_shared>;
  std::shared_ptr<void> impl;
  typename base::vtable const *vtbl;

public:
  template <typename Impl>
  explicit sendable_shared(Impl *impl)
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit sendable_shared(std::shared_ptr<Impl> impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  sendable_shared(Impl &&impl)
      : impl(std::make_shared<std::remove_reference_t<Impl>>(
            std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  operator sendable_ref() const { return {impl.get(), vtbl}; }
  void swap(sendable_shared &other) noexcept {
    impl.swap(other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(sendable_shared &lhs, sendable_shared &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(sendable_shared const &lhs,
                         sendable_shared const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  void send(int const &x) { return vtbl->send(impl.get(), x); }
};
} // namespace jduck::gen_trait::sample

namespace std {
template <size_t I, typename... Args>
struct hash<jduck::gen_trait::sample::pack_and_get_ref<I, Args...>> {
  size_t operator()(jduck::gen_trait::sample::pack_and_get_ref<I, Args...> const
                        &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::sendable_ref> {
  size_t
  operator()(jduck::gen_trait::sample::sendable_ref const &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <size_t I, typename... Args>
struct hash<jduck::gen_trait::sample::pack_and_get<I, Args...>> {
  size_t operator()(jduck::gen_trait::sample::pack_and_get<I, Args...> const &v)
      const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::sendable> {
  size_t
  operator()(jduck::gen_trait::sample::sendable const &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <size_t I, typename... Args>
struct hash<jduck::gen_trait::sample::pack_and_get_shared<I, Args...>> {
  size_t
  operator()(jduck::gen_trait::sample::pack_and_get_shared<I, Args...> const &v)
      const noexcept {
    return std::hash<void *>()(v.impl.get());
  }
};
template <> struct hash<jduck::gen_trait::sample::sendable_shared> {
  size_t operator()(
      jduck::gen_trait::sample::sendable_shared const &v) const noexcept {
    return std::hash<void *>()(v.impl.get());
  }
};
} // namespace std
#endif // INCLUDE_ead89cfa4467da2c883e8ea0ab1a743a0e2afd8f
