import hashlib
import json
import sys


class CppTmpl:
    vtable = """using _gen_trait_{iname}_vtbl_t = {ret} (*)({plist}){noexcept};
_gen_trait_{iname}_vtbl_t {iname};"""

    vtable_impl = """static {ret} {iname}({plist}){noexcept}{{ return static_cast<Impl *>(impl)->{name}({clist}); }}"""

    vtable_impl_any = """static {ret} {iname}({plist}){noexcept}{{ return {cast_ops}<{cast_type}>(impl){access_ops}{name}({clist}); }}"""

    vtable_for = """vtable_impl<Impl>::{iname},"""

    trait_base = """{decl} {{
struct vtable {{
{vtable_list}
void (*_gen_trait_destroy)(void *);
}};
template <typename Impl>
struct vtable_impl {{
{vtable_impl_list}
static void _gen_trait_destroy(void *impl) {{ delete static_cast<Impl *>(impl); }}
}};
template <typename Impl>
constexpr static vtable vtable_for{{
{vtable_for_list}
vtable_impl<Impl>::_gen_trait_destroy,
}};
template <typename Impl>
constexpr static bool not_relative = !std::is_base_of_v<{name}, std::decay_t<Impl>>;
template <typename T, template <typename...> typename TA>
struct is_specialization_of : std::false_type {{}};
template <template <typename...> typename T, typename... TA>
struct is_specialization_of<T<TA...>, T> : std::true_type {{}};
template <typename T>
constexpr static bool not_smartptr =
!(is_specialization_of<T, std::shared_ptr>::value ||
is_specialization_of<T, std::unique_ptr>::value);
}};"""

    trait_api = """{ret} {name}({plist}){cvref}{{ return vtbl->{iname}({clist}); }}"""

    trait_api_inplace = """{ret} {name}({plist}){cvref}{{ return vtbl({clist}); }}"""

    trait_any = """{decl} {{
struct vtable {{
{vtable_list}
}};
template <typename Impl>
struct vtable_impl {{
{vtable_impl_list}
}};
template <typename Impl>
constexpr static vtable vtable_for{{
{vtable_for_list}
}};
template <typename Impl>
constexpr static bool not_relative = !std::is_base_of_v<{name}, std::decay_t<Impl>>;
// TODO: std::any is not hashable, and hashing can't work when erased right?
mutable std::any impl;
vtable{ref_vtbl} vtbl;
public:
template <typename Impl>
{name}(Impl const &impl) noexcept : impl(impl), vtbl({ref_vtbl_init}) {{}}
void swap({name} &other) noexcept {{
impl.swap(other.impl);
std::swap(vtbl, other.vtbl);
}}
friend void swap({name} &lhs, {name} &rhs) noexcept {{ lhs.swap(rhs); }}
// TODO: operator== should be in vtable
{api_list}
}};"""

    trait_ref = """{decl} : {base} {{
using base = {base};
{friend_decl_unique};
{friend_decl_shared};
friend struct std::hash<{name}>;
void *impl;
typename base::vtable{ref_vtbl} vtbl;
{name}(void *impl, typename base::vtable const *vtbl) noexcept : impl(impl), vtbl({ref_vtbl_init}) {{}}
public:
{name}() noexcept = default;
template <typename Impl>
explicit {name}(Impl *impl) noexcept : {name}(impl, &base::template vtable_for<Impl>) {{}}
template <typename Impl>
explicit {name}(std::unique_ptr<Impl> const &impl) noexcept : {name}(impl.get(), &base::template vtable_for<Impl>) {{}}
template <typename Impl>
explicit {name}(std::shared_ptr<Impl> const &impl) noexcept : {name}(impl.get(), &base::template vtable_for<Impl>) {{}}
template <typename Impl,
typename = std::enable_if_t<base::template not_relative<Impl> &&
base::template not_smartptr<Impl>>>
{name}(Impl &impl) noexcept : {name}(std::addressof(impl), &base::template vtable_for<Impl>) {{}}
void swap({name} &other) noexcept {{
std::swap(impl, other.impl);
std::swap(vtbl, other.vtbl);
}}
friend void swap({name} &lhs, {name} &rhs) noexcept {{ lhs.swap(rhs); }}
friend bool operator==({name} const &lhs, {name} const &rhs) noexcept {{ return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl; }}

{api_list}
}};"""

    trait_unique = """{decl} : {base} {{
using base = {base};
friend struct std::hash<{name}>;
void *impl;
typename base::vtable const *vtbl;

public:
{name}({name} const &) = delete;
{name} &operator=({name} const &) = delete;
{name}({name} &&other) noexcept : impl(other.impl), vtbl(other.vtbl) {{ other.impl = nullptr; }}
{name} &operator=({name} &&other) noexcept {{
auto tmp(std::move(other));
tmp.swap(*this);
return *this;
}}
template <typename Impl>
explicit {name}(Impl *impl) noexcept : impl(impl), vtbl(&base::template vtable_for<Impl>) {{}}
template <typename Impl>
explicit {name}(std::unique_ptr<Impl> impl) noexcept : impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {{}}
template <typename Impl, typename = std::enable_if_t<base::template not_relative<Impl>>>
{name}(Impl &&impl) : impl(new std::remove_reference_t<Impl>(std::forward<Impl>(impl))),
vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {{}}
~{name}() {{ vtbl->_gen_trait_destroy(impl); impl = nullptr; }}
operator {trait_ref}() const {{ return {{impl, vtbl}}; }}
void swap({name} &other) noexcept {{
std::swap(impl, other.impl);
std::swap(vtbl, other.vtbl);
}}
friend void swap({name} &lhs, {name} &rhs) noexcept {{ lhs.swap(rhs); }}
friend bool operator==({name} const &lhs, {name} const &rhs) noexcept {{ return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl; }}

{api_list}
}};"""

    trait_shared = """{decl} : {base} {{
using base = {base};
friend struct std::hash<{name}>;
std::shared_ptr<void> impl;
typename base::vtable const *vtbl;

public:
template <typename Impl>
explicit {name}(Impl *impl) : impl(impl), vtbl(&base::template vtable_for<Impl>) {{}}
template <typename Impl>
explicit {name}(std::unique_ptr<Impl> impl) noexcept
: impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {{}}
template <typename Impl>
explicit {name}(std::shared_ptr<Impl> impl) noexcept : impl(impl), vtbl(&base::template vtable_for<Impl>) {{}}
template <typename Impl, typename = std::enable_if_t<base::template not_relative<Impl>>>
{name}(Impl &&impl) :
impl(std::make_shared<std::remove_reference_t<Impl>>(std::forward<Impl>(impl))), vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {{}}
operator {trait_ref}() const {{ return {{impl.get(), vtbl}}; }}
void swap({name} &other) noexcept {{
impl.swap(other.impl);
std::swap(vtbl, other.vtbl);
}}
friend void swap({name} &lhs, {name} &rhs) noexcept {{ lhs.swap(rhs); }}
friend bool operator==({name} const &lhs, {name} const &rhs) noexcept {{ return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl; }}

{api_list}
}};"""

    hash = """template <{tlist}> struct hash<{spec}>{{
size_t operator()({spec} const &v) const noexcept {{
return std::hash<void *>()(v.impl);
}}
}};"""

    hash_shared = """template <{tlist}> struct hash<{spec}>{{
size_t operator()({spec} const &v) const noexcept {{
return std::hash<void *>()(v.impl.get());
}}
}};"""

    full = """{guard_begin}
{pragma}
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


class IdentGenerator:
    """Generates some optionally templated identifiers"""

    Arg = dict[str, str]
    ArgList = list[Arg]

    def __init__(self, template_list):
        self.tlist = template_list
        self.has_tmpl = len(self.tlist) > 0
        self.tmap = {}  # type param: name -> template
        self.ntmap = {}  # non-type param: name -> template
        for item in template_list:
            if item['type'] == "typename" or item['type'] == "class":
                self.tmap[item['name']] = item
            else:
                self.ntmap[item['name']] = item
        self.has_t = len(self.tmap) > 0
        self.has_nt = len(self.ntmap) > 0

    def is_tmpl_t(self, arg: Arg) -> bool:
        """Check if an arg is type template"""
        return self.has_t and arg['type'] in self.tmap

    def is_tmpl_nt(self, arg: Arg) -> bool:
        """Check if an arg is non-type template"""
        return self.has_nt and arg['type'] in self.ntmap

    def is_pack(self, arg: Arg) -> bool:
        """Check if an arg is pack"""
        if not self.has_tmpl:
            return False
        # check type param
        if self.has_t and arg['type'] in self.tmap:
            elem = self.tmap[arg['type']]
            return elem.get('pack', False)
        # check non-type param
        if self.has_nt and arg['name'] in self.ntmap:
            elem = self.ntmap[arg['name']]
            return elem.get('pack', False)

    def trans_fparam(self, arg: Arg) -> str:
        """Transform an arg to function param"""
        if "cvref" not in arg or arg['cvref'] == "":
            tmpl = "{type}..." if self.is_pack(arg) else "{type}"
            return tmpl.format(type=arg['type'])
        else:
            tmpl = "{type} {cvref}..." if self.is_pack(arg) else "{type} {cvref}"
            return tmpl.format(type=arg['type'], cvref=arg['cvref'])

    def trans_fparam_named(self, arg: Arg) -> str:
        """Transform an arg to named function param"""
        if "cvref" not in arg or arg['cvref'] == "":
            tmpl = "{type}... {name}" if self.is_pack(arg) else "{type} {name}"
            return tmpl.format(type=arg['type'], name=arg['name'])
        else:
            tmpl = "{type} {cvref}... {name}" if self.is_pack(arg) else "{type} {cvref} {name}"
            return tmpl.format(type=arg['type'], cvref=arg['cvref'], name=arg['name'])

    def fparam_list(self, args: ArgList) -> str:
        """Generate function param list"""
        if len(args) == 0:
            return ""
        return ", ".join([self.trans_fparam(arg) for arg in args])

    def fparam_list_named(self, args: ArgList) -> str:
        """Generate named function param list"""
        if len(args) == 0:
            return ""
        return ", ".join([self.trans_fparam_named(arg) for arg in args])

    def trans_fcall(self, arg: Arg) -> str:
        """Transform an arg to fit function call expression"""
        if "wrap" not in arg or arg['wrap'] == "":
            tmpl = "{name}..." if self.is_pack(arg) else "{name}"
            return tmpl.format(name=arg['name'])
        else:
            tmpl = "{wrap}({name})..." if self.is_pack(arg) else "{wrap}({name})"
            return tmpl.format(wrap=arg['wrap'], name=arg['name'])

    def fcall_list(self, args: ArgList) -> str:
        """Generate function call arg list"""
        if len(args) == 0:
            return ""
        # NOTE: we should always forward template args unless user wraps explicitly
        for arg in args:
            if "warp" not in arg and self.is_tmpl_t(arg):
                # we don't need to forward const& but whatever, no harm
                arg['wrap'] = "static_cast<{type}&&>".format(type=arg['type'])
        return ", ".join([self.trans_fcall(arg) for arg in args])

    @staticmethod
    def expand_tpack(item: dict) -> str:
        return "..." if item.get('pack', False) else ""

    def tparam_list(self):
        """Generate template param list"""
        if not self.has_tmpl:
            return ""
        tmpl = "{t}{p}"
        return ", ".join([tmpl.format(t=t['type'], p=self.expand_tpack(t)) for t in self.tlist])

    def tparam_list_named(self):
        """Generate named template param list"""
        if not self.has_tmpl:
            return ""
        tmpl = "{t}{p} {n}"
        return ", ".join([tmpl.format(t=t['type'], p=self.expand_tpack(t), n=t['name']) for t in self.tlist])

    def targ_list(self):
        """Generate template arg list"""
        if not self.has_tmpl:
            return ""
        tmpl = "{n}{p}"
        return ", ".join([tmpl.format(n=t['name'], p=self.expand_tpack(t)) for t in self.tlist])

    def declare(self, name, decl="class", friend=False):
        """Generate a class/struct declaration"""
        if not self.has_tmpl:
            tmpl = "friend {decl} {name}" if friend else "{decl} {name}"
            return tmpl.format(name=name, decl=decl)
        if friend:
            tmpl = "template <{tparam_list}> friend {decl} {name}"
            return tmpl.format(tparam_list=self.tparam_list(), decl=decl, name=name)
        else:
            tmpl = "template <{tparam_list}> {decl} {name}"
            return tmpl.format(tparam_list=self.tparam_list_named(), decl=decl, name=name)

    def specialize(self, name, ns):
        """Generate a specialization"""
        if not self.has_tmpl:
            tmpl = "{ns}::{name}" if ns else "{name}"
            return tmpl.format(name=name, ns=ns)
        else:
            tmpl = "{ns}::{name}<{targ_list}>" if ns else "{name}<{targ_list}>"
            return tmpl.format(name=name, ns=ns, targ_list=self.targ_list())


class IncGuard:
    """Generates include guard"""

    def __init__(self, unique_body: str):
        self.id = hashlib.sha1(unique_body.encode("utf-8")).hexdigest()

    def begin(self):
        return f"#ifndef INCLUDE_{self.id}\n#define INCLUDE_{self.id}\n"

    def end(self):
        return f"#endif // INCLUDE_{self.id}\n"


class Pragma:
    """Generates #pragma list"""

    def __init__(self, pragma_list: list[str]):
        self.pragma_list = pragma_list

    def __str__(self):
        if len(self.pragma_list) == 0:
            return ""
        tmpl = "#pragma {pragma}"
        return "\n".join([tmpl.format(pragma=pragma) for pragma in self.pragma_list])


class Include:
    """Generates #include list, adds <memory> and <functional> if not present"""

    def __init__(self, include_list: list[str]):
        self.include_list = [f"\"{i}\"" if not i.startswith("<") else i for i in include_list]
        if "<memory>" not in self.include_list:
            self.include_list.append("<memory>")
        if "<functional>" not in self.include_list:
            self.include_list.append("<functional>")

    def __str__(self):
        tmpl = "#include {include}"
        return "\n".join([tmpl.format(include=include) for include in self.include_list])


class Trait:
    """Generates your traits"""

    def __init__(self, name: str, template_list: list[dict], function_list: list[dict], gen_list: list[str], ns: str):
        self.name = name
        self.id = IdentGenerator(template_list)
        self.func = function_list
        self.gen = []
        for g in gen_list:
            if len(g) == 0:
                continue
            gg = g[0].lower()
            if gg == "u" or gg == "s" or gg == "r":
                self.gen.append(gg)
        if len(self.gen) == 0:
            self.gen = ["u", "s", "r"]
        self.ns = ns

    @staticmethod
    def add_impl_ptr(args: list, shared_ptr=False):
        prepend = {"name": "impl.get()" if shared_ptr else "impl", "type": "void *"}
        return [prepend] + args

    @staticmethod
    def add_impl_any(args: list):
        prepend = {"name": "impl", "type": "std::any &"}
        return [prepend] + args

    @staticmethod
    def iname_for(f: dict) -> str:
        return f['name'] if f['name'] != "operator()" else "_gen_trait_invoke"

    def vtable(self, f: dict) -> str:
        """Generates a vtable entry"""
        noexcept = "noexcept" if "noexcept" in f.get('cvref', "") else ""
        plist = self.id.fparam_list(self.add_impl_ptr(f['args']))
        return CppTmpl.vtable.format(ret=f['ret'], iname=self.iname_for(f), plist=plist, noexcept=noexcept)

    def vtable_impl(self, f: dict) -> str:
        """Generates a vtable impl entry"""
        noexcept = "noexcept" if "noexcept" in f.get('cvref', "") else ""
        plist = self.id.fparam_list_named(self.add_impl_ptr(f['args']))
        clist = self.id.fcall_list(f['args'])
        return CppTmpl.vtable_impl.format(ret=f['ret'], iname=self.iname_for(f), name=f['name'], plist=plist, clist=clist, noexcept=noexcept)

    @staticmethod
    def vtable_for(f: dict) -> str:
        """Generate a vtable_for<Impl> entry"""
        return CppTmpl.vtable_for.format(iname=Trait.iname_for(f))

    def trait_base(self):
        """Generate trait base"""
        base_name = self.name + "_base"
        decl = self.id.declare(base_name, "struct")
        vtable_list = "\n".join([self.vtable(f) for f in self.func])
        vtable_impl_list = "\n".join([self.vtable_impl(f) for f in self.func])
        vtable_for_list = "\n".join([self.vtable_for(f) for f in self.func])
        return CppTmpl.trait_base.format(decl=decl,
                                         name=base_name,
                                         vtable_list=vtable_list,
                                         vtable_impl_list=vtable_impl_list,
                                         vtable_for_list=vtable_for_list)

    def trait_api(self, f: dict, shared: bool):
        """Generates a trait api entry"""
        plist = self.id.fparam_list_named(f['args'])
        clist = self.id.fcall_list(self.add_impl_ptr(f['args'], shared))
        cvref = f.get('cvref', "")
        return CppTmpl.trait_api.format(ret=f['ret'], name=f['name'], iname=self.iname_for(f), plist=plist, clist=clist, cvref=cvref)

    def trait_api_inplace(self, f: dict):
        """Generates a trait api call from in-place vtbl"""
        plist = self.id.fparam_list_named(f['args'])
        clist = self.id.fcall_list(self.add_impl_ptr(f['args'], False))
        cvref = f.get('cvref', "")
        return CppTmpl.trait_api_inplace.format(ret=f['ret'], name=f['name'], plist=plist, clist=clist, cvref=cvref)

    def trait_ref(self):
        """Generates a trait ref definition"""
        if "r" not in self.gen:
            return ""
        indirect = len(self.func) > 1
        name = self.name + "_ref"
        decl = self.id.declare(name, "class")
        base_name = self.name + "_base"
        base = self.id.specialize(base_name, "detail")
        friend_decl_unique = self.id.declare(self.name, "class", True)
        shared_name = self.name + "_shared"
        friend_decl_shared = self.id.declare(shared_name, "class", True)
        ref_vtbl = " const *" if indirect else "::_gen_trait_{iname}_vtbl_t ".format(iname=self.iname_for(self.func[0]))
        ref_vtbl_init = "vtbl" if indirect else "vtbl->{iname}".format(iname=self.iname_for(self.func[0]))
        if indirect:
            api_list = "\n".join([self.trait_api(f, False) for f in self.func])
        else:
            api_list = self.trait_api_inplace(self.func[0])
        return CppTmpl.trait_ref.format(name=name,
                                        decl=decl,
                                        base=base,
                                        friend_decl_unique=friend_decl_unique,
                                        friend_decl_shared=friend_decl_shared,
                                        ref_vtbl=ref_vtbl,
                                        ref_vtbl_init=ref_vtbl_init,
                                        api_list=api_list)

    def trait_unique(self):
        """Generates unique ownership trait definition"""
        if "u" not in self.gen:
            return ""
        name = self.name
        decl = self.id.declare(name, "class")
        base_name = self.name + "_base"
        base = self.id.specialize(base_name, "detail")
        ref_name = self.name + "_ref"
        trait_ref = self.id.specialize(ref_name, "")
        api_list = "\n".join([self.trait_api(f, False) for f in self.func])
        return CppTmpl.trait_unique.format(name=name,
                                           decl=decl,
                                           base=base,
                                           trait_ref=trait_ref,
                                           api_list=api_list)

    def trait_shared(self):
        """Generates shared ownership trait definition"""
        if "s" not in self.gen:
            return ""
        name = self.name + "_shared"
        decl = self.id.declare(name, "class")
        base_name = self.name + "_base"
        base = self.id.specialize(base_name, "detail")
        ref_name = self.name + "_ref"
        trait_ref = self.id.specialize(ref_name, "")
        api_list = "\n".join([self.trait_api(f, True) for f in self.func])
        return CppTmpl.trait_shared.format(name=name,
                                           decl=decl,
                                           base=base,
                                           trait_ref=trait_ref,
                                           api_list=api_list)

    def hash_ref(self):
        """Generates std::hash specialization for trait ref"""
        if "r" not in self.gen:
            return ""
        name = self.name + "_ref"
        tlist = self.id.tparam_list_named()
        spec = self.id.specialize(name, self.ns)
        return CppTmpl.hash.format(tlist=tlist, spec=spec)

    def hash_unique(self):
        """Generates std::hash specialization for unique trait"""
        if "u" not in self.gen:
            return ""
        name = self.name
        tlist = self.id.tparam_list_named()
        spec = self.id.specialize(name, self.ns)
        return CppTmpl.hash.format(tlist=tlist, spec=spec)

    def hash_shared(self):
        """Generates std::hash specialization for shared trait"""
        if "s" not in self.gen:
            return ""
        name = self.name + "_shared"
        tlist = self.id.tparam_list_named()
        spec = self.id.specialize(name, self.ns)
        return CppTmpl.hash_shared.format(tlist=tlist, spec=spec)


class GenTrait:
    def __init__(self, trait: list[dict], ns: str, pragma=None, include=None):
        if pragma is None:
            pragma = []
        if include is None:
            include = []
        self.pragma = Pragma(pragma)
        self.include = Include(include)
        if ns:
            self.ns = ns
            self.ns_begin = f"namespace {self.ns} {{"
            self.ns_end = "}"
        else:
            self.ns = ""
            self.ns_begin = ""
            self.ns_end = ""
        self.trait = [Trait(t['name'], t.get('template', []), t['func'], t.get('gen', ["u", "s", "r"]), self.ns) for t in trait]

    def __str__(self):
        trait_base_list = "\n".join([t.trait_base() for t in self.trait])
        trait_ref_list = "\n".join([t.trait_ref() for t in self.trait])
        trait_unique_list = "\n".join([t.trait_unique() for t in self.trait])
        trait_shared_list = "\n".join([t.trait_shared() for t in self.trait])
        hash_ref_list = "\n".join([t.hash_ref() for t in self.trait])
        hash_unique_list = "\n".join([t.hash_unique() for t in self.trait])
        hash_shared_list = "\n".join([t.hash_shared() for t in self.trait])
        guard = IncGuard(trait_base_list)
        return CppTmpl.full.format(guard_begin=guard.begin(),
                                   guard_end=guard.end(),
                                   pragma=self.pragma,
                                   include=self.include,
                                   ns_begin=self.ns_begin,
                                   ns_end=self.ns_end,
                                   trait_base_list=trait_base_list,
                                   trait_ref_list=trait_ref_list,
                                   trait_unique_list=trait_unique_list,
                                   trait_shared_list=trait_shared_list,
                                   hash_ref_list=hash_ref_list,
                                   hash_unique_list=hash_unique_list,
                                   hash_shared_list=hash_shared_list)


def main(filename):
    """Called directly from console, prints generated traits"""
    with open(filename, "r") as f:
        j = json.load(f)
    pragma = j.get('pragma', [])
    include = j.get('include', [])
    gen = GenTrait(j['trait'], j['namespace'], pragma, include)
    print(gen)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python3 gen_trait.py <json file>")
        sys.exit(1)
    else:
        main(sys.argv[1])
