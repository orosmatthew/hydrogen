$$
\begin{align}
    \text{Block} &\to
    \begin{cases}
        \text{[Stmt];[Block]} \\
        \text{[Control][Block]} \\
        \epsilon
    \end{cases} \\
    \text{Stmt} &\to
    \begin{cases}
        \text{[Expr]} \\
        \text{ident} = \text{[Expr]} \\
        \text{print}(\text{[Expr]}) \\
        \text{write}(\text{[Expr]},\text{[Expr]})\\
        \text{var} \space \text{ident} = \text{[Expr]} \\
        \text{break}\\
        \epsilon
    \end{cases} \\
    \text{Control} &\to
    \begin{cases}
        \text{if}\space(\text{[Expr]})\text{[Scope]}\text{[Else]} \\
        \text{while} (\text{[Expr]})\text{[Scope]} \\
        \text{[Scope]} \\
        \text{for}(\text{[Stmt];[Expr];[Stmt]})\text{[Scope]} \\
    \end{cases} \\
    \text{Scope} &\to \{\text{[StmtList]}\} \\
    \text{Else} &\to
    \begin{cases}
        \text{else}\text{[Scope]} \\
        \epsilon
    \end{cases} \\
    \text{Expr} &\to
    \begin{cases}
        \text{[Term]} \\
        \text{[ExprBin]}
    \end{cases} \\
    \text{ExprBin} &\to
    \begin{cases}
        \text{[Expr]}*\text{[Expr]} & \text{prec}=3 \\
        \text{[Expr]}\space/\space \text{[Expr]} & \text{prec}=3 \\
        \text{[Expr]}+\text{[Expr]} & \text{prec}=2 \\
        \text{[Expr]}-\text{[Expr]} & \text{prec}=2 \\
        \text{[Expr]}<\text{[Expr]} & \text{prec}=1 \\
        \text{[Expr]}>\text{[Expr]} & \text{prec}=1 \\
        \text{[Expr]}<=\text{[Expr]} & \text{prec}=1 \\
        \text{[Expr]}>=\text{[Expr]} & \text{prec}=1 \\
        \text{[Expr]}==\text{[Expr]} & \text{prec}=0 \\
        \text{[Expr]}\space!=\space\text{[Expr]} & \text{prec}=0
    \end{cases} \\
    \text{Term} &\to
    \begin{cases}
        -\text{[TermBase]} \\
        \text{[TermBase]}
    \end{cases} \\
    \text{TermBase} &\to
        \begin{cases}
        (\text{[Expr]}) \\
        \text{num} \\
        \text{ident}\text{[Post]} \\
        ++\text{ident} \\
        --\text{ident} \\
        \text{str} \\
        \text{true} \\
        \text{false}
    \end{cases} \\
    \text{Post} &\to
    \begin{cases}
        ++ \\
        -- \\
        \epsilon
    \end{cases}
\end{align}
$$
