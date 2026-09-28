#!/usr/bin/env bash
#
# Select which NuttX the BR Core firmware is built with.
#
#   nuttx.sh status     show which NuttX is checked out
#   nuttx.sh br         BeyondRobotix/NuttX with the BR fixes (the default,
#                       and the commit this branch records)
#   nuttx.sh upstream   the PX4/NuttX commit that the PX4 release pins
#
# Switching cleans the in-tree NuttX build and the BR Core build folders,
# because PX4 builds NuttX inside the source tree and would otherwise link
# stale objects. See README.md in this folder.

set -euo pipefail

# PX4/NuttX commit pinned by PX4 v1.17.0. Update when BR-Core moves to a new
# PX4 release.
UPSTREAM_URL=https://github.com/PX4/NuttX.git
UPSTREAM_COMMIT=fb2fadf6f599c1406f052db013efd00a2518e72c

BR_URL=https://github.com/BeyondRobotix/NuttX.git

root=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
nuttx_path=platforms/nuttx/NuttX/nuttx
nuttx="$root/$nuttx_path"

# The BR commit is whatever this branch records for the submodule
br_commit=$(git -C "$root" ls-tree HEAD "$nuttx_path" | awk '{print $3}')

current() {
	git -C "$nuttx" rev-parse HEAD
}

describe() {
	case "$1" in
	"$br_commit") echo "br ($BR_URL)" ;;
	"$UPSTREAM_COMMIT") echo "upstream ($UPSTREAM_URL)" ;;
	*) echo "other (neither the BR nor the upstream commit)" ;;
	esac
}

ensure_commit() {
	local name=$1 url=$2 commit=$3

	if ! git -C "$nuttx" remote get-url "$name" >/dev/null 2>&1; then
		git -C "$nuttx" remote add "$name" "$url"
	fi

	if ! git -C "$nuttx" cat-file -e "$commit^{commit}" 2>/dev/null; then
		echo "Fetching $commit from $url"
		git -C "$nuttx" fetch "$name" "$commit"
	fi
}

switch_to() {
	local name=$1 url=$2 commit=$3

	if [ -n "$(git -C "$nuttx" status --porcelain --untracked-files=no)" ]; then
		echo "error: $nuttx_path has uncommitted changes; commit or stash them first" >&2
		exit 1
	fi

	ensure_commit "$name" "$url" "$commit"
	git -C "$nuttx" -c advice.detachedHead=false checkout --quiet "$commit"

	echo "Cleaning the in-tree NuttX build and BR Core build folders"
	git -C "$nuttx" clean -dXfq
	git -C "$root/platforms/nuttx/NuttX/apps" clean -dXfq
	rm -rf "$root"/build/beyondrobotix_brcore_*

	echo "NuttX is now: $(describe "$commit") at ${commit:0:10}"
}

case "${1:-status}" in
status)
	echo "NuttX is: $(describe "$(current)") at $(current | cut -c1-10)"
	;;
br)
	switch_to br "$BR_URL" "$br_commit"
	;;
upstream)
	switch_to upstream "$UPSTREAM_URL" "$UPSTREAM_COMMIT"
	echo "Build with GIT_SUBMODULES_ARE_EVIL=1, otherwise PX4's submodule check stops"
	echo "the build (or, with CI=true, resets NuttX to the recorded commit):"
	echo "  GIT_SUBMODULES_ARE_EVIL=1 make beyondrobotix_brcore_default"
	echo "git status now shows $nuttx_path as modified. Do not commit that;"
	echo "run '$0 br' to return to the recorded commit."
	;;
*)
	sed -n '3,12p' "$0" | sed 's/^# \{0,1\}//'
	exit 1
	;;
esac
