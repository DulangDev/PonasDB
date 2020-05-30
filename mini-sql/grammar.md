#  Formal grammar of mini-sql

select <expr>

<expr> ::== <logor> || <logor> "or" <logor>
<logor> ::== <logand> || <logand> "and" <logand>
<logand> ::== <lognot> || "not " <lognot>
<lognot> ::== <symbol> { "==" || "<" || ">" } <value> || "(" <expr> ")"
<symbol> ::== <symbol>.<string> || <name>

