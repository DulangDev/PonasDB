#  Formal grammar of mini-sql

select <expr>

<expr> ::== <logand> || <logand> "or" <logand>
<logand> ::== <lognot> || "not " <lognot>
<lognot> ::== <symbol> { "==" || "<" || ">" } <value> || "(" <expr> ")"

