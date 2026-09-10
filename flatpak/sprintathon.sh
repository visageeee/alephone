#!/bin/sh

game_data="$(sprintathon-game-selector)" || exit 0
exec /app/bin/alephone "$game_data" "$@"
