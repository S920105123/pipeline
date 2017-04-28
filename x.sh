if [ "$(cat golden/tmp)" != "" ]; then
	echo "YO"
else
	echo "$(cat golden/tmp)"
fi
