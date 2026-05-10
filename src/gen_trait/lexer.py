from dataclasses import dataclass
from enum import Enum, auto
from typing import List


class TT(Enum):
    IDENT = auto()
    NUMBER = auto()
    INCLUDE = auto()
    LANGLE = auto()
    RANGLE = auto()
    LPAREN = auto()
    RPAREN = auto()
    LBRACE = auto()
    RBRACE = auto()
    LBRACKET = auto()
    RBRACKET = auto()
    SEMI = auto()
    COMMA = auto()
    COLONCOLON = auto()
    ELLIPSIS = auto()
    AMP = auto()
    AMPAMP = auto()
    STAR = auto()
    EQ = auto()
    EOF = auto()


@dataclass
class Token:
    type: TT
    value: str
    line: int
    col: int


class Lexer:
    def __init__(self, src: str):
        self._src = src
        self._pos = 0
        self._line = 1
        self._col = 1

    def _ch(self) -> str:
        return self._src[self._pos] if self._pos < len(self._src) else "\0"

    def _adv(self) -> str:
        c = self._src[self._pos]
        self._pos += 1
        if c == "\n":
            self._line += 1
            self._col = 1
        else:
            self._col += 1
        return c

    def _skip(self):
        while self._pos < len(self._src):
            c = self._ch()
            if c in " \t\r\n":
                self._adv()
            elif (
                c == "/"
                and self._pos + 1 < len(self._src)
                and self._src[self._pos + 1] == "/"
            ):
                while self._pos < len(self._src) and self._ch() != "\n":
                    self._adv()
            else:
                break

    def _ident(self) -> str:
        s = self._pos
        while self._pos < len(self._src) and (self._ch().isalnum() or self._ch() == "_"):
            self._adv()
        return self._src[s : self._pos]

    def _include_path(self) -> str:
        c = self._ch()
        if c == "<":
            self._adv()
            s = self._pos
            while self._ch() != ">":
                self._adv()
            path = "<" + self._src[s : self._pos] + ">"
            self._adv()
            return path
        elif c == '"':
            self._adv()
            s = self._pos
            while self._ch() != '"':
                self._adv()
            path = '"' + self._src[s : self._pos] + '"'
            self._adv()
            return path
        else:
            raise SyntaxError(
                f"Expected '<' or '\"' after #include at line {self._line}"
            )

    def tokenize(self) -> List[Token]:
        tokens: List[Token] = []
        while self._pos < len(self._src):
            self._skip()
            if self._pos >= len(self._src):
                break

            line, col = self._line, self._col
            c = self._ch()

            if c == "#":
                self._adv()
                self._skip()
                w = self._ident()
                if w == "include":
                    self._skip()
                    tokens.append(Token(TT.INCLUDE, self._include_path(), line, col))
                else:
                    raise SyntaxError(f"Unknown directive #{w} at line {line}")
            elif c.isalpha() or c == "_":
                tokens.append(Token(TT.IDENT, self._ident(), line, col))
            elif c.isdigit():
                s = self._pos
                while self._pos < len(self._src) and self._ch().isdigit():
                    self._adv()
                tokens.append(Token(TT.NUMBER, self._src[s : self._pos], line, col))
            elif c == "(":
                self._adv()
                tokens.append(Token(TT.LPAREN, "(", line, col))
            elif c == ")":
                self._adv()
                tokens.append(Token(TT.RPAREN, ")", line, col))
            elif c == "{":
                self._adv()
                tokens.append(Token(TT.LBRACE, "{", line, col))
            elif c == "}":
                self._adv()
                tokens.append(Token(TT.RBRACE, "}", line, col))
            elif c == "[":
                self._adv()
                tokens.append(Token(TT.LBRACKET, "[", line, col))
            elif c == "]":
                self._adv()
                tokens.append(Token(TT.RBRACKET, "]", line, col))
            elif c == ";":
                self._adv()
                tokens.append(Token(TT.SEMI, ";", line, col))
            elif c == ",":
                self._adv()
                tokens.append(Token(TT.COMMA, ",", line, col))
            elif c == "*":
                self._adv()
                tokens.append(Token(TT.STAR, "*", line, col))
            elif c == "=":
                self._adv()
                tokens.append(Token(TT.EQ, "=", line, col))
            elif c == ":":
                self._adv()
                if self._ch() == ":":
                    self._adv()
                    tokens.append(Token(TT.COLONCOLON, "::", line, col))
                else:
                    raise SyntaxError(f"Unexpected ':' at line {line}")
            elif c == ".":
                self._adv()
                if self._ch() == ".":
                    self._adv()
                    if self._ch() == ".":
                        self._adv()
                        tokens.append(Token(TT.ELLIPSIS, "...", line, col))
                    else:
                        raise SyntaxError(f"Unexpected '..' at line {line}")
                else:
                    raise SyntaxError(f"Unexpected '.' at line {line}")
            elif c == "&":
                self._adv()
                if self._ch() == "&":
                    self._adv()
                    tokens.append(Token(TT.AMPAMP, "&&", line, col))
                else:
                    tokens.append(Token(TT.AMP, "&", line, col))
            elif c == "<":
                self._adv()
                tokens.append(Token(TT.LANGLE, "<", line, col))
            elif c == ">":
                self._adv()
                tokens.append(Token(TT.RANGLE, ">", line, col))
            elif c == "~":
                self._adv()
                tokens.append(Token(TT.IDENT, "~", line, col))
            else:
                raise SyntaxError(
                    f"Unexpected '{c}' at line {line}, col {col}"
                )

        tokens.append(Token(TT.EOF, "", self._line, self._col))
        return tokens
