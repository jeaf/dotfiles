gvim()
{
  /c/Python27/python.exe ~/script/gvim.py --remote-silent "$@";
}

git_commit()
{
  git commit "$@";
}

alias e=gvim
alias l="ls -lA --color --classify"
alias gb="git branch -a -v -v"
alias gc=git_commit
alias gd="git diff"
alias gr="git remote -v"
alias gs="git status"

alias tfx="cd /c/cae/pkg/mtfx_ver/main_2.2"

