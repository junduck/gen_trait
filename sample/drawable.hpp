#ifndef INCLUDE_13a25b999058c3b83f8b0d00303a4be8b56b88c6
#define INCLUDE_13a25b999058c3b83f8b0d00303a4be8b56b88c6

#include <functional>
#include <iostream>
#include <memory>

namespace jduck::gen_trait::sample {
namespace detail {
struct drawable_base {
  struct vtable {
    using _gen_trait_draw_vtbl_t = void (*)(void *, std::ostream &);
    _gen_trait_draw_vtbl_t draw;
    using _gen_trait_draw_cap_vtbl_t = void (*)(void *, std::ostream &);
    _gen_trait_draw_cap_vtbl_t draw_cap;
    void (*_gen_trait_destroy)(void *);
  };
  template <typename _GEN_TRAIT_TMPL_Impl> struct vtable_impl {
    static void draw(void *_gen_trait_impl, std::ostream &os) {
      return static_cast<_GEN_TRAIT_TMPL_Impl *>(_gen_trait_impl)->draw(os);
    }
    static void draw_cap(void *_gen_trait_impl, std::ostream &os) {
      return static_cast<_GEN_TRAIT_TMPL_Impl *>(_gen_trait_impl)->draw_cap(os);
    }
    static void _gen_trait_destroy(void *_gen_trait_impl) {
      delete static_cast<_GEN_TRAIT_TMPL_Impl *>(_gen_trait_impl);
    }
  };
  template <typename _GEN_TRAIT_TMPL_Impl>
  constexpr static vtable vtable_for{
      vtable_impl<_GEN_TRAIT_TMPL_Impl>::draw,
      vtable_impl<_GEN_TRAIT_TMPL_Impl>::draw_cap,
      vtable_impl<_GEN_TRAIT_TMPL_Impl>::_gen_trait_destroy,
  };
  template <typename _GEN_TRAIT_TMPL_Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<drawable_base, std::decay_t<_GEN_TRAIT_TMPL_Impl>>;
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
class drawable_ref : detail::drawable_base {
  using base = detail::drawable_base;
  friend class drawable;
  friend class drawable_shared;
  friend struct std::hash<drawable_ref>;
  void *_gen_trait_impl;
  typename base::vtable const *_gen_trait_vtbl;
  drawable_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : _gen_trait_impl(impl), _gen_trait_vtbl(vtbl) {}

public:
  drawable_ref() noexcept = default;
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable_ref(_GEN_TRAIT_TMPL_Impl *impl) noexcept
      : drawable_ref(impl, &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable_ref(
      std::unique_ptr<_GEN_TRAIT_TMPL_Impl> const &impl) noexcept
      : drawable_ref(impl.get(),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable_ref(
      std::shared_ptr<_GEN_TRAIT_TMPL_Impl> const &impl) noexcept
      : drawable_ref(impl.get(),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl> &&
                base::template not_smartptr<_GEN_TRAIT_TMPL_Impl>>>
  drawable_ref(_GEN_TRAIT_TMPL_Impl &impl) noexcept
      : drawable_ref(std::addressof(impl),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  void swap(drawable_ref &other) noexcept {
    std::swap(_gen_trait_impl, other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(drawable_ref &lhs, drawable_ref &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(drawable_ref const &lhs,
                         drawable_ref const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  void draw(std::ostream &os) {
    return _gen_trait_vtbl->draw(_gen_trait_impl, os);
  }
  void draw_cap(std::ostream &os) const {
    return _gen_trait_vtbl->draw_cap(_gen_trait_impl, os);
  }
};
class drawable : detail::drawable_base {
  using base = detail::drawable_base;
  friend struct std::hash<drawable>;
  void *_gen_trait_impl;
  typename base::vtable const *_gen_trait_vtbl;

public:
  drawable(drawable const &) = delete;
  drawable &operator=(drawable const &) = delete;
  drawable(drawable &&other) noexcept
      : _gen_trait_impl(other._gen_trait_impl),
        _gen_trait_vtbl(other._gen_trait_vtbl) {
    other._gen_trait_impl = nullptr;
  }
  drawable &operator=(drawable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable(_GEN_TRAIT_TMPL_Impl *impl) noexcept
      : _gen_trait_impl(impl),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable(std::unique_ptr<_GEN_TRAIT_TMPL_Impl> impl) noexcept
      : _gen_trait_impl(impl.release()),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl>>>
  drawable(_GEN_TRAIT_TMPL_Impl &&impl)
      : _gen_trait_impl(new std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>(
            std::forward<_GEN_TRAIT_TMPL_Impl>(impl))),
        _gen_trait_vtbl(&base::template vtable_for<
                        std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>>) {}
  ~drawable() {
    _gen_trait_vtbl->_gen_trait_destroy(_gen_trait_impl);
    _gen_trait_impl = nullptr;
  }
  operator drawable_ref() const { return {_gen_trait_impl, _gen_trait_vtbl}; }
  void swap(drawable &other) noexcept {
    std::swap(_gen_trait_impl, other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(drawable &lhs, drawable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(drawable const &lhs, drawable const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  void draw(std::ostream &os) {
    return _gen_trait_vtbl->draw(_gen_trait_impl, os);
  }
  void draw_cap(std::ostream &os) const {
    return _gen_trait_vtbl->draw_cap(_gen_trait_impl, os);
  }
};
class drawable_shared : detail::drawable_base {
  using base = detail::drawable_base;
  friend struct std::hash<drawable_shared>;
  std::shared_ptr<void> _gen_trait_impl;
  typename base::vtable const *_gen_trait_vtbl;

public:
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable_shared(_GEN_TRAIT_TMPL_Impl *impl)
      : _gen_trait_impl(impl),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable_shared(std::unique_ptr<_GEN_TRAIT_TMPL_Impl> impl) noexcept
      : _gen_trait_impl(impl.release()),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit drawable_shared(std::shared_ptr<_GEN_TRAIT_TMPL_Impl> impl) noexcept
      : _gen_trait_impl(impl),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl>>>
  drawable_shared(_GEN_TRAIT_TMPL_Impl &&impl)
      : _gen_trait_impl(
            std::make_shared<std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>>(
                std::forward<_GEN_TRAIT_TMPL_Impl>(impl))),
        _gen_trait_vtbl(&base::template vtable_for<
                        std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>>) {}
  operator drawable_ref() const {
    return {_gen_trait_impl.get(), _gen_trait_vtbl};
  }
  void swap(drawable_shared &other) noexcept {
    _gen_trait_impl.swap(other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(drawable_shared &lhs, drawable_shared &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(drawable_shared const &lhs,
                         drawable_shared const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  void draw(std::ostream &os) {
    return _gen_trait_vtbl->draw(_gen_trait_impl.get(), os);
  }
  void draw_cap(std::ostream &os) const {
    return _gen_trait_vtbl->draw_cap(_gen_trait_impl.get(), os);
  }
};
} // namespace jduck::gen_trait::sample

namespace std {
template <> struct hash<jduck::gen_trait::sample::drawable_ref> {
  size_t
  operator()(jduck::gen_trait::sample::drawable_ref const &v) const noexcept {
    return std::hash<void *>()(v._gen_trait_impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::drawable> {
  size_t
  operator()(jduck::gen_trait::sample::drawable const &v) const noexcept {
    return std::hash<void *>()(v._gen_trait_impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::drawable_shared> {
  size_t operator()(
      jduck::gen_trait::sample::drawable_shared const &v) const noexcept {
    return std::hash<void *>()(v._gen_trait_impl.get());
  }
};
} // namespace std
#endif // INCLUDE_13a25b999058c3b83f8b0d00303a4be8b56b88c6
