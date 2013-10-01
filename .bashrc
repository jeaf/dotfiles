xcd_func() {
    ~/project/xcd/xcd.exe ~/.xcd.db ~/.xcd.dirchanger.sh "$@"
    . ~/.xcd.dirchanger.sh
}

alias e=vim
alias l="ls -lA --color --classify"
alias x=xcd_func

