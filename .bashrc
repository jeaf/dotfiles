xcd_func() {
    ~/project/xcd/xcd.exe ~/.xcd.db ~/.xcd.dirchanger.sh "$@"
    . ~/.xcd.dirchanger.sh
}

alias e=vim
alias l="ls -lA --color --classify"
alias x=xcd_func

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

