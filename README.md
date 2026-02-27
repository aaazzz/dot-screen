Useful functions
===

```zsh

function sls() {
  local target
  target=$(screen -ls | awk '/Detached|Attached/ {print $1}' | fzf --prompt="attach> ")
  if [[ -n "$target" ]]; then
    screen -r "$target"
  else
    echo "No session selected."
  fi
}

function skill() {
  local target
  target=$(screen -ls | awk '/Detached|Attached/ {print $1}' | fzf --prompt="kill> ")
  [[ -n "$target" ]] && screen -X -S "$target" quit
}

function snew() {
  local name="$1"

  if [[ -z "$name" ]]; then
    read "name?Session name: "
  fi

  if [[ -z "$name" ]]; then
    echo "Cancelled."
    return 1
  fi

  screen -S "$name"
}
```
