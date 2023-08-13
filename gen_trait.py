import hashlib
import json
import sys


class CppVal:
    # TODO: only basic support, no storage/linkage etc.
    def __init__(self, type: str, name: str, cvref: str = ""):
        self._type = str(type)
        self._name = str(name)
        self._cvref = str(cvref)

    @staticmethod
    def load_from(a) -> "CppVal":
        return CppVal(a['type'], a['name'], a.get('cvref', ""))

    @property
    def name(self):
        return self._name

    @property
    def type(self):
        return self._type

    @property
    def cvref(self):
        return self._cvref

    @cvref.deleter
    def cvref(self):
        self._cvref = ""

    def forward(self):
        """Create an ad-hoc forward expression of self and treat it as a name."""
        new_name = f"static_cast<{self._type}&&>({self._name})"
        return CppVal(self._type, new_name, "")

    def __str__(self):
        return self._name


class CppTparam:
    def __init__(self, type: str, name: str, pack: bool = False):
        self._type = str(type)
        self._name = str(name)
        self._pack = pack

    @staticmethod
    def load_from(t) -> "CppTparam":
        return CppTparam(t['type'], t['name'], t.get('pack', False))

    @property
    def type(self):
        return self._type

    @property
    def name(self):
        return self._name

    @property
    def pack(self):
        return self._pack

    def __str__(self):
        return self._name


class CppExprBuilder:
    def __init__(self, oprand, is_ptr: bool = False):
        self._oprand = str(oprand)
        self._setptr = is_ptr

    def sub(self, index):
        """Subscript"""
        return CppExprBuilder(f"{self._oprand}[{index}]")

    def __getitem__(self, index):
        return self.sub(index)

    def deref(self):
        """Dereference"""
        return CppExprBuilder(f"*{self._oprand}")

    def addr(self):
        """Take address"""
        return CppExprBuilder(f"&{self._oprand}")

    def setptr(self, ptr: bool = True):
        """Optionally mark self as a pointer, for correct member access"""
        return CppExprBuilder(self._oprand, ptr)

    def member(self, member):
        """Access member"""
        return CppExprBuilder(f"{self._oprand}->{member}") if self._setptr else CppExprBuilder(f"{self._oprand}.{member}")

    def cast(self, type, op="static_cast"):
        """Cast to type"""
        if (op == "c"):
            return CppExprBuilder(f"({type}){self._oprand}")
        else:
            return CppExprBuilder(f"{op}<{type}>({self._oprand})")

    def call(self, args = None):
        """Function call"""
        if args:
            return CppExprBuilder(f"{self._oprand}({args})")
        else:
            return CppExprBuilder(f"{self._oprand}()")

    def called_by(self, f):
        """Called by function"""
        return CppExprBuilder(f"{f}({self._oprand})")

    def wrap(self):
        """Wrap in parentheses"""
        return CppExprBuilder(f"({self._oprand})")

    def binary_op(self, op, rhs):
        """Binary operation"""
        return CppExprBuilder(f"{self._oprand} {op} {rhs}")

    def logic_or(self, rhs):
        return self.binary_op("||", rhs)

    def logic_and(self, rhs):
        return self.binary_op("&&", rhs)

    def bitwise_or(self, rhs):
        return self.binary_op("|", rhs)

    def bitwise_xor(self, rhs):
        return self.binary_op("^", rhs)

    def bitwise_and(self, rhs):
        return self.binary_op("&", rhs)

    def __add__(self, rhs):
        return self.binary_op("+", rhs)

    def __sub__(self, rhs):
        return self.binary_op("-", rhs)

    def __mul__(self, rhs):
        return self.binary_op("*", rhs)

    def __truediv__(self, rhs):
        return self.binary_op("/", rhs)

    def __mod__(self, rhs):
        return self.binary_op("%", rhs)

    # NOTE: logical ops not implemented to avoid confusion between bitwise and logical

    def __lshift__(self, rhs):
        return self.binary_op("<<", rhs)

    def __rshift__(self, rhs):
        return self.binary_op(">>", rhs)

    def __eq__(self, rhs):
        return self.binary_op("==", rhs)

    def __ne__(self, rhs):
        return self.binary_op("!=", rhs)

    def __lt__(self, rhs):
        return self.binary_op("<", rhs)

    def __gt__(self, rhs):
        return self.binary_op(">", rhs)

    def __le__(self, rhs):
        return self.binary_op("<=", rhs)

    def __ge__(self, rhs):
        return self.binary_op(">=", rhs)

    def unary_op(self, op):
        """Unary operation"""
        return CppExprBuilder(f"{op}{self._oprand}")

    def __neg__(self):
        return self.unary_op("-")

    def __pos__(self):
        return self.unary_op("+")

    def __invert__(self):
        return self.unary_op("~")

    def __not__(self):
        return self.unary_op("!")

    def expand(self):
        """Expand pack"""
        return CppExprBuilder(f"{self._oprand}...")

    def __str__(self):
        return self._oprand


class CppArgBuilder:
    def __init__(self, template_list: list[CppTparam] = None):
        self._tlist = template_list.copy() if template_list is not None else []
        self._has_tmpl = len(self._tlist) > 0
        self._tmap = {}  # type param
        self._vmap = {}  # value param
        self._has_t = False
        self._has_v = False
        for t in self._tlist:
            if t.type == "typename" or t.type == "class":
                self._tmap[t.name] = t
                self._has_t = True
            else:
                self._vmap[t.name] = t
                self._has_v = True
        self._alist = []

    def add_arg(self, arg: CppVal):
        self._alist.append(arg)
        return self

    def add_arg_list(self, arg_list: list[CppVal]):
        self._alist.extend(arg_list)
        return self

    def _is_ttmpl(self, type: str) -> bool:
        return self._has_t and type in self._tmap

    def _is_vtmpl(self, name: str) -> bool:
        return self._has_v and name in self._vmap

    def _is_tpack(self, type: str) -> bool:
        return self._has_t and type in self._tmap and self._tmap[type].pack

    def _is_vpack(self, name: str) -> bool:
        return self._has_v and name in self._vmap and self._vmap[name].pack

    def as_fparam(self, arg: CppVal) -> str:
        """Transform an arg to function param"""
        tmpl = "{type} {cvref}..." if self._is_tpack(
            arg.type) else "{type} {cvref}"
        return tmpl.format(type=arg.type, cvref=arg.cvref)

    def as_fparam_named(self, arg: CppVal) -> str:
        """Transform an arg to function param"""
        tmpl = "{type} {cvref}... {name}" if self._is_tpack(
            arg.type) else "{type} {cvref} {name}"
        return tmpl.format(type=arg.type, name=arg.name, cvref=arg.cvref)

    def as_farg(self, arg: CppVal) -> str:
        """Transform arg to function call arg"""
        if self._is_tpack(arg.type) or self._is_vpack(arg.name):
            return f"{arg.name}..."
        else:
            return arg.name

    def fparam_list(self) -> str:
        """Build function param list"""
        return ", ".join([self.as_fparam(arg) for arg in self._alist if not self._is_vtmpl(arg.name)])

    def fparam_list_named(self) -> str:
        """Build function param list"""
        return ", ".join([self.as_fparam_named(arg) for arg in self._alist if not self._is_vtmpl(arg.name)])

    def farg_list(self) -> str:
        """Build function arg list"""
        return ", ".join([self.as_farg(arg) for arg in self._alist])

    @staticmethod
    def _ellipse(t: CppTparam) -> str:
        return "..." if t.pack else ""

    def tparam_list(self) -> str:
        """Build template param list"""
        if not self._has_tmpl:
            return ""
        tmpl = "{type}{ellipsis}"
        return ", ".join([tmpl.format(type=t.type, ellipsis=self._ellipse(t)) for t in self._tlist])

    def tparam_list_named(self) -> str:
        """Build named template param list"""
        if not self._has_tmpl:
            return ""
        tmpl = "{type}{ellipsis} {name}"
        return ", ".join([tmpl.format(type=t.type, name=t.name, ellipsis=self._ellipse(t)) for t in self._tlist])

    def targ_list(self) -> str:
        """Build template arg list"""
        if not self._has_tmpl:
            return ""
        tmpl = "{name}{ellipsis}"
        return ", ".join([tmpl.format(name=t.name, ellipsis=self._ellipse(t)) for t in self._tlist])


class TraitFunc:
    # NOTE: trait functions are not templated. All templates are class scoped.
    def __init__(self, fid: int, name: str, ret: str, args: list[CppVal], args_wrap: list[str] = None, cvref: str = ""):
        self._fid = fid
        self._name = name
        self._ret = ret
        self._args = args.copy() if args is not None else []
        self._args_wrap = args_wrap.copy() if args_wrap is not None else []
        self._cvref = cvref

    @staticmethod
    def load_from(f) -> "TraitFunc":
        args = []
        args_wrap = []
        for a in f['args']:
            args.append(CppVal.load_from(a))
            args_wrap.append(a.get('wrap', ""))
        return TraitFunc(f.get('fid', 0), f['name'], f['ret'], args, args_wrap, f.get('cvref', ""))

    @property
    def fid(self) -> int:
        return self._fid

    @fid.setter
    def fid(self, fid: int):
        self._fid = fid

    @property
    def name(self) -> str:
        return self._name

    @property
    def vname(self) -> str:
        return f"_gentrait_fn{self.fid}"

    @property
    def ret(self) -> str:
        return self._ret

    @property
    def args(self) -> list[CppVal]:
        return self._args

    @property
    def args_wrap(self) -> list[str]:
        return self._args_wrap

    @property
    def cvref(self) -> str:
        return self._cvref

    @property
    def noexcept(self) -> bool:
        return "noexcept" in self._cvref


class TraitTmpl:
    @staticmethod
    def func_ptr(ret: str, plist: str, noexcept: bool) -> str:
        tmpl = "{ret} (*)({plist}) noexcept" if noexcept else "{ret} (*)({plist})"
        return tmpl.format(ret=ret, plist=plist)

    @staticmethod
    def func_decl(ret: str, name: str, plist: str, cvref: str, noexcept: bool) -> str:
        tmpl = "{ret} {name}({plist}) {cvref} noexcept" if noexcept else "{ret} {name}({plist}) {cvref}"
        return tmpl.format(ret=ret, name=name, plist=plist, cvref=cvref)

    @staticmethod
    def func_impl(decl: str, ret_expr: str, static: bool) -> str:
        tmpl = "static {decl} {{ return {ret_expr}; }}" if static else "{decl} {{ return {ret_expr}; }}"
        return tmpl.format(decl=decl, ret_expr=ret_expr)

    @staticmethod
    def class_decl(name: str, tplist: str, type: str = "class", friend: bool = False) -> str:
        if not tplist:
            return f"friend {type} {name}" if friend else f"{type} {name}"
        if friend:
            return f"template <{tplist}> friend {type} {name}"
        else:
            return f"template <{tplist}> {type} {name}"

    @staticmethod
    def vtable_entry(name: str, fptr: str) -> str:
        tmpl = """using {name}_t = {fptr};
{name}_t {name};"""
        return tmpl.format(name=name, fptr=fptr)

    @staticmethod
    def vtable_for_entry(name: str) -> str:
        return f"vtable_impl<_GENTRAIT_IMPL>::{name},"

    @staticmethod
    def specialise(name: str, ns: str, talist: str) -> str:
        if not talist:
            return f"{ns}::{name}" if ns else name
        else:
            return f"{ns}::{name}<{talist}>" if ns else f"{name}<{talist}>"

    # TODO: defaulted operator== is C++20
    trait_base = """{decl} {{
struct vtable_base {{
{vtable_list}
bool operator==(vtable_base const &) const = default;
}};
struct vtable : vtable_base {{
void (*_gentrait_destroy)(void *);
}};
template <typename _GENTRAIT_IMPL>
struct vtable_impl {{
{vtable_impl_list}
static void _gentrait_destroy(void *impl) {{ delete static_cast<_GENTRAIT_IMPL *>(impl); }}
}};
template <typename _GENTRAIT_IMPL>
constexpr static vtable vtable_for{{
{vtable_for_list}
vtable_impl<_GENTRAIT_IMPL>::_gentrait_destroy,
}};
template <typename _GENTRAIT_IMPL>
constexpr static bool not_relative = !std::is_base_of_v<{name}, std::decay_t<_GENTRAIT_IMPL>>;
template <typename _GENTRAIT_T, template <typename...> typename _GENTRAIT_TA>
struct is_specialization_of : std::false_type {{}};
template <template <typename...> typename _GENTRAIT_T, typename... _GENTRAIT_TA>
struct is_specialization_of<_GENTRAIT_T<_GENTRAIT_TA...>, _GENTRAIT_T> : std::true_type {{}};
template <typename _GENTRAIT_T>
constexpr static bool not_smartptr =
!(is_specialization_of<_GENTRAIT_T, std::shared_ptr>::value ||
is_specialization_of<_GENTRAIT_T, std::unique_ptr>::value);
}};"""

    trait_ref = """{decl} : {base} {{
using base = {base};
{friend_decl_unique};
{friend_decl_shared};
friend struct std::hash<{name}>;
void *_gentrait_impl;
{vtbl_type} _gentrait_vtbl;
{name}(void *impl, typename base::vtable const *vtbl) noexcept : _gentrait_impl(impl), _gentrait_vtbl({vtbl_get}) {{}}
public:
{name}() noexcept = default;
template <typename _GENTRAIT_IMPL>
explicit {name}(_GENTRAIT_IMPL *impl) noexcept : {name}(impl, &base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL>
explicit {name}(std::unique_ptr<_GENTRAIT_IMPL> const &impl) noexcept : {name}(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL>
explicit {name}(std::shared_ptr<_GENTRAIT_IMPL> const &impl) noexcept : {name}(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL,
typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL> &&
base::template not_smartptr<_GENTRAIT_IMPL>>>
{name}(_GENTRAIT_IMPL &impl) noexcept : {name}(std::addressof(impl), &base::template vtable_for<_GENTRAIT_IMPL>) {{}}
void swap({name} &other) noexcept {{
std::swap(_gentrait_impl, other._gentrait_impl);
std::swap(_gentrait_vtbl, other._gentrait_vtbl);
}}
friend void swap({name} &lhs, {name} &rhs) noexcept {{ lhs.swap(rhs); }}
friend bool operator==({name} const &lhs, {name} const &rhs) noexcept {{ return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }}

{trait_list}
}};"""

    trait_unique = """{decl} : {base} {{
using base = {base};
friend struct std::hash<{name}>;
void *_gentrait_impl;
typename base::vtable const *_gentrait_vtbl;
public:
{name}({name} const &) = delete;
{name} &operator=({name} const &) = delete;
{name}({name} &&other) noexcept : _gentrait_impl(other._gentrait_impl), _gentrait_vtbl(other._gentrait_vtbl) {{ other._gentrait_impl = nullptr; }}
{name} &operator=({name} &&other) noexcept {{
auto tmp(std::move(other));
tmp.swap(*this);
return *this;
}}
template <typename _GENTRAIT_IMPL>
explicit {name}(_GENTRAIT_IMPL *impl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL>
explicit {name}(std::unique_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl.release()), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL, typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL>>>
{name}(_GENTRAIT_IMPL &&impl) : _gentrait_impl(new std::remove_reference_t<_GENTRAIT_IMPL>(std::forward<_GENTRAIT_IMPL>(impl))),
_gentrait_vtbl(&base::template vtable_for<std::remove_reference_t<_GENTRAIT_IMPL>>) {{}}
~{name}() {{ _gentrait_vtbl->_gentrait_destroy(_gentrait_impl); _gentrait_impl = nullptr; }}
operator {name_ref}() const {{ return {{_gentrait_impl, _gentrait_vtbl}}; }}
void swap({name} &other) noexcept {{
std::swap(_gentrait_impl, other._gentrait_impl);
std::swap(_gentrait_vtbl, other._gentrait_vtbl);
}}
friend void swap({name} &lhs, {name} &rhs) noexcept {{ lhs.swap(rhs); }}
friend bool operator==({name} const &lhs, {name} const &rhs) noexcept {{ return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }}

{trait_list}
}};"""

    trait_shared = """{decl} : {base} {{
using base = {base};
friend struct std::hash<{name}>;
std::shared_ptr<void> _gentrait_impl;
typename base::vtable const *_gentrait_vtbl;

public:
template <typename _GENTRAIT_IMPL>
explicit {name}(_GENTRAIT_IMPL *impl) : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL>
explicit {name}(std::unique_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl.release()), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL>
explicit {name}(std::shared_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {{}}
template <typename _GENTRAIT_IMPL, typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL>>>
{name}(_GENTRAIT_IMPL &&impl) : _gentrait_impl(std::make_shared<std::remove_reference_t<_GENTRAIT_IMPL>>(std::forward<_GENTRAIT_IMPL>(impl))), _gentrait_vtbl(&base::template vtable_for<std::remove_reference_t<_GENTRAIT_IMPL>>) {{}}
operator {name_ref}() const {{ return {{_gentrait_impl.get(), _gentrait_vtbl}}; }}
void swap({name} &other) noexcept {{
_gentrait_impl.swap(other._gentrait_impl);
std::swap(_gentrait_vtbl, other._gentrait_vtbl);
}}
friend void swap({name} &lhs, {name} &rhs) noexcept {{ lhs.swap(rhs); }}
friend bool operator==({name} const &lhs, {name} const &rhs) noexcept {{ return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }}

{trait_list}
}};"""

    trait_hash = """template <{tplist}> struct hash<{name}>{{
size_t operator()({name} const &v) const noexcept {{
return {ret_expr};
}}
}};"""

    full = """{guard_begin}
{include}

{ns_begin}
namespace detail {{
{trait_base_list}
}}
{trait_ref_list}
{trait_unique_list}
{trait_shared_list}
{ns_end}

namespace std {{
{hash_ref_list}
{hash_unique_list}
{hash_shared_list}
}}
{guard_end}"""

class TraitBuilder:
    """Trait generator"""

    def __init__(self, name: str, template_list: list[CppTparam], function_list: list[TraitFunc], ns: str, inplace_ref: bool, gen: list[str]):
        self._name = name
        self._tmpl = template_list
        self._ttype = {t.name for t in template_list if t.type == "typename" or t.type == "class"}
        self._func = function_list
        for i in range(len(self._func)):
            self._func[i].fid = i
        self._ns = ns
        self._inplace_ref = inplace_ref
        self._gen = {g[0].lower() for g in gen if g}
        if not self._gen:
            self._gen = {"r", "u", "s"}

    def _wrap_args(self, f: TraitFunc) -> list[CppVal]:
        args = []
        for a, w in zip(f.args, f.args_wrap):
            if w:
                args.append(CppVal(a.type, CppExprBuilder(a.name).called_by(w), a.cvref))
            elif a.type in self._ttype and not a.cvref:
                # arg is a template type, we need to foward it
                args.append(a.forward())
            else:
                args.append(a)
        return args

    def _specialise_base(self) -> str:
        talist = CppArgBuilder(self._tmpl).targ_list()
        return TraitTmpl.specialise(self._name + "_base", "detail", talist)

    def _specialise_trait(self, name: str, ns: str = "") -> str:
        talist = CppArgBuilder(self._tmpl).targ_list()
        return TraitTmpl.specialise(name, ns, talist)

    def _decl_trait(self, name: str) -> str:
        tplist = CppArgBuilder(self._tmpl).tparam_list_named()
        if name.endswith("_base"):
            return TraitTmpl.class_decl(name, tplist, "struct", False)
        else:
            return TraitTmpl.class_decl(name, tplist, "class", False)

    def trait_base(self) -> str:
      name = self._name + "_base"
      decl = self._decl_trait(name)

      impl_ptr = CppVal("void *", "_gentrait_impl")

      vtable_list = []
      for f in self._func:
          plist = CppArgBuilder(self._tmpl).add_arg(impl_ptr).add_arg_list(f.args).fparam_list()
          fptr = TraitTmpl.func_ptr(f.ret, plist, f.noexcept)
          vtable_list.append(TraitTmpl.vtable_entry(f.vname, fptr))

      vtable_impl_list = []
      for f in self._func:
          plist = CppArgBuilder(self._tmpl).add_arg(impl_ptr).add_arg_list(f.args).fparam_list_named()
          fdecl = TraitTmpl.func_decl(f.ret, f.vname, plist, "", f.noexcept)
          args = self._wrap_args(f)
          farg = CppArgBuilder(self._tmpl).add_arg_list(args).farg_list()
          ret_expr = CppExprBuilder(impl_ptr).cast("_GENTRAIT_IMPL *").setptr().member(f.name).call(farg)
          vtable_impl_list.append(TraitTmpl.func_impl(fdecl, ret_expr, True))

      vtable_for_list = []
      for f in self._func:
          vtable_for_list.append(TraitTmpl.vtable_for_entry(f.vname))

      return TraitTmpl.trait_base.format(decl=decl,
                                         vtable_list="\n".join(vtable_list),
                                         vtable_impl_list="\n".join(vtable_impl_list),
                                         vtable_for_list="\n".join(vtable_for_list),
                                         name=name)

    def _build_impl(self, f: TraitFunc, impl_ptr: CppVal, vtbl: CppVal, vtbl_inplace: bool = False) -> str:
        plist = CppArgBuilder(self._tmpl).add_arg_list(f.args).fparam_list_named()
        fdecl = TraitTmpl.func_decl(f.ret, f.name, plist, f.cvref, f.noexcept)
        args = self._wrap_args(f)
        farg = CppArgBuilder(self._tmpl).add_arg(impl_ptr).add_arg_list(args).farg_list()
        ret_expr = CppExprBuilder(vtbl).setptr(not vtbl_inplace).member(f.vname).call(farg)
        return TraitTmpl.func_impl(fdecl, ret_expr, False)

    def trait_ref(self) -> str:
        if not "r" in self._gen:
            return ""

        name = self._name + "_ref"
        decl = self._decl_trait(name)
        base = self._specialise_base()

        tplist = CppArgBuilder(self._tmpl).tparam_list()
        friend_decl_unique = TraitTmpl.class_decl(self._name, tplist, "class", True)
        friend_decl_shared = TraitTmpl.class_decl(self._name + "_shared", tplist, "class", True)

        vtbl_type = "typename base::vtable_base" + ("" if self._inplace_ref else " const *")
        vtbl_get = "*vtbl" if self._inplace_ref else "vtbl"

        impl_ptr = CppVal("void *", "_gentrait_impl")
        vtbl = CppVal("?", "_gentrait_vtbl")
        trait_impl_list = []
        for f in self._func:
            trait_impl_list.append(self._build_impl(f, impl_ptr, vtbl, self._inplace_ref))

        return TraitTmpl.trait_ref.format(decl=decl,
                                          base=base,
                                          name=name,
                                          friend_decl_unique=friend_decl_unique,
                                          friend_decl_shared=friend_decl_shared,
                                          vtbl_type=vtbl_type,
                                          vtbl_get=vtbl_get,
                                          trait_list="\n".join(trait_impl_list))

    def trait_unique(self) -> str:
        if not "u" in self._gen:
            return ""

        name = self._name
        name_ref = self._specialise_trait(self._name + "_ref")
        decl = self._decl_trait(name)
        base = self._specialise_base()

        impl_ptr = CppVal("void *", "_gentrait_impl")
        vtbl = CppVal("?", "_gentrait_vtbl")
        trait_impl_list = []
        for f in self._func:
            trait_impl_list.append(self._build_impl(f, impl_ptr, vtbl, False))

        return TraitTmpl.trait_unique.format(decl=decl,
                                             base=base,
                                             name=name,
                                             name_ref=name_ref,
                                             trait_list="\n".join(trait_impl_list))

    def trait_shared(self) -> str:
        if not "s" in self._gen:
            return ""

        name = self._name + "_shared"
        name_ref = self._specialise_trait(self._name + "_ref")
        decl = self._decl_trait(name)
        base = self._specialise_base()

        impl_ptr = CppVal("void *", "_gentrait_impl.get()")
        vtbl = CppVal("?", "_gentrait_vtbl")
        trait_impl_list = []
        for f in self._func:
            trait_impl_list.append(self._build_impl(f, impl_ptr, vtbl, False))

        return TraitTmpl.trait_shared.format(decl=decl,
                                             base=base,
                                             name=name,
                                             name_ref=name_ref,
                                             trait_list="\n".join(trait_impl_list))

    def hash_ref(self) -> str:
        if not "r" in self._gen:
            return ""

        tplist = CppArgBuilder(self._tmpl).tparam_list_named()
        name = self._specialise_trait(self._name + "_ref", self._ns)
        ret_expr = CppExprBuilder("v").member("_gentrait_impl").called_by("std::hash<void *>()")
        return TraitTmpl.trait_hash.format(tplist=tplist, name=name, ret_expr=ret_expr)

    def hash_unique(self) -> str:
        if not "u" in self._gen:
            return ""

        tplist = CppArgBuilder(self._tmpl).tparam_list_named()
        name = self._specialise_trait(self._name, self._ns)
        ret_expr = CppExprBuilder("v").member("_gentrait_impl").called_by("std::hash<void *>()")
        return TraitTmpl.trait_hash.format(tplist=tplist, name=name, ret_expr=ret_expr)

    def hash_shared(self) -> str:
        if not "s" in self._gen:
            return ""

        tplist = CppArgBuilder(self._tmpl).tparam_list_named()
        name = self._specialise_trait(self._name + "_shared", self._ns)
        ret_expr = CppExprBuilder("v").member("_gentrait_impl").member("get").call().called_by("std::hash<void *>()")
        return TraitTmpl.trait_hash.format(tplist=tplist, name=name, ret_expr=ret_expr)

class GuardBuilder:
    """Generates include guard"""

    def __init__(self, unique_body: str):
        self._id = hashlib.sha1(unique_body.encode("utf-8")).hexdigest()

    def begin(self):
        return f"#ifndef INCLUDE_{self._id}\n#define INCLUDE_{self._id}\n"

    def end(self):
        return f"#endif // INCLUDE_{self._id}\n"

class IncludeBuilder:
    """Generates #include list"""

    def __init__(self, include_list: list[str]):
        s = {f"\"{i.strip()}\"" if not i.strip().startswith("<") else i.strip() for i in include_list} if include_list is not None else set()
        if "<memory>" not in s:
            s.add("<memory>")
        if "<functional>" not in s:
            s.add("<functional>")
        self._include = s

    def __str__(self) -> str:
        tmpl = "#include {inc}"
        return "\n".join(tmpl.format(inc=i) for i in self._include)

class GenTrait:
    def __init__(self, include: list[str], namespace: str, traits: list[dict]):
        self._include = IncludeBuilder(include)
        if namespace:
            self._namespace = namespace
            self._ns_begin = f"namespace {self._namespace} {{"
            self._ns_end = "}"
        else:
            self._namespace = ""
            self._ns_begin = ""
            self._ns_end = ""
        self._traits = []
        for t in traits:
            inplace_ref = t.get('inplace_ref', len(t['func']) == 1)
            self._traits.append(TraitBuilder(name=t['name'],
                                             template_list=[CppTparam.load_from(tmpl) for tmpl in t.get('template', [])],
                                             function_list=[TraitFunc.load_from(func) for func in t['func']],
                                             ns=self._namespace,
                                             inplace_ref=inplace_ref,
                                             gen=t.get('gen', [])))

    def __str__(self) -> str:
        trait_base_list = "\n".join([t.trait_base() for t in self._traits])
        trait_ref_list = "\n".join([t.trait_ref() for t in self._traits])
        trait_unique_list = "\n".join([t.trait_unique() for t in self._traits])
        trait_shared_list = "\n".join([t.trait_shared() for t in self._traits])
        hash_ref_list = "\n".join([t.hash_ref() for t in self._traits])
        hash_unique_list = "\n".join([t.hash_unique() for t in self._traits])
        hash_shared_list = "\n".join([t.hash_shared() for t in self._traits])
        guard = GuardBuilder(trait_base_list)
        return TraitTmpl.full.format(guard_begin=guard.begin(),
                                     include=self._include,
                                     ns_begin=self._ns_begin,
                                     trait_base_list=trait_base_list,
                                     trait_ref_list=trait_ref_list,
                                     trait_unique_list=trait_unique_list,
                                     trait_shared_list=trait_shared_list,
                                     ns_end=self._ns_end,
                                     hash_ref_list=hash_ref_list,
                                     hash_unique_list=hash_unique_list,
                                     hash_shared_list=hash_shared_list,
                                     guard_end=guard.end())


def main(filename):
    """Called directly from console, prints generated traits"""
    with open(filename, "r") as f:
        j = json.load(f)
    gen = GenTrait(j.get('include', []), j['namespace'], j['trait'])
    print(gen)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python3 gen_trait.py <json file>")
        sys.exit(1)
    else:
        main(sys.argv[1])
