from typing import List, Optional

from gen_trait.lexer import Lexer, Token, TT
from gen_trait.gen_trait import GenTrait


class Parser:
    def __init__(self, tokens: List[Token]):
        self._t = tokens
        self._p = 0

    def _cur(self) -> Token:
        return self._t[self._p]

    def _pk(self, off: int = 0) -> Token:
        i = self._p + off
        return self._t[i] if i < len(self._t) else self._t[-1]

    def _eat(self, tp: TT = None, val: str = None) -> Token:
        t = self._cur()
        if tp and t.type != tp:
            raise SyntaxError(
                f"Expected {tp.name}, got {t.type.name} ('{t.value}') at line {t.line}"
            )
        if val is not None and t.value != val:
            raise SyntaxError(f"Expected '{val}', got '{t.value}' at line {t.line}")
        self._p += 1
        return t

    def _try(self, tp: TT, val: str = None) -> Optional[Token]:
        t = self._cur()
        if t.type == tp and (val is None or t.value == val):
            self._p += 1
            return t
        return None

    def parse(self) -> GenTrait:
        includes: List[str] = []
        namespace = ""
        traits: List[dict] = []

        while self._cur().type != TT.EOF:
            if self._cur().type == TT.INCLUDE:
                includes.append(self._eat().value)
            elif self._cur().type == TT.IDENT and self._cur().value == "namespace":
                namespace = self._namespace()
            else:
                traits.append(self._trait())

        return GenTrait(includes, namespace, traits)

    def _namespace(self) -> str:
        self._eat(TT.IDENT, "namespace")
        parts = [self._eat(TT.IDENT).value]
        while self._try(TT.COLONCOLON):
            parts.append(self._eat(TT.IDENT).value)
        self._eat(TT.SEMI)
        return "::".join(parts)

    def _trait(self) -> dict:
        template = []
        if self._cur().type == TT.IDENT and self._cur().value == "template":
            template = self._template()

        self._eat(TT.IDENT, "trait")
        name = self._eat(TT.IDENT).value
        attrs = self._attrs()
        self._eat(TT.LBRACE)

        funcs: List[dict] = []
        while self._cur().type != TT.RBRACE:
            funcs.append(self._func())

        self._eat(TT.RBRACE)
        self._eat(TT.SEMI)

        d: dict = {"name": name, "func": funcs}
        if template:
            d["template"] = template
        if "gen" in attrs:
            v = attrs["gen"]
            d["gen"] = [x.strip() for x in v.split(",")] if isinstance(v, str) else []
        if "inplace_ref" in attrs:
            v = attrs["inplace_ref"]
            if isinstance(v, bool):
                d["inplace_ref"] = v
            elif isinstance(v, str):
                d["inplace_ref"] = v.lower() not in ("false", "0")
            else:
                d["inplace_ref"] = True
        return d

    def _template(self) -> list:
        self._eat(TT.IDENT, "template")
        self._eat(TT.LANGLE)
        params = [self._tparam()]
        while self._try(TT.COMMA):
            params.append(self._tparam())
        self._eat(TT.RANGLE)
        return params

    def _tparam(self) -> dict:
        ptype = self._eat(TT.IDENT).value
        pack = bool(self._try(TT.ELLIPSIS))
        name = self._eat(TT.IDENT).value
        return {"type": ptype, "name": name, "pack": pack}

    def _func(self) -> dict:
        ret, name = self._func_header()
        self._eat(TT.LPAREN)
        args, wraps = self._params()
        self._eat(TT.RPAREN)
        cvref = self._cvref()
        fattrs = self._attrs()
        self._eat(TT.SEMI)

        fd: dict = {"name": name, "ret": ret, "args": args}
        if cvref:
            fd["cvref"] = cvref
        for i, w in enumerate(wraps):
            if w:
                fd["args"][i]["wrap"] = w
        return fd

    def _func_header(self):
        toks: List[Token] = []
        while self._cur().type != TT.LPAREN:
            toks.append(self._cur())
            self._p += 1

        if toks and toks[-1].type == TT.IDENT and toks[-1].value == "operator":
            self._p += 1
            self._eat(TT.RPAREN)
            return _to_str(toks[:-1]), "operator()"

        if not toks or toks[-1].type != TT.IDENT:
            raise SyntaxError(
                f"Expected function name at line {self._cur().line}"
            )

        return _to_str(toks[:-1]), toks[-1].value

    def _params(self):
        args: List[dict] = []
        wraps: List[str] = []
        if self._cur().type == TT.RPAREN:
            return args, wraps

        a, w = self._param()
        args.append(a)
        wraps.append(w)
        while self._try(TT.COMMA):
            a, w = self._param()
            args.append(a)
            wraps.append(w)
        return args, wraps

    def _param(self):
        toks: List[Token] = []

        while True:
            t = self._cur()
            if t.type == TT.RPAREN:
                break
            if t.type == TT.COMMA:
                break
            if t.type == TT.LBRACKET and self._pk(1).type == TT.LBRACKET:
                break

            if t.type == TT.LANGLE:
                toks.append(t)
                self._p += 1
                depth = 1
                while depth > 0:
                    c = self._cur()
                    if c.type == TT.LANGLE:
                        depth += 1
                    elif c.type == TT.RANGLE:
                        depth -= 1
                    toks.append(c)
                    self._p += 1
                continue

            if t.type == TT.ELLIPSIS:
                self._p += 1
                continue

            toks.append(t)
            self._p += 1

        if not toks:
            return {"name": "", "type": ""}, ""

        name_tok = toks.pop()
        type_toks, cvref = _extract_cvref(toks)
        type_str = _to_str(type_toks)

        d: dict = {"name": name_tok.value, "type": type_str}
        if cvref:
            d["cvref"] = cvref

        pa = self._attrs()
        wrap = pa.get("wrap", "") if isinstance(pa.get("wrap", ""), str) else ""

        return d, wrap

    def _cvref(self) -> str:
        parts: List[str] = []
        while self._cur().type == TT.IDENT and self._cur().value in (
            "const",
            "volatile",
            "mutable",
        ):
            parts.append(self._cur().value)
            self._p += 1
        if self._cur().type == TT.AMPAMP:
            parts.append("&&")
            self._p += 1
        elif self._cur().type == TT.AMP:
            parts.append("&")
            self._p += 1
        if self._cur().type == TT.IDENT and self._cur().value == "noexcept":
            parts.append("noexcept")
            self._p += 1
        return " ".join(parts)

    def _attrs(self) -> dict:
        if not (self._cur().type == TT.LBRACKET and self._pk(1).type == TT.LBRACKET):
            return {}

        self._p += 2
        attrs: dict = {}

        while not (self._cur().type == TT.RBRACKET and self._pk(1).type == TT.RBRACKET):
            name = self._eat(TT.IDENT).value

            if self._cur().type == TT.LPAREN:
                self._p += 1
                val_toks: List[Token] = []
                depth = 0
                while True:
                    t = self._cur()
                    if t.type == TT.LPAREN:
                        depth += 1
                    elif t.type == TT.RPAREN:
                        if depth == 0:
                            break
                        depth -= 1
                    val_toks.append(t)
                    self._p += 1
                self._eat(TT.RPAREN)
                attrs[name] = _to_str(val_toks)
            else:
                attrs[name] = True

            self._try(TT.COMMA)

        self._p += 2
        return attrs


def _extract_cvref(toks: List[Token]) -> tuple:
    cvref_parts: List[str] = []
    while toks:
        last = toks[-1]
        if last.type == TT.AMP:
            cvref_parts.insert(0, "&")
            toks.pop()
        elif last.type == TT.AMPAMP:
            cvref_parts.insert(0, "&&")
            toks.pop()
        elif last.type == TT.IDENT and last.value == "const":
            cvref_parts.insert(0, "const")
            toks.pop()
        else:
            break
    return toks, " ".join(cvref_parts)


def _to_str(toks: List[Token]) -> str:
    if not toks:
        return ""
    result = ""
    ns = False
    for tok in toks:
        if tok.type == TT.COLONCOLON:
            result += "::"
            ns = False
        elif tok.type == TT.LANGLE:
            result += "<"
            ns = False
        elif tok.type == TT.RANGLE:
            result += ">"
            ns = True
        elif tok.type == TT.COMMA:
            result += ","
            ns = True
        elif tok.type == TT.STAR:
            result += "*"
            ns = True
        elif tok.type == TT.AMP:
            result += "&"
            ns = True
        elif tok.type == TT.AMPAMP:
            result += "&&"
            ns = True
        else:
            if ns and result:
                result += " "
            result += tok.value
            ns = True
    return result


def parse_dsl(source: str) -> GenTrait:
    tokens = Lexer(source).tokenize()
    return Parser(tokens).parse()
