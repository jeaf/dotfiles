xa()
{
    if [ "$1" ]; then
        echo $(realpath "$1") >> ~/.xcddirs
    fi
}

x()
{
    local sel_dir=$(cat ~/.xcddirs | fzf --query="$1" --select-1)
    if [ "$sel_dir" ]; then
        cd $sel_dir
    fi
}

xcd_func()
{
    ~/project/xcd/xcd.exe ~/.xcd.db ~/.xcd.dirchanger.sh "$@"
    . ~/.xcd.dirchanger.sh
}

alias e=vim
alias ga='git add'
alias gc='git commit'
alias gco='git checkout'
alias gd='git diff'
alias gp='git push'
alias gr='git remote'
alias gs='git status'
alias l='ls -l --color'
alias la='l -a'
alias vi=vim
alias x=xcd_func

export PATH="/usr/local/bin:/usr/bin"

set -o vi

