#!/bin/bash

# Colors
blue=$'\033[0;34m'
cyan=$'\033[1;96m'
reset=$'\033[0;39m'

# Avoid boring prefix in du/df/etc
cd "$HOME"

initial_used_space=$(df -h "$HOME" | grep -v 'Filesystem' | awk '{ printf("%f", $3) }')

echo "cleaning out some space..."
rm -rfv ~/Library/Caches/*
rm -rfv ~/Library/Containers/com.docker.docker/*
rm -rfv ~/Library/Containers/*
rm -rfv ~/Library/Application\ Support/discord/Cache/*
rm -rfv ~/.Trash/*
rm -rfv ~/.cache/*
rm -rfv ~/Library/Developer/CoreSimulator/*
rm -rfv ~/Library/Application\ Support/Slack/Service Worker/CacheStorage/*
rm -rfv ~/Library/Application\ Support/Code/CachedData/*


echo "mmhm, refreshing!"

df -h . | grep --color=always -E "Size|Used|Avail|Capacity|[0-9]*\.*[0-9]*Mi|[0-9]*\.*[0-9]*Gi|[0-9]+\.*[0-9]+% |$"

final_used_space=$(df -h "$HOME" | grep -v 'Filesystem' | awk '{ printf("%f", $3) }')
freed_space=$(printf "%.1f" $(echo -e "${initial_used_space} - ${final_used_space}" | bc))
echo -e "${blue}\nFreed space: ${cyan}${freed_space}Gi${reset}"
echo -e "${blue}Pro tip: use ${cyan}GrandPerspective${blue} (GUI, available in the MSC) or ${cyan}ncdu${blue} (terminal, available with brew) to show a deep scan of your space.${reset}"
