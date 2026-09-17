#!/bin/sh
# Build and install R PointScroll on Haiku.

set -e

APP="R PointScroll"
FILTER="RPointScrollFilter"
APPS_DIR="$HOME/config/non-packaged/apps"
MENU_DIR="$HOME/config/non-packaged/data/deskbar/menu/Preferences"
FILTER_DIR="$HOME/config/non-packaged/add-ons/input_server/filters"

cd "$(dirname "$0")"

if [ "$(uname -s)" != "Haiku" ]; then
	echo "This builds a Haiku input_server add-on; run it on Haiku." >&2
	exit 1
fi

if [ "$1" = "--uninstall" ]; then
	rm -f "$FILTER_DIR/$FILTER" "$APPS_DIR/$APP" "$MENU_DIR/$APP"
	echo "Removed R PointScroll. The input server unloads the filter automatically."
	exit 0
fi

echo "Compiling input filter..."
g++ -O2 -nostart -o "$FILTER" PointScrollFilter.cpp \
	PointScrollSettings.cpp /system/servers/input_server -lbe -lroot

echo "Compiling settings application..."
g++ -O2 -o "$APP" PointScrollApp.cpp PointScrollSettings.cpp -lbe -lroot

rc -o PointScrollFilter.rsrc PointScrollFilter.rdef
xres -o "$FILTER" PointScrollFilter.rsrc
rc -o PointScrollApp.rsrc PointScrollApp.rdef
xres -o "$APP" PointScrollApp.rsrc
mimeset -f "$APP"
rm -f PointScrollFilter.rsrc PointScrollApp.rsrc

if [ "$1" = "--build-only" ]; then
	echo "Built ./$FILTER and ./$APP (not installed)."
	exit 0
fi

mkdir -p "$APPS_DIR" "$MENU_DIR" "$FILTER_DIR"

# Removing the old node lets input_server unload that image before replacement.
rm -f "$FILTER_DIR/$FILTER"
sleep 1
cp -f "$FILTER" "$FILTER_DIR/$FILTER"
cp -f "$APP" "$APPS_DIR/$APP"
ln -sf "$APPS_DIR/$APP" "$MENU_DIR/$APP"
rm -f "$FILTER" "$APP"

echo "Installed R PointScroll. Open Preferences -> R PointScroll to tune it."
